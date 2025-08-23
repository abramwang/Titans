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

class CorrectedT0Importer:
    """修正版T0导入器 - 正确理解每行包含买入和卖出数据"""
    
    def __init__(self):
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        self.engine = create_engine(connection_string)
    
    def extract_date_from_filename(self, filename):
        """从文件名提取日期"""
        match = re.search(r'(\d{4}-\d{2}-\d{2})', filename)
        if match:
            return match.group(1)
        return None
    
    def process_t0_section_corrected(self, df, trade_date):
        """修正版T0数据处理 - 每行代表一对买卖交易"""
        records = []
        
        print(f"处理日期: {trade_date}")
        print(f"Excel形状: {df.shape}")
        
        # 分析不同区域
        t0_start = -1
        other_sections = []
        
        # 找到T0区域的开始（列标题为"日内底仓T0"）
        if '日内底仓T0' in str(df.columns[0]):
            t0_start = 0
            print("✓ 找到日内底仓T0区域（从第0行开始）")
        
        # 查找其他业务区域的分界点
        for idx, row in df.iterrows():
            first_col = str(row.iloc[0]).strip() if pd.notna(row.iloc[0]) else ""
            
            # 检查是否为其他业务类型的标题
            if any(keyword in first_col for keyword in ['ETF日内申赎', '隔夜底仓', 'ETF赎回涨停票']):
                other_sections.append((first_col, idx))
                print(f"✓ 找到其他业务区域: {first_col} (第{idx}行)")
        
        # 确定T0区域的结束位置
        if other_sections:
            t0_end = min([idx for _, idx in other_sections])
        else:
            t0_end = len(df)
        
        print(f"T0数据区域: 第{t0_start}行 到 第{t0_end}行")
        
        # 处理T0区域的数据
        valid_transactions = 0
        for idx in range(t0_start + 1, t0_end):  # 跳过标题行
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
                
                # 检查数据有效性 - 必须有买入或卖出数据
                has_buy = buy_price and buy_volume and buy_amount
                has_sell = sell_price and sell_volume and sell_amount
                
                if not (has_buy or has_sell):
                    continue
                
                print(f"  行{idx}: {security_code} {security_name}")
                if has_buy:
                    print(f"    买入: {buy_volume:,}股 @ {buy_price} = {buy_amount:,.2f}")
                if has_sell:
                    print(f"    卖出: {sell_volume:,}股 @ {sell_price} = {sell_amount:,.2f}")
                if profit:
                    print(f"    利润: {profit:,.2f}")
                
                # 创建一条T0交易记录（包含完整的买卖信息）
                record = {
                    'account_id': '314000045768',
                    'trade_date': trade_date,
                    'security_code': security_code,
                    'security_name': security_name,
                    'business_type': '日内底仓T0',
                    'business_category': 'trading',
                    'trade_direction': 'T0交易',  # 表示完整的T0买卖交易
                    'buy_volume': buy_volume if has_buy else 0,
                    'buy_price': buy_price if has_buy else 0,
                    'buy_amount': buy_amount if has_buy else 0,
                    'sell_volume': sell_volume if has_sell else 0,
                    'sell_price': sell_price if has_sell else 0,
                    'sell_amount': sell_amount if has_sell else 0,
                    'commission': commission,
                    'profit': profit if profit else 0,
                    'trade_volume': max(buy_volume or 0, sell_volume or 0),  # 主要数量
                    'trade_price': buy_price if has_buy else sell_price,  # 主要价格
                    'trade_amount': max(buy_amount or 0, sell_amount or 0),  # 主要金额
                    'net_amount': (buy_amount or 0) + commission,
                    'pnl': profit if profit else 0
                }
                records.append(record)
                valid_transactions += 1
                
            except (ValueError, IndexError) as e:
                print(f"    ❌ 行{idx}解析失败: {e}")
                continue
        
        print(f"✓ 处理完成: {valid_transactions}条有效T0交易")
        return records
    
    def clean_and_reimport_t0_data(self):
        """清理并重新导入T0数据"""
        print("="*80)
        print("修正版日内底仓T0数据导入")
        print("="*80)
        
        # 先清理现有的T0数据
        print("清理现有的日内底仓T0数据...")
        with self.engine.connect() as conn:
            result = conn.execute(text("DELETE FROM trading_details WHERE business_type = '日内底仓T0'"))
            deleted_count = result.rowcount
            conn.commit()
            print(f"✓ 已删除 {deleted_count} 条旧的T0记录")
        
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
                
                print(f"\\n处理文件: {filename}")
                
                try:
                    # 读取Excel文件
                    df = pd.read_excel(filepath)
                    
                    # 处理T0数据
                    t0_records = self.process_t0_section_corrected(df, trade_date)
                    
                    if t0_records:
                        # 保存到数据库
                        self.save_records(t0_records)
                        print(f"  ✓ 导入 {len(t0_records)} 条T0交易记录")
                        total_records += len(t0_records)
                        files_processed += 1
                    else:
                        print(f"  ⚠️ 未找到T0数据")
                        
                except Exception as e:
                    print(f"  ❌ 处理失败: {e}")
                    continue
        
        print(f"\\n导入完成:")
        print(f"  处理文件: {files_processed} 个")
        print(f"  T0交易记录: {total_records} 条")
        
        return total_records
    
    def save_records(self, records):
        """保存记录到数据库"""
        if not records:
            return
            
        # 需要调整数据库表结构来支持完整的T0交易数据
        # 或者将其保存为现有格式
        
        # 转换为现有表格式（临时方案）
        converted_records = []
        for record in records:
            converted_record = {
                'account_id': record['account_id'],
                'trade_date': record['trade_date'],
                'security_code': record['security_code'],
                'security_name': record['security_name'],
                'business_type': record['business_type'],
                'business_category': record['business_category'],
                'trade_direction': record['trade_direction'],
                'trade_volume': record['trade_volume'],
                'trade_price': record['trade_price'],
                'trade_amount': record['trade_amount'],
                'commission': record['commission'],
                'stamp_duty': 0,
                'transfer_fee': 0,
                'net_amount': record['net_amount'],
                'pnl': record['pnl']
            }
            converted_records.append(converted_record)
        
        # 转换为DataFrame并保存
        df = pd.DataFrame(converted_records)
        df.to_sql('trading_details', con=self.engine, if_exists='append', index=False)
        logger.info(f"已保存 {len(converted_records)} 条记录到trading_details表")

if __name__ == "__main__":
    importer = CorrectedT0Importer()
    importer.clean_and_reimport_t0_data()
