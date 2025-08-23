#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG
import os
from datetime import datetime
import logging
import re

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class FixedDataImporter:
    """修正版数据导入器 - 正确处理日内底仓T0数据"""
    
    def __init__(self):
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        self.engine = create_engine(connection_string)
    
    def extract_date_from_filename(self, filename):
        """从文件名提取日期"""
        # 华鑫_314000045768_业绩估算_2025-08-11.xlsx
        match = re.search(r'(\d{4}-\d{2}-\d{2})', filename)
        if match:
            return match.group(1)
        return None
    
    def process_t0_section(self, df, trade_date):
        """处理日内底仓T0数据区域"""
        records = []
        
        # 检查第一列是否为"日内底仓T0"
        if '日内底仓T0' not in str(df.columns[0]):
            logger.warning("未找到日内底仓T0列标题")
            return records
        
        logger.info("开始处理日内底仓T0数据")
        
        # 从第1行开始（跳过标题行）
        for idx, row in df.iterrows():
            if idx == 0:  # 跳过列名行
                continue
                
            # 检查是否为有效的证券代码行
            security_code = str(row.iloc[0]).strip() if pd.notna(row.iloc[0]) else ""
            
            # 跳过空行和非证券代码行
            if not security_code or security_code == 'nan' or not ('.' in security_code):
                continue
                
            # 获取证券名称
            security_name = str(row.iloc[1]).strip() if pd.notna(row.iloc[1]) else ""
            if not security_name or security_name == 'nan':
                continue
            
            # 提取交易数据
            try:
                buy_price = float(row.iloc[2]) if pd.notna(row.iloc[2]) and str(row.iloc[2]) != 'nan' else 0
                buy_volume = int(row.iloc[3]) if pd.notna(row.iloc[3]) and str(row.iloc[3]) != 'nan' else 0
                buy_amount = float(row.iloc[4]) if pd.notna(row.iloc[4]) and str(row.iloc[4]) != 'nan' else 0
                
                sell_price = float(row.iloc[5]) if pd.notna(row.iloc[5]) and str(row.iloc[5]) != 'nan' else 0
                sell_volume = int(row.iloc[6]) if pd.notna(row.iloc[6]) and str(row.iloc[6]) != 'nan' else 0
                sell_amount = float(row.iloc[7]) if pd.notna(row.iloc[7]) and str(row.iloc[7]) != 'nan' else 0
                
                commission = float(row.iloc[8]) if pd.notna(row.iloc[8]) and str(row.iloc[8]) != 'nan' else 0
                profit = float(row.iloc[9]) if pd.notna(row.iloc[9]) and str(row.iloc[9]) != 'nan' else 0
                
            except (ValueError, IndexError) as e:
                logger.warning(f"行 {idx} 数据解析失败: {e}")
                continue
            
            # 创建买入记录（如果存在）
            if buy_volume > 0:
                record = {
                    'account_id': '314000045768',
                    'trade_date': trade_date,
                    'security_code': security_code,
                    'security_name': security_name,
                    'business_type': '日内底仓T0',
                    'business_category': 'trading',
                    'trade_direction': '买入',
                    'trade_volume': buy_volume,
                    'trade_price': buy_price,
                    'trade_amount': buy_amount,
                    'commission': commission / 2 if commission > 0 else 0,  # 平均分配手续费
                    'stamp_duty': 0,
                    'transfer_fee': 0,
                    'net_amount': buy_amount + (commission / 2 if commission > 0 else 0)
                }
                records.append(record)
            
            # 创建卖出记录（如果存在）
            if sell_volume > 0:
                record = {
                    'account_id': '314000045768',
                    'trade_date': trade_date,
                    'security_code': security_code,
                    'security_name': security_name,
                    'business_type': '日内底仓T0',
                    'business_category': 'trading',
                    'trade_direction': '卖出',
                    'trade_volume': sell_volume,
                    'trade_price': sell_price,
                    'trade_amount': sell_amount,
                    'commission': commission / 2 if commission > 0 else 0,  # 平均分配手续费
                    'stamp_duty': 0,
                    'transfer_fee': 0,
                    'net_amount': sell_amount - (commission / 2 if commission > 0 else 0)
                }
                records.append(record)
        
        logger.info(f"处理日内底仓T0数据完成，共 {len(records)} 条记录")
        return records
    
    def import_t0_data(self):
        """导入日内底仓T0数据"""
        print("="*80)
        print("导入日内底仓T0数据")
        print("="*80)
        
        data_folder = './data'
        files_processed = 0
        total_records = 0
        
        # 处理所有Excel文件
        for filename in sorted(os.listdir(data_folder)):
            if '业绩估算' in filename and filename.endswith('.xlsx'):
                filepath = os.path.join(data_folder, filename)
                trade_date = self.extract_date_from_filename(filename)
                
                if not trade_date:
                    print(f"❌ 无法从文件名提取日期: {filename}")
                    continue
                
                print(f"\n处理文件: {filename} (日期: {trade_date})")
                
                try:
                    # 读取Excel文件
                    df = pd.read_excel(filepath)
                    
                    # 处理T0数据
                    t0_records = self.process_t0_section(df, trade_date)
                    
                    if t0_records:
                        # 保存到数据库
                        self.save_records(t0_records)
                        print(f"  ✓ 导入 {len(t0_records)} 条T0记录")
                        total_records += len(t0_records)
                        files_processed += 1
                    else:
                        print(f"  ⚠️ 未找到T0数据")
                        
                except Exception as e:
                    print(f"  ❌ 处理失败: {e}")
                    continue
        
        print(f"\n导入完成:")
        print(f"  处理文件: {files_processed} 个")
        print(f"  T0记录: {total_records} 条")
    
    def save_records(self, records):
        """保存记录到数据库"""
        if not records:
            return
            
        # 转换为DataFrame
        df = pd.DataFrame(records)
        
        # 保存到trading_details表
        df.to_sql('trading_details', con=self.engine, if_exists='append', index=False)
        logger.info(f"已保存 {len(records)} 条记录到trading_details表")

if __name__ == "__main__":
    importer = FixedDataImporter()
    importer.import_t0_data()
