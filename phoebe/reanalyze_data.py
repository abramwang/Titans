#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import os
from datetime import datetime
import glob
import re

def reanalyze_data_structure():
    """重新分析数据结构"""
    
    print("=" * 80)
    print("重新分析华鑫证券Excel数据结构")
    print("=" * 80)
    
    # 获取所有Excel文件
    excel_files = glob.glob("./data/*.xlsx")
    
    # 重新分类文件
    summary_files = [f for f in excel_files if "业绩统计" in f]  # 汇总统计文件
    daily_files = [f for f in excel_files if re.search(r'\d{4}-\d{2}-\d{2}', f)]  # 包含日期的日交易文件
    
    print(f"发现文件类型:")
    print(f"- 业绩统计文件（汇总）: {len(summary_files)} 个")
    print(f"- 日交易明细文件: {len(daily_files)} 个")
    print()
    
    # 1. 首先分析业绩统计文件（汇总数据）
    if summary_files:
        analyze_summary_data(summary_files[0])
    
    # 2. 然后分析日交易明细文件
    analyze_daily_trading_files(daily_files)
    
    print("=" * 80)
    print("数据结构重新分析完成")
    print("=" * 80)

def analyze_summary_data(file_path):
    """分析业绩统计汇总文件"""
    print("1. 业绩统计文件分析（汇总数据）")
    print("-" * 50)
    print(f"文件: {os.path.basename(file_path)}")
    
    try:
        df = pd.read_excel(file_path)
        print(f"数据维度: {df.shape}")
        print(f"列名: {list(df.columns)}")
        
        # 显示数据范围
        if '日期' in df.columns:
            dates = pd.to_datetime(df['日期']).dropna()
            if not dates.empty:
                print(f"数据时间范围: {dates.min().date()} 到 {dates.max().date()}")
                print(f"覆盖天数: {len(dates)} 天")
        
        print("\n业绩统计文件数据样例（前5行）:")
        print(df.head().to_string())
        
        print("\n业绩统计文件数据类型:")
        for col in df.columns:
            non_null_count = df[col].count()
            print(f"  {col}: {df[col].dtype} (有效数据: {non_null_count})")
        
        # 分析关键指标
        if '累计盈利' in df.columns:
            cumulative_profit = df['累计盈利'].dropna()
            if not cumulative_profit.empty:
                print(f"\n关键指标:")
                print(f"  最终累计盈利: {cumulative_profit.iloc[-1]:,.2f} 元")
                print(f"  最大累计盈利: {cumulative_profit.max():,.2f} 元")
                print(f"  最小累计盈利: {cumulative_profit.min():,.2f} 元")
        
    except Exception as e:
        print(f"分析业绩统计文件出错: {e}")
    
    print()

def analyze_daily_trading_files(daily_files):
    """分析日交易明细文件"""
    print("2. 日交易明细文件分析")
    print("-" * 50)
    
    if not daily_files:
        print("未发现日交易明细文件")
        return
    
    print(f"发现 {len(daily_files)} 个日交易明细文件")
    
    # 按日期排序
    daily_files_with_dates = []
    for file in daily_files:
        date_match = re.search(r'(\d{4}-\d{2}-\d{2})', file)
        if date_match:
            date_str = date_match.group(1)
            daily_files_with_dates.append((file, date_str))
    
    daily_files_with_dates.sort(key=lambda x: x[1])
    
    if daily_files_with_dates:
        print(f"时间范围: {daily_files_with_dates[0][1]} 到 {daily_files_with_dates[-1][1]}")
    
    # 分析几个典型的日交易文件
    print("\n分析典型日交易文件:")
    
    sample_files = daily_files_with_dates[:3]  # 分析前3个文件
    
    total_securities = set()
    total_trades = 0
    
    for file_path, date_str in sample_files:
        print(f"\n--- {date_str} ({os.path.basename(file_path)}) ---")
        
        try:
            df = pd.read_excel(file_path)
            
            # 查找表头行
            header_row = find_trading_header_row(df)
            if header_row is not None:
                # 重新读取，跳过无关行
                df = pd.read_excel(file_path, skiprows=header_row)
                df = df.dropna(how='all')
                
                print(f"原始数据维度: {df.shape}")
                
                # 提取有效的交易记录
                valid_trades = extract_valid_trades(df)
                
                if valid_trades:
                    print(f"有效交易记录: {len(valid_trades)} 条")
                    total_trades += len(valid_trades)
                    
                    # 统计涉及的证券
                    securities_in_file = set([trade['security_code'] for trade in valid_trades])
                    total_securities.update(securities_in_file)
                    print(f"涉及证券: {len(securities_in_file)} 只")
                    
                    # 显示交易样例
                    print("交易记录样例:")
                    for i, trade in enumerate(valid_trades[:3]):
                        print(f"  {i+1}. {trade['security_code']} {trade.get('security_name', '')} - 利润: {trade.get('profit', 'N/A')}")
                    
                    # 分析盈亏分布
                    profits = [trade.get('profit') for trade in valid_trades if trade.get('profit') is not None]
                    if profits:
                        total_profit = sum(profits)
                        profitable_count = len([p for p in profits if p > 0])
                        print(f"当日盈亏: 总计 {total_profit:,.2f} 元, 盈利交易 {profitable_count}/{len(profits)} 笔")
                else:
                    print("未找到有效交易记录")
                    
                # 查找汇总信息
                summary_info = extract_daily_summary(df, file_path)
                if summary_info:
                    print(f"当日汇总: 结算盈亏 {summary_info.get('settlement_pnl', 'N/A')}, 浮动盈亏 {summary_info.get('floating_pnl', 'N/A')}")
            
        except Exception as e:
            print(f"分析文件 {date_str} 出错: {e}")
    
    print(f"\n整体统计:")
    print(f"  总交易记录: {total_trades} 条")
    print(f"  涉及证券: {len(total_securities)} 只")
    print(f"  证券代码示例: {list(total_securities)[:10]}")

