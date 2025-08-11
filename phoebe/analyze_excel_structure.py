#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import os
import glob
import re
from datetime import datetime

def analyze_excel_four_sections():
    """
    分析Excel文件中的4个数据部分：
    1. 日内底仓T0 (交易数据)
    2. ETF日内申赎 (交易数据)  
    3. 隔夜底仓 (持仓数据)
    4. ETF赎回涨停票 (持仓数据)
    """
    
    print("=" * 80)
    print("华鑫证券Excel数据结构详细分析 - 4个数据部分")
    print("=" * 80)
    
    # 获取所有日期Excel文件
    excel_files = glob.glob("./data/*2025-*.xlsx")
    excel_files.sort()
    
    print(f"发现 {len(excel_files)} 个日期Excel文件")
    print()
    
    if not excel_files:
        print("未发现包含日期的Excel文件")
        return
    
    # 分析前3个文件的详细结构
    sample_files = excel_files[:3]
    
    for file_path in sample_files:
        date_match = re.search(r'(\d{4}-\d{2}-\d{2})', file_path)
        date_str = date_match.group(1) if date_match else "未知日期"
        
        print(f"{'='*20} {date_str} {'='*20}")
        print(f"文件: {os.path.basename(file_path)}")
        
        analyze_single_file_sections(file_path)
        print()
    
    print("=" * 80)
    print("Excel 4个数据部分结构分析完成")
    print("=" * 80)

def analyze_single_file_sections(file_path):
    """分析单个Excel文件的4个数据部分"""
    
    try:
        # 读取整个Excel文件
        df = pd.read_excel(file_path)
        print(f"Excel总体维度: {df.shape}")
        
        # 定义4个数据部分的关键词
        section_keywords = {
            "日内底仓T0": ["日内底仓T0", "日内底仓", "T0"],
            "ETF日内申赎": ["ETF日内申赎", "ETF申赎", "日内申赎"],
            "隔夜底仓": ["隔夜底仓", "隔夜持仓"],
            "ETF赎回涨停票": ["ETF赎回涨停票", "赎回涨停票", "涨停票"]
        }
        
        # 寻找各个数据部分的起始位置
        section_positions = find_section_positions(df, section_keywords)
        
        if section_positions:
            print(f"识别到 {len(section_positions)} 个数据部分:")
            for section_name, start_row in section_positions:
                print(f"  - {section_name}: 第{start_row+1}行开始")
            print()
            
            # 分析每个数据部分
            analyze_each_section(df, section_positions)
        else:
            print("未能识别到标准的4个数据部分，尝试智能分析...")
            smart_analyze_sections(df)
            
    except Exception as e:
        print(f"文件分析出错: {e}")

def find_section_positions(df, section_keywords):
    """寻找各个数据部分在Excel中的位置"""
    section_positions = []
    
    for idx, row in df.iterrows():
        # 将整行转为字符串进行匹配
        row_text = ' '.join([str(cell) for cell in row if pd.notna(cell)]).strip()
        
        # 检查是否匹配任何部分的关键词
        for section_name, keywords in section_keywords.items():
            for keyword in keywords:
                if keyword in row_text:
                    section_positions.append((section_name, idx))
                    break
            else:
                continue
            break
    
    return section_positions

def analyze_each_section(df, section_positions):
    """分析每个识别到的数据部分"""
    
    for i, (section_name, start_row) in enumerate(section_positions):
        # 确定数据部分的结束位置
        if i + 1 < len(section_positions):
            end_row = section_positions[i + 1][1]
        else:
            end_row = len(df)
        
        print(f"【{section_name}】分析:")
        print(f"  数据范围: 第{start_row+1}行 到 第{end_row}行")
        
        # 提取该部分的数据
        section_df = df.iloc[start_row:end_row].copy()
        
        # 分析数据类型
        if section_name in ["日内底仓T0", "ETF日内申赎"]:
            analyze_trading_section(section_df, section_name)
        else:  # 隔夜底仓, ETF赎回涨停票
            analyze_position_section(section_df, section_name)
        
        print()

def analyze_trading_section(section_df, section_name):
    """分析交易数据部分"""
    print(f"  数据类型: 交易数据")
    
    # 寻找数据行（排除标题和空行）
    data_rows = []
    securities = set()
    profits = []
    quantities = []
    
    for idx, row in section_df.iterrows():
        if row.isna().all():
            continue
            
        # 检查是否包含证券代码
        has_security = False
        has_numeric_data = False
        
        for cell in row:
            if pd.isna(cell):
                continue
                
            cell_str = str(cell)
            
            # 检查证券代码模式
            if re.match(r'\d{6}\.(SH|SZ)', cell_str):
                securities.add(cell_str)
                has_security = True
            
            # 检查数值数据
            if isinstance(cell, (int, float)):
                has_numeric_data = True
                # 收集可能的利润数据
                if abs(cell) > 0.01 and abs(cell) < 1000000:
                    if abs(cell) < 100000:  # 可能是利润
                        profits.append(cell)
                    else:  # 可能是数量或金额
                        quantities.append(cell)
        
        if has_security and has_numeric_data:
            data_rows.append(row)
    
    # 输出分析结果
    print(f"  有效交易记录: {len(data_rows)} 条")
    if securities:
        print(f"  涉及证券: {len(securities)} 只")
        print(f"  证券代码: {sorted(list(securities))[:5]}{'...' if len(securities) > 5 else ''}")
    
    if profits:
        total_profit = sum(profits)
        profitable_count = len([p for p in profits if p > 0])
        print(f"  交易盈亏: 总计 {total_profit:,.2f} 元")
        print(f"  盈亏分布: {profitable_count} 笔盈利, {len(profits)-profitable_count} 笔亏损")
    
    if quantities:
        print(f"  交易规模: 涉及金额/数量 {len(quantities)} 笔")

