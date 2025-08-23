#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import sys
import os

def analyze_excel():
    """分析Excel文件结构"""
    file_path = "/root/titans_github/phoebe/data/华鑫_314000045768_业绩估算_2025-07-22.xlsx"
    
    try:
        # 获取所有sheet
        excel_file = pd.ExcelFile(file_path)
        print(f"文件: {file_path}")
        print(f"Sheet列表: {excel_file.sheet_names}")
        
        # 查看每个sheet的内容
        for sheet_name in excel_file.sheet_names:
            print(f"\n=== Sheet: {sheet_name} ===")
            df = pd.read_excel(file_path, sheet_name=sheet_name)
            print(f"形状: {df.shape}")
            print(f"列名: {list(df.columns)}")
            
            # 显示前几行
            print("前5行数据:")
            print(df.head())
            
    except Exception as e:
        print(f"分析失败: {str(e)}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    analyze_excel()
