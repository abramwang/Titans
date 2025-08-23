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

class EnhancedT0Importer:
    """增强版T0导入器 - 支持完整的买卖信息显示"""
    
    def __init__(self):
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        self.engine = create_engine(connection_string)
    
    def add_t0_fields_to_database(self):
        """为数据库添加T0交易的买卖字段"""
        try:
            with self.engine.connect() as conn:
                # 检查字段是否已存在
                check_query = """
                    SELECT COLUMN_NAME 
                    FROM INFORMATION_SCHEMA.COLUMNS 
                    WHERE TABLE_SCHEMA = 'ti_trading_performance' 
                    AND TABLE_NAME = 'trading_details' 
                    AND COLUMN_NAME IN ('buy_price', 'buy_volume', 'buy_amount', 'sell_price', 'sell_volume', 'sell_amount')
                """
                existing_fields = conn.execute(text(check_query)).fetchall()
                existing_field_names = [row[0] for row in existing_fields]
                
                fields_to_add = []
                if 'buy_price' not in existing_field_names:
                    fields_to_add.extend([
                        'ADD COLUMN buy_price DECIMAL(10,4) DEFAULT NULL COMMENT "买入价格"',
                        'ADD COLUMN buy_volume INT DEFAULT NULL COMMENT "买入数量"',
                        'ADD COLUMN buy_amount DECIMAL(15,2) DEFAULT NULL COMMENT "买入金额"',
                        'ADD COLUMN sell_price DECIMAL(10,4) DEFAULT NULL COMMENT "卖出价格"',
                        'ADD COLUMN sell_volume INT DEFAULT NULL COMMENT "卖出数量"',
                        'ADD COLUMN sell_amount DECIMAL(15,2) DEFAULT NULL COMMENT "卖出金额"'
                    ])
                
                if fields_to_add:
                    alter_sql = f"ALTER TABLE trading_details {', '.join(fields_to_add)}"
                    conn.execute(text(alter_sql))
                    conn.commit()
                    print(f"✓ 成功添加 {len(fields_to_add)} 个T0买卖字段")
                else:
                    print("✓ T0买卖字段已存在")
                    
        except Exception as e:
            print(f"❌ 添加字段失败: {e}")
            
    def extract_date_from_filename(self, filename):
        """从文件名提取日期"""
        match = re.search(r'(\d{4}-\d{2}-\d{2})', filename)
        if match:
            return match.group(1)
        return None
    
    def process_t0_section_enhanced(self, df, trade_date):
        """增强版T0数据处理 - 保存完整买卖信息"""
        records = []
        
        # 分析不同区域
        t0_start = -1
        other_sections = []
        
        # 找到T0区域的开始
        if '日内底仓T0' in str(df.columns[0]):
            t0_start = 0
        
        # 查找其他业务区域的分界点
        for idx, row in df.iterrows():
            first_col = str(row.iloc[0]).strip() if pd.notna(row.iloc[0]) else ""
            if any(keyword in first_col for keyword in ['ETF日内申赎', '隔夜底仓', 'ETF赎回涨停票']):
                other_sections.append((first_col, idx))
        
        # 确定T0区域的结束位置
        if other_sections:
            t0_end = min([idx for _, idx in other_sections])
        else:
            t0_end = len(df)
        
        # 处理T0区域的数据
        valid_transactions = 0
        for idx in range(t0_start + 1, t0_end):
            if idx >= len(df):
                break
                
            row = df.iloc[idx]
            security_code = str(row.iloc[0]).strip() if pd.notna(row.iloc[0]) else ""
            
            # 检查是否为有效的证券代码行
            if not security_code or security_code == 'nan' or not ('.' in security_code and (security_code.endswith('.SH') or security_code.endswith('.SZ'))):
                continue
            
            security_name = str(row.iloc[1]).strip() if pd.notna(row.iloc[1]) else ""
            if not security_name or security_name == 'nan':
                continue
            
            try:
                # 解析买入数据
                buy_price = float(row.iloc[2]) if pd.notna(row.iloc[2]) and str(row.iloc[2]) != 'nan' else None
                buy_volume = int(float(row.iloc[3])) if pd.notna(row.iloc[3]) and str(row.iloc[3]) != 'nan' else None
                buy_amount = float(row.iloc[4]) if pd.notna(row.iloc[4]) and str(row.iloc[4]) != 'nan' else None
                
                # 解析卖出数据
                sell_price = float(row.iloc[5]) if pd.notna(row.iloc[5]) and str(row.iloc[5]) != 'nan' else None
                sell_volume = int(float(row.iloc[6])) if pd.notna(row.iloc[6]) and str(row.iloc[6]) != 'nan' else None
                sell_amount = float(row.iloc[7]) if pd.notna(row.iloc[7]) and str(row.iloc[7]) != 'nan' else None
                
                # 手续费和利润
                commission = float(row.iloc[8]) if pd.notna(row.iloc[8]) and str(row.iloc[8]) != 'nan' else 0
                profit = float(row.iloc[9]) if pd.notna(row.iloc[9]) and str(row.iloc[9]) != 'nan' else None
                
                # 检查数据有效性
                has_buy = buy_price and buy_volume and buy_amount
                has_sell = sell_price and sell_volume and sell_amount
                
                if not (has_buy or has_sell):
                    continue
                
                # 创建增强的T0交易记录
                record = {
                    'account_id': '314000045768',
                    'trade_date': trade_date,
                    'security_code': security_code,
                    'security_name': security_name,
                    'business_type': '日内底仓T0',
                    'business_category': 'trading',
                    'trade_direction': 'T0交易',
                    'trade_volume': max(buy_volume or 0, sell_volume or 0),
                    'trade_price': buy_price if has_buy else sell_price,
                    'trade_amount': max(buy_amount or 0, sell_amount or 0),
                    'commission': commission,
                    'stamp_duty': 0,
                    'transfer_fee': 0,
                    'net_amount': (buy_amount or 0) + commission,
                    'pnl': profit if profit else 0,
                    # 新增的买卖详细字段
                    'buy_price': buy_price,
                    'buy_volume': buy_volume,
                    'buy_amount': buy_amount,
                    'sell_price': sell_price,
                    'sell_volume': sell_volume,
                    'sell_amount': sell_amount
                }
                records.append(record)
                valid_transactions += 1
                
            except (ValueError, IndexError) as e:
                continue
        
        return records
    
    def update_t0_data_with_details(self):
        """更新T0数据，添加买卖详细信息"""
        print("="*80)
        print("增强T0数据 - 添加买卖详细信息")
        print("="*80)
        
        # 首先添加新字段
        self.add_t0_fields_to_database()
        
        # 清理现有的T0数据
        print("\\n清理现有的日内底仓T0数据...")
        with self.engine.connect() as conn:
            result = conn.execute(text("DELETE FROM trading_details WHERE business_type = '日内底仓T0'"))
            deleted_count = result.rowcount
            conn.commit()
            print(f"✓ 已删除 {deleted_count} 条旧的T0记录")
        
        data_folder = './data'
        files_processed = 0
        total_records = 0
        
        # 重新导入所有T0数据
        for filename in sorted(os.listdir(data_folder)):
            if '业绩估算' in filename and filename.endswith('.xlsx'):
                filepath = os.path.join(data_folder, filename)
                trade_date = self.extract_date_from_filename(filename)
                
                if not trade_date:
                    continue
                
                print(f"\\n处理文件: {filename}")
                
                try:
                    df = pd.read_excel(filepath)
                    t0_records = self.process_t0_section_enhanced(df, trade_date)
                    
                    if t0_records:
                        self.save_enhanced_records(t0_records)
                        print(f"  ✓ 导入 {len(t0_records)} 条增强T0记录")
                        total_records += len(t0_records)
                        files_processed += 1
                        
                except Exception as e:
                    print(f"  ❌ 处理失败: {e}")
                    continue
        
        print(f"\\n增强导入完成:")
        print(f"  处理文件: {files_processed} 个")
        print(f"  增强T0记录: {total_records} 条")
        
    def save_enhanced_records(self, records):
        """保存增强的记录到数据库"""
        if not records:
            return
            
        df = pd.DataFrame(records)
        df.to_sql('trading_details', con=self.engine, if_exists='append', index=False)

if __name__ == "__main__":
    importer = EnhancedT0Importer()
    importer.update_t0_data_with_details()
