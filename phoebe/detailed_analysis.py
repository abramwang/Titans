#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import os
from datetime import datetime
import glob
import re

def clean_and_analyze_excel():
    """清理并分析Excel文件数据"""
    
    # 获取所有Excel文件
    excel_files = glob.glob("./data/*.xlsx")
    
    # 分类文件
    daily_files = [f for f in excel_files if "业绩估算" in f]
    summary_files = [f for f in excel_files if "业绩统计" in f]
    
    print(f"发现 {len(daily_files)} 个日业绩估算文件")
    print(f"发现 {len(summary_files)} 个业绩统计文件")
    
    # 分析业绩统计文件
    if summary_files:
        print("\n=== 业绩统计文件分析 ===")
        analyze_summary_file(summary_files[0])
    
    # 分析业绩估算文件
    print("\n=== 业绩估算文件分析 ===")
    analyze_daily_files(daily_files[:3])  # 分析前3个文件

def analyze_summary_file(file_path):
    """分析业绩统计文件"""
    try:
        df = pd.read_excel(file_path)
        print(f"文件: {os.path.basename(file_path)}")
        print(f"数据形状: {df.shape}")
        print(f"列名: {list(df.columns)}")
        
        # 清理数据
        cleaned_df = df.dropna(how='all')
        print(f"清理后行数: {len(cleaned_df)}")
        
        # 显示数据样例
        print("\n数据样例:")
        for i, row in cleaned_df.head(10).iterrows():
            print(f"行{i}: {dict(row)}")
            
    except Exception as e:
        print(f"分析业绩统计文件出错: {e}")

def analyze_daily_files(files):
    """分析日业绩估算文件"""
    
    for file_path in files:
        print(f"\n--- 分析文件: {os.path.basename(file_path)} ---")
        
        try:
            # 提取日期
            date_match = re.search(r'(\d{4}-\d{2}-\d{2})', file_path)
            trade_date = date_match.group(1) if date_match else "未知日期"
            print(f"交易日期: {trade_date}")
            
            # 读取Excel文件
            df = pd.read_excel(file_path)
            
            # 查找数据起始行
            header_row = find_header_row(df)
            if header_row is not None:
                print(f"找到表头行: {header_row}")
                # 重新读取，跳过无关行
                df = pd.read_excel(file_path, skiprows=header_row)
                df = df.dropna(how='all')  # 删除全空行
                
                # 清理列名
                df.columns = [clean_column_name(col, i) for i, col in enumerate(df.columns)]
                
                print(f"清理后列名: {list(df.columns)}")
                print(f"数据行数: {len(df)}")
                
                # 提取证券持仓数据
                holdings_data = extract_holdings_data(df)
                if holdings_data:
                    print(f"提取到 {len(holdings_data)} 条持仓记录")
                    print("持仓数据样例:")
                    for i, holding in enumerate(holdings_data[:5]):
                        print(f"  {i+1}: {holding}")
                
                # 提取盈亏数据
                pnl_data = extract_pnl_data(df, file_path)
                if pnl_data:
                    print(f"盈亏数据: {pnl_data}")
                    
        except Exception as e:
            print(f"分析文件出错: {e}")

def find_header_row(df):
    """查找包含'证券代码'的表头行"""
    for i, row in df.iterrows():
        if '证券代码' in str(row.iloc[0]):
            return i
    return None

def clean_column_name(col, index):
    """清理列名"""
    if pd.isna(col) or str(col).startswith('Unnamed'):
        # 根据位置推断列名
        if index == 0:
            return '证券代码'
        elif index == 1:
            return '证券名称'
        elif index == 2:
            return '买入价格'
        elif index == 3:
            return '买入数量'
        elif index == 4:
            return '卖出价格'
        elif index == 5:
            return '卖出数量'
        elif index == 6:
            return '当前价格'
        elif index == 7:
            return '当前市值'
        elif index == 8:
            return '成本金额'
        elif index == 9:
            return '利润'
        else:
            return f'列{index}'
    return str(col)

def extract_holdings_data(df):
    """提取持仓数据"""
    holdings = []
    
    for i, row in df.iterrows():
        # 跳过表头行和空行
        if i == 0 or pd.isna(row.iloc[0]):
            continue
            
        # 检查是否为证券代码格式
        code = str(row.iloc[0]).strip()
        if not is_valid_security_code(code):
            continue
            
        holding = {
            'security_code': code,
            'security_name': str(row.iloc[1]) if not pd.isna(row.iloc[1]) else '',
            'buy_price': parse_number(row.iloc[2]),
            'buy_quantity': parse_number(row.iloc[3]),
            'sell_price': parse_number(row.iloc[4]) if len(row) > 4 else None,
            'sell_quantity': parse_number(row.iloc[5]) if len(row) > 5 else None,
            'current_price': parse_number(row.iloc[6]) if len(row) > 6 else None,
            'current_market_value': parse_number(row.iloc[7]) if len(row) > 7 else None,
            'cost_amount': parse_number(row.iloc[8]) if len(row) > 8 else None,
            'profit': parse_number(row.iloc[9]) if len(row) > 9 else None
        }
        holdings.append(holding)
    
    return holdings

def extract_pnl_data(df, file_path):
    """提取盈亏数据"""
    pnl_data = {}
    
    # 从文件名或列名中提取盈亏信息
    for col in df.columns:
        if '结算盈亏' in str(col):
            # 查找结算盈亏值
            for i, row in df.iterrows():
                if not pd.isna(row[col]):
                    pnl_data['settlement_pnl'] = parse_number(row[col])
                    break
        elif '浮动盈亏' in str(col) or isinstance(col, (int, float)):
            # 数字列名可能是浮动盈亏
            for i, row in df.iterrows():
                if not pd.isna(row[col]) and isinstance(row[col], (int, float)):
                    if 'floating_pnl' not in pnl_data:
                        pnl_data['floating_pnl'] = parse_number(row[col])
                    break
    
    return pnl_data

def is_valid_security_code(code):
    """检查是否为有效的证券代码"""
    if not code or code == 'nan':
        return False
    # 中国证券代码格式：6位数字.交易所代码
    pattern = r'^\d{6}\.(SH|SZ)$'
    return bool(re.match(pattern, code))

def parse_number(value):
    """解析数字，处理各种格式"""
    if pd.isna(value):
        return None
    
    try:
        # 移除逗号和其他非数字字符
        if isinstance(value, str):
            value = value.replace(',', '').replace(' ', '')
            # 如果包含非数字字符，尝试提取数字
            numbers = re.findall(r'-?\d+\.?\d*', value)
            if numbers:
                return float(numbers[0])
            return None
        return float(value)
    except:
        return None

if __name__ == "__main__":
    clean_and_analyze_excel()