def analyze_position_section(section_df, section_name):
    """分析持仓数据部分"""
    print(f"  数据类型: 持仓数据")
    
    # 寻找持仓数据
    position_rows = []
    securities = set()
    market_values = []
    quantities = []
    
    for idx, row in section_df.iterrows():
        if row.isna().all():
            continue
            
        has_security = False
        has_numeric_data = False
        
        for cell in row:
            if pd.isna(cell):
                continue
                
            cell_str = str(cell)
            
            # 检查证券代码
            if re.match(r'\d{6}\.(SH|SZ)', cell_str):
                securities.add(cell_str)
                has_security = True
            
            # 检查数值数据
            if isinstance(cell, (int, float)) and cell != 0:
                has_numeric_data = True
                if abs(cell) > 1000:  # 可能是市值
                    market_values.append(cell)
                elif abs(cell) > 0:  # 可能是数量
                    quantities.append(cell)
        
        if has_security and has_numeric_data:
            position_rows.append(row)
    
    # 输出分析结果
    print(f"  持仓记录: {len(position_rows)} 条")
    if securities:
        print(f"  持仓证券: {len(securities)} 只")
        print(f"  证券代码: {sorted(list(securities))[:5]}{'...' if len(securities) > 5 else ''}")
    
    if market_values:
        total_value = sum([v for v in market_values if v > 0])
        print(f"  持仓市值: 约 {total_value:,.2f} 元")
    
    if quantities:
        print(f"  持仓数量: {len(quantities)} 笔持仓")

def smart_analyze_sections(df):
    """智能分析数据部分（当无法识别标准分段时）"""
    print("智能分析模式:")
    
    # 寻找包含证券代码的行
    security_rows = []
    for idx, row in df.iterrows():
        for cell in row:
            if pd.notna(cell) and isinstance(cell, str):
                if re.match(r'\d{6}\.(SH|SZ)', cell):
                    security_rows.append(idx)
                    break
    
    if security_rows:
        print(f"  发现 {len(security_rows)} 行包含证券代码")
        
        # 分析数据密度分布
        dense_regions = analyze_data_density(df, security_rows)
        
        if dense_regions:
            print(f"  识别到 {len(dense_regions)} 个数据区域:")
            for i, (start, end, data_type) in enumerate(dense_regions):
                print(f"    区域{i+1}: 第{start+1}-{end+1}行, 推测类型: {data_type}")
    else:
        print("  未发现证券代码，可能需要手动检查数据格式")

def analyze_data_density(df, security_rows):
    """分析数据密度，推测不同的数据区域"""
    regions = []
    
    if not security_rows:
        return regions
    
    # 按连续性分组
    current_start = security_rows[0]
    current_end = security_rows[0]
    
    for i in range(1, len(security_rows)):
        if security_rows[i] - security_rows[i-1] <= 5:  # 间隔不超过5行认为是连续的
            current_end = security_rows[i]
        else:
            # 分析当前区域
            data_type = guess_data_type(df, current_start, current_end)
            regions.append((current_start, current_end, data_type))
            
            current_start = security_rows[i]
            current_end = security_rows[i]
    
    # 添加最后一个区域
    data_type = guess_data_type(df, current_start, current_end)
    regions.append((current_start, current_end, data_type))
    
    return regions

def guess_data_type(df, start_row, end_row):
    """根据数据特征推测数据类型"""
    section_df = df.iloc[start_row:end_row+1]
    
    # 统计数值类型的分布
    large_values = 0  # 大数值（可能是市值）
    small_values = 0  # 小数值（可能是利润）
    
    for idx, row in section_df.iterrows():
        for cell in row:
            if isinstance(cell, (int, float)) and cell != 0:
                if abs(cell) > 10000:
                    large_values += 1
                elif abs(cell) > 0.01:
                    small_values += 1
    
    # 根据数值分布推测类型
    if large_values > small_values:
        return "持仓数据（包含大额市值）"
    elif small_values > 0:
        return "交易数据（包含损益）"
    else:
        return "未知类型数据"

if __name__ == "__main__":
    analyze_excel_four_sections()