def find_trading_header_row(df):
    """查找交易明细的表头行"""
    for i, row in df.iterrows():
        # 查找包含"证券代码"的行
        if '证券代码' in str(row.iloc[0]):
            return i
    return None

def extract_valid_trades(df):
    """提取有效的交易记录"""
    trades = []
    
    for i, row in df.iterrows():
        if i == 0:  # 跳过表头
            continue
            
        # 检查证券代码
        security_code = str(row.iloc[0]).strip()
        if not is_valid_security_code(security_code):
            continue
        
        # 提取交易信息
        trade = {
            'security_code': security_code,
            'security_name': str(row.iloc[1]) if len(row) > 1 and not pd.isna(row.iloc[1]) else '',
            'buy_price': safe_float(row.iloc[2]) if len(row) > 2 else None,
            'buy_quantity': safe_int(row.iloc[3]) if len(row) > 3 else None,
            'sell_price': safe_float(row.iloc[4]) if len(row) > 4 else None,
            'sell_quantity': safe_int(row.iloc[5]) if len(row) > 5 else None,
            'current_price': safe_float(row.iloc[6]) if len(row) > 6 else None,
            'current_market_value': safe_float(row.iloc[7]) if len(row) > 7 else None,
            'cost_amount': safe_float(row.iloc[8]) if len(row) > 8 else None,
            'profit': safe_float(row.iloc[9]) if len(row) > 9 else None
        }
        trades.append(trade)
    
    return trades

def extract_daily_summary(df, file_path):
    """提取日汇总信息"""
    summary = {}
    
    # 从列名中查找
    for col in df.columns:
        if '结算盈亏' in str(col):
            for i, row in df.iterrows():
                if not pd.isna(row[col]):
                    summary['settlement_pnl'] = safe_float(row[col])
                    break
        elif isinstance(col, (int, float)) and col != 0:
            # 数字列名可能是浮动盈亏
            for i, row in df.iterrows():
                if not pd.isna(row[col]) and isinstance(row[col], (int, float)):
                    if 'floating_pnl' not in summary:
                        summary['floating_pnl'] = safe_float(row[col])
                    break
    
    return summary

def is_valid_security_code(code):
    """检查证券代码是否有效"""
    if not code or code == 'nan':
        return False
    # 中国证券代码格式：6位数字.交易所代码
    pattern = r'^\d{6}\.(SH|SZ)$'
    return bool(re.match(pattern, code))

def safe_float(value):
    """安全转换为浮点数"""
    if pd.isna(value):
        return None
    try:
        if isinstance(value, str):
            value = value.replace(',', '').replace(' ', '')
        return float(value)
    except:
        return None

def safe_int(value):
    """安全转换为整数"""
    if pd.isna(value):
        return None
    try:
        if isinstance(value, str):
            value = value.replace(',', '').replace(' ', '')
        return int(float(value))
    except:
        return None

if __name__ == "__main__":
    reanalyze_data_structure()
