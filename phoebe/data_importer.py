#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import pymysql
from sqlalchemy import create_engine, text
import os
import glob
import re
from datetime import datetime
import logging

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class TradingDataImporter:
    def __init__(self, host='localhost', port=3306, user='root', password='', database='trading_performance'):
        """初始化数据库连接"""
        self.host = host
        self.port = port
        self.user = user
        self.password = password
        self.database = database
        self.engine = None
        self.connection = None
        
    def connect(self):
        """连接数据库"""
        try:
            # 创建数据库连接
            connection_string = f"mysql+pymysql://{self.user}:{self.password}@{self.host}:{self.port}/{self.database}?charset=utf8mb4"
            self.engine = create_engine(connection_string)
            self.connection = self.engine.connect()
            logger.info("数据库连接成功")
            return True
        except Exception as e:
            logger.error(f"数据库连接失败: {e}")
            return False
    
    def close(self):
        """关闭数据库连接"""
        if self.connection:
            self.connection.close()
        if self.engine:
            self.engine.dispose()
    
    def import_all_data(self):
        """导入所有Excel文件数据"""
        if not self.connect():
            return False
        
        try:
            # 获取所有Excel文件
            excel_files = glob.glob("./data/*.xlsx")
            
            # 分类文件
            daily_files = [f for f in excel_files if "业绩估算" in f]
            summary_files = [f for f in excel_files if "业绩统计" in f]
            
            logger.info(f"发现 {len(daily_files)} 个日业绩估算文件")
            logger.info(f"发现 {len(summary_files)} 个业绩统计文件")
            
            # 导入业绩统计数据
            for file_path in summary_files:
                self.import_summary_file(file_path)
            
            # 导入日业绩估算数据
            for file_path in daily_files:
                self.import_daily_file(file_path)
            
            logger.info("所有数据导入完成")
            return True
            
        except Exception as e:
            logger.error(f"导入数据时出错: {e}")
            return False
        finally:
            self.close()
    
    def import_summary_file(self, file_path):
        """导入业绩统计文件"""
        try:
            logger.info(f"开始导入业绩统计文件: {os.path.basename(file_path)}")
            
            # 从文件名提取账户编号
            account_id = self.extract_account_id(file_path)
            
            # 读取Excel文件
            df = pd.read_excel(file_path)
            df = df.dropna(how='all')  # 删除全空行
            
            # 清理和转换数据
            records = []
            for i, row in df.iterrows():
                if pd.isna(row['日期']):
                    continue
                
                record = {
                    'account_id': account_id,
                    'trade_date': row['日期'].date(),
                    'start_assets': self.parse_asset_value(row['日初账户资产(不含公募占款)']),
                    'end_assets': self.parse_asset_value(row['日末账户资产(不含公募占款)']),
                    'net_value': self.safe_float(row['净值']),
                    'custody_net_value': self.safe_float(row['托管净值']),
                    'estimated_profit': self.safe_float(row['预估盈利']),
                    'floating_pnl': self.safe_float(row['浮动盈亏']),
                    'capital_flow': self.safe_float(row['出入金']),
                    'cumulative_profit': self.safe_float(row['累计盈利']),
                    'profit_including_floating': self.safe_float(row['包含浮动盈利'])
                }
                records.append(record)
            
            # 批量插入数据
            if records:
                df_records = pd.DataFrame(records)
                df_records.to_sql('daily_account_assets', self.engine, if_exists='append', index=False)
                logger.info(f"成功导入 {len(records)} 条业绩统计记录")
                
                # 记录导入日志
                self.log_import(file_path, '业绩统计', account_id, None, 'SUCCESS', len(records))
            
        except Exception as e:
            logger.error(f"导入业绩统计文件失败: {e}")
            self.log_import(file_path, '业绩统计', account_id, None, 'FAILED', 0, str(e))
    
    def import_daily_file(self, file_path):
        """导入日业绩估算文件"""
        try:
            logger.info(f"开始导入日业绩估算文件: {os.path.basename(file_path)}")
            
            # 从文件名提取账户编号和日期
            account_id = self.extract_account_id(file_path)
            trade_date = self.extract_trade_date(file_path)
            
            if not trade_date:
                logger.warning(f"无法从文件名提取交易日期: {file_path}")
                return
            
            # 读取Excel文件
            df = pd.read_excel(file_path)
            
            # 查找表头行
            header_row = self.find_header_row(df)
            if header_row is not None:
                # 重新读取，跳过无关行
                df = pd.read_excel(file_path, skiprows=header_row)
                df = df.dropna(how='all')
                
                # 提取交易明细
                trading_records = self.extract_trading_records(df, account_id, trade_date)
                
                # 提取盈亏汇总
                pnl_summary = self.extract_pnl_summary(df, file_path, account_id, trade_date)
                
                # 插入交易明细
                if trading_records:
                    # 先插入证券信息
                    self.insert_securities(trading_records)
                    
                    # 插入交易明细
                    df_trading = pd.DataFrame(trading_records)
                    df_trading.to_sql('daily_trading_details', self.engine, if_exists='append', index=False)
                    logger.info(f"成功导入 {len(trading_records)} 条交易明细记录")
                
                # 插入盈亏汇总
                if pnl_summary:
                    df_pnl = pd.DataFrame([pnl_summary])
                    df_pnl.to_sql('daily_pnl_summary', self.engine, if_exists='append', index=False)
                    logger.info(f"成功导入盈亏汇总记录")
                
                # 记录导入日志
                total_records = len(trading_records) + (1 if pnl_summary else 0)
                self.log_import(file_path, '业绩估算', account_id, trade_date, 'SUCCESS', total_records)
            
        except Exception as e:
            logger.error(f"导入日业绩估算文件失败: {e}")
            self.log_import(file_path, '业绩估算', account_id, trade_date, 'FAILED', 0, str(e))
    
    def extract_trading_records(self, df, account_id, trade_date):
        """提取交易记录"""
        records = []
        
        for i, row in df.iterrows():
            if i == 0:  # 跳过表头
                continue
                
            # 检查证券代码
            security_code = str(row.iloc[0]).strip()
            if not self.is_valid_security_code(security_code):
                continue
            
            security_name = str(row.iloc[1]) if not pd.isna(row.iloc[1]) else ''
            
            record = {
                'account_id': account_id,
                'trade_date': trade_date,
                'security_code': security_code,
                'buy_price': self.safe_float(row.iloc[2]),
                'buy_quantity': self.safe_int(row.iloc[3]),
                'sell_price': self.safe_float(row.iloc[4]) if len(row) > 4 else None,
                'sell_quantity': self.safe_int(row.iloc[5]) if len(row) > 5 else None,
                'current_price': self.safe_float(row.iloc[6]) if len(row) > 6 else None,
                'current_market_value': self.safe_float(row.iloc[7]) if len(row) > 7 else None,
                'cost_amount': self.safe_float(row.iloc[8]) if len(row) > 8 else None,
                'profit': self.safe_float(row.iloc[9]) if len(row) > 9 else None,
                'trade_type': 'T0'
            }
            records.append(record)
        
        return records
    
    def extract_pnl_summary(self, df, file_path, account_id, trade_date):
        """提取盈亏汇总"""
        settlement_pnl = None
        floating_pnl = None
        
        # 从列名中查找盈亏数据
        for col in df.columns:
            if '结算盈亏' in str(col):
                for i, row in df.iterrows():
                    if not pd.isna(row[col]):
                        settlement_pnl = self.safe_float(row[col])
                        break
            elif isinstance(col, (int, float)) and col != 0:
                # 数字列名可能是浮动盈亏
                for i, row in df.iterrows():
                    if not pd.isna(row[col]) and isinstance(row[col], (int, float)):
                        if floating_pnl is None:
                            floating_pnl = self.safe_float(row[col])
                        break
        
        # 也可以从文件名提取
        if settlement_pnl is None:
            # 尝试从文件名或其他地方提取
            pass
        
        return {
            'account_id': account_id,
            'trade_date': trade_date,
            'settlement_pnl': settlement_pnl,
            'floating_pnl': floating_pnl,
            'total_pnl': (settlement_pnl or 0) + (floating_pnl or 0),
            'total_trades': 0,  # 这些需要后续计算
            'profitable_trades': 0,
            'loss_trades': 0
        }
    
    def insert_securities(self, trading_records):
        """插入证券信息"""
        securities_data = []
        existing_codes = set()
        
        for record in trading_records:
            security_code = record['security_code']
            if security_code not in existing_codes:
                # 从证券代码提取交易所
                exchange = security_code.split('.')[1] if '.' in security_code else 'UNKNOWN'
                
                # 根据名称判断证券类型
                security_name = ''
                for r in trading_records:
                    if r['security_code'] == security_code and 'security_name' in r:
                        security_name = r.get('security_name', '')
                        break
                
                security_type = 'ETF' if 'ETF' in security_name else '股票'
                
                securities_data.append({
                    'security_code': security_code,
                    'security_name': security_name,
                    'exchange': exchange,
                    'security_type': security_type
                })
                existing_codes.add(security_code)
        
        if securities_data:
            try:
                df_securities = pd.DataFrame(securities_data)
                df_securities.to_sql('securities', self.engine, if_exists='append', index=False)
            except Exception as e:
                # 忽略重复键错误
                if 'Duplicate entry' not in str(e):
                    logger.warning(f"插入证券信息时出错: {e}")
    
    def log_import(self, file_name, file_type, account_id, trade_date, status, records_count, error_msg=None):
        """记录导入日志"""
        try:
            log_data = {
                'file_name': os.path.basename(file_name),
                'file_type': file_type,
                'account_id': account_id,
                'trade_date': trade_date,
                'import_status': status,
                'import_records': records_count,
                'error_message': error_msg
            }
            df_log = pd.DataFrame([log_data])
            df_log.to_sql('import_logs', self.engine, if_exists='append', index=False)
        except Exception as e:
            logger.error(f"记录导入日志失败: {e}")
    
    # 辅助方法
    def extract_account_id(self, file_path):
        """从文件名提取账户编号"""
        match = re.search(r'(\d{12})', file_path)
        return match.group(1) if match else '314000045768'
    
    def extract_trade_date(self, file_path):
        """从文件名提取交易日期"""
        match = re.search(r'(\d{4}-\d{2}-\d{2})', file_path)
        if match:
            return datetime.strptime(match.group(1), '%Y-%m-%d').date()
        return None
    
    def find_header_row(self, df):
        """查找表头行"""
        for i, row in df.iterrows():
            if '证券代码' in str(row.iloc[0]):
                return i
        return None
    
    def is_valid_security_code(self, code):
        """检查证券代码是否有效"""
        if not code or code == 'nan':
            return False
        pattern = r'^\d{6}\.(SH|SZ)$'
        return bool(re.match(pattern, code))
    
    def safe_float(self, value):
        """安全转换为浮点数"""
        if pd.isna(value):
            return None
        try:
            if isinstance(value, str):
                value = value.replace(',', '').replace(' ', '')
            return float(value)
        except:
            return None
    
    def safe_int(self, value):
        """安全转换为整数"""
        if pd.isna(value):
            return None
        try:
            if isinstance(value, str):
                value = value.replace(',', '').replace(' ', '')
            return int(float(value))
        except:
            return None
    
    def parse_asset_value(self, value):
        """解析资产价值（处理特殊格式）"""
        if pd.isna(value):
            return None
        
        if isinstance(value, str):
            # 处理类似 "10,001,542,43" 的格式
            value = value.replace(',', '')
            # 如果最后两位看起来像小数，添加小数点
            if len(value) > 2 and value[-2:].isdigit():
                value = value[:-2] + '.' + value[-2:]
        
        return self.safe_float(value)

# 使用示例
if __name__ == "__main__":
    # 注意：需要先创建数据库和表
    # 可以使用 database_design.sql 文件中的SQL语句
    
    importer = TradingDataImporter(
        host='localhost',
        port=3306,
        user='root',  # 请根据实际情况修改
        password='',  # 请根据实际情况修改
        database='trading_performance'
    )
    
    success = importer.import_all_data()
    if success:
        print("数据导入成功完成！")
    else:
        print("数据导入失败，请检查日志。")
