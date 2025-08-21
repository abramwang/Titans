#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG
import os
from datetime import datetime
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class EnhancedDataImporter:
    """增强版数据导入器 - 支持4个业务部分识别"""
    
    def __init__(self):
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        self.engine = create_engine(connection_string)
        
        # 4个业务部分的标识关键词
        self.section_keywords = {
            '日内底仓T0': ['日内底仓T0', 'T0', '日内底仓'],
            'ETF日内申赎': ['ETF日内申赎', '申赎', 'ETF申购', 'ETF赎回'],
            '隔夜底仓': ['隔夜底仓', '底仓', '隔夜持仓'],
            'ETF赎回涨停票': ['ETF赎回涨停票', '涨停票', '涨停', '赎回涨停']
        }
    
    def identify_four_sections(self, df):
        """识别Excel文件中的4个业务部分"""
        sections = {}
        
        for idx, row in df.iterrows():
            first_col = str(row.iloc[0]) if pd.notna(row.iloc[0]) else ""
            
            # 检查每个部分的关键词
            for section_name, keywords in self.section_keywords.items():
                for keyword in keywords:
                    if keyword in first_col:
                        sections[section_name] = idx
                        logger.info(f"找到 {section_name} 部分，起始行: {idx}")
                        break
        
        return sections
    
    def extract_section_data(self, df, section_name, start_row, next_section_row=None):
        """提取指定部分的数据"""
        if next_section_row is None:
            end_row = len(df)
        else:
            end_row = next_section_row
        
        # 提取数据段
        section_df = df.iloc[start_row+1:end_row].copy()
        
        # 清理空行
        section_df = section_df.dropna(how='all')
        
        # 添加部分标识
        section_df['section_type'] = section_name
        
        return section_df
    
    def import_with_business_logic(self):
        """基于业务逻辑导入数据"""
        print("="*80)
        print("基于4个业务部分的增强数据导入")
        print("="*80)
        
        data_folder = './data'
        files_processed = 0
        total_trading_records = 0
        total_position_records = 0
        
        # 清空现有数据
        print("清空现有数据...")
        with self.engine.connect() as conn:
            conn.execute(text("DELETE FROM daily_trading_details"))
            conn.execute(text("DELETE FROM daily_account_assets"))
            conn.commit()
            print("✓ 数据表已清空")
        
        # 处理日期文件
        for filename in sorted(os.listdir(data_folder)):
            if '业绩估算' in filename and filename.endswith('.xlsx'):
                filepath = os.path.join(data_folder, filename)
                date_str = filename.split('_')[-1].replace('.xlsx', '')
                
                print(f"\n处理文件: {filename} ({date_str})")
                
                try:
                    # 读取Excel
                    df = pd.read_excel(filepath)
                    
                    # 识别4个部分
                    sections = self.identify_four_sections(df)
                    
                    if not sections:
                        print(f"  ❌ 未找到业务部分标识")
                        continue
                    
                    # 按顺序排序部分
                    sorted_sections = sorted(sections.items(), key=lambda x: x[1])
                    
                    trading_data = []
                    position_data = []
                    
                    # 处理每个部分
                    for i, (section_name, start_row) in enumerate(sorted_sections):
                        next_row = sorted_sections[i+1][1] if i+1 < len(sorted_sections) else None
                        
                        section_df = self.extract_section_data(df, section_name, start_row, next_row)
                        
                        if len(section_df) > 0:
                            # 根据业务逻辑分类
                            if section_name in ['日内底仓T0', 'ETF日内申赎']:
                                # 交易数据
                                trading_records = self.process_trading_data(section_df, date_str, section_name)
                                trading_data.extend(trading_records)
                                print(f"  ✓ {section_name}: {len(trading_records)} 条交易记录")
                            
                            elif section_name in ['隔夜底仓', 'ETF赎回涨停票']:
                                # 持仓数据
                                position_records = self.process_position_data(section_df, date_str, section_name)
                                position_data.extend(position_records)
                                print(f"  ✓ {section_name}: {len(position_records)} 条持仓记录")
                    
                    # 保存到数据库
                    if trading_data:
                        self.save_trading_data(trading_data)
                        total_trading_records += len(trading_data)
                    
                    if position_data:
                        self.save_position_data(position_data)
                        total_position_records += len(position_data)
                    
                    files_processed += 1
                    
                except Exception as e:
                    print(f"  ❌ 处理失败: {e}")
                    continue
        
        print(f"\n导入完成:")
        print(f"  处理文件: {files_processed} 个")
        print(f"  交易记录: {total_trading_records} 条")
        print(f"  持仓记录: {total_position_records} 条")
    
    def process_trading_data(self, section_df, date_str, section_name):
        """处理交易数据"""
        trading_records = []
        
        for idx, row in section_df.iterrows():
            # 提取证券代码
            security_code = self.extract_security_code(row)
            if not security_code:
                continue
            
            # 提取交易数据
            record = {
                'account_id': '314000045768',
                'trade_date': date_str,
                'security_code': security_code,
                'section_type': section_name,
                'profit': self.extract_profit(row)
            }
            
            # 添加其他字段
            record.update(self.extract_trading_fields(row))
            
            trading_records.append(record)
        
        return trading_records
    
    def process_position_data(self, section_df, date_str, section_name):
        """处理持仓数据"""
        position_records = []
        
        for idx, row in section_df.iterrows():
            # 提取证券代码
            security_code = self.extract_security_code(row)
            if not security_code:
                continue
            
            # 正确计算持仓盈亏
            profit = self.calculate_position_profit(row)
            
            # 提取持仓数据
            record = {
                'account_id': '314000045768',
                'trade_date': date_str,
                'security_code': security_code,
                'section_type': section_name,
                'profit': profit  # 使用正确计算的盈亏
            }
            
            # 添加其他字段
            record.update(self.extract_position_fields(row))
            
            position_records.append(record)
        
        return position_records
    
    def extract_security_code(self, row):
        """提取证券代码"""
        for cell in row:
            if pd.notna(cell) and isinstance(cell, str):
                # 寻找证券代码格式 (6位数字.SH/SZ)
                import re
                match = re.search(r'(\d{6}\.[A-Z]{2})', str(cell))
                if match:
                    return match.group(1)
        return None
    
    def extract_profit(self, row):
        """提取利润数据"""
        # 寻找可能的利润字段
        for cell in row:
            if pd.notna(cell) and isinstance(cell, (int, float)):
                # 假设利润在合理范围内
                if -100000 < cell < 100000 and cell != 0:
                    return float(cell)
        return 0.0
    
    def calculate_position_profit(self, row):
        """计算持仓盈亏 - 修正版"""
        # 寻找可能的成本价、数量、收盘价字段
        numeric_values = []
        for cell in row:
            if pd.notna(cell) and isinstance(cell, (int, float)) and cell > 0:
                numeric_values.append(float(cell))
        
        # 需要至少3个数值：成本价、数量、收盘价
        if len(numeric_values) >= 3:
            cost_price = numeric_values[0]      # 买入成本
            quantity = numeric_values[1]        # 剩余数量
            current_price = numeric_values[2]   # 收盘价
            
            # 计算盈亏: (收盘价 - 成本价) * 数量
            profit = (current_price - cost_price) * quantity
            return profit
        
        return 0.0
    
    def extract_trading_fields(self, row):
        """提取其他交易字段"""
        return {
            'buy_price': None,
            'sell_price': None,
            'quantity': None,
            'trade_type': 'INTRADAY'
        }
    
    def extract_position_fields(self, row):
        """提取其他持仓字段"""
        return {
            'quantity': None,
            'current_price': None,
            'position_type': 'OVERNIGHT'
        }
    
    def save_trading_data(self, trading_data):
        """保存交易数据"""
        for record in trading_data:
            record['data_category'] = 'TRADING'
            # 将quantity字段映射到正确的数据库字段
            if 'quantity' in record:
                if record['quantity'] and record['quantity'] > 0:
                    record['buy_quantity'] = record['quantity']
                    record['sell_quantity'] = None
                elif record['quantity'] and record['quantity'] < 0:
                    record['buy_quantity'] = None
                    record['sell_quantity'] = abs(record['quantity'])
                else:
                    record['buy_quantity'] = None
                    record['sell_quantity'] = None
                del record['quantity']  # 删除quantity字段
        
        df = pd.DataFrame(trading_data)
        df.to_sql('daily_trading_details', self.engine, if_exists='append', index=False)
    
    def save_position_data(self, position_data):
        """保存持仓数据"""
        for record in position_data:
            record['data_category'] = 'POSITION'
            # 持仓数据的字段映射
            if 'quantity' in record:
                record['buy_quantity'] = record['quantity'] if record['quantity'] and record['quantity'] > 0 else None
                record['sell_quantity'] = None
                del record['quantity']
        
        df = pd.DataFrame(position_data)
        df.to_sql('daily_trading_details', self.engine, if_exists='append', index=False)

def main():
    """主函数"""
    importer = EnhancedDataImporter()
    importer.import_with_business_logic()

if __name__ == "__main__":
    main()
