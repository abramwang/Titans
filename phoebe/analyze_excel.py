#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import os
from datetime import datetime
import glob

def analyze_excel_files():
    """分析data文件夹中的所有Excel文件"""
    
    # 获取所有Excel文件
    excel_files = glob.glob("./data/*.xlsx")
    print(f"找到 {len(excel_files)} 个Excel文件:")
    
    for file_path in excel_files:
        print(f"\n{'='*50}")
        print(f"文件: {os.path.basename(file_path)}")
        print(f"{'='*50}")
        
        try:
            # 读取Excel文件
            xl_file = pd.ExcelFile(file_path)
            print(f"工作表数量: {len(xl_file.sheet_names)}")
            print(f"工作表名称: {xl_file.sheet_names}")
            
            # 分析每个工作表
            for sheet_name in xl_file.sheet_names:
                print(f"\n--- 工作表: {sheet_name} ---")
                
                # 读取工作表数据
                df = pd.read_excel(file_path, sheet_name=sheet_name)
                print(f"行数: {len(df)}")
                print(f"列数: {len(df.columns)}")
                print(f"列名: {list(df.columns)}")
                
                # 显示数据类型
                print("\n数据类型:")
                for col in df.columns:
                    dtype = df[col].dtype
                    non_null_count = df[col].count()
                    null_count = len(df) - non_null_count
                    print(f"  {col}: {dtype} (非空: {non_null_count}, 空值: {null_count})")
                
                # 显示前几行数据
                print(f"\n前5行数据:")
                print(df.head())
                
                # 如果数据量不大，显示所有数据
                if len(df) <= 20:
                    print(f"\n所有数据:")
                    print(df)
                
                print("-" * 30)
                
        except Exception as e:
            print(f"读取文件时出错: {e}")

if __name__ == "__main__":
    analyze_excel_files()
