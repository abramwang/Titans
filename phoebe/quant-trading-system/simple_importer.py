#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from datetime import datetime, date
import os
import sys
import logging
import re

# 添加项目根目录到Python路径
project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, project_root)
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from src.models.database import DatabaseManager, TradingDetails, CurrentPositions, DailyAccountAssets, ImportLog, Securities
from config import DATABASE_CONFIG

logger = logging.getLogger(__name__)

class SimpleDataImporter:
    """简化版数据导入器"""
    
    def __init__(self, data_path="/root/titans_github/phoebe/data"):
        self.db_manager = DatabaseManager()
        self.data_path = data_path
    
    def import_excel_file(self, file_path):
        """导入单个Excel文件"""
        print(f"开始导入文件: {file_path}")
        
        try:
            # 读取Excel文件
            df = pd.read_excel(file_path)
            
            # 从文件名提取日期
            file_name = os.path.basename(file_path)
            date_match = re.search(r'(\d{4}-\d{2}-\d{2})', file_name)
            if not date_match:
                raise ValueError(f"无法从文件名中提取日期: {file_name}")
            
            trade_date = datetime.strptime(date_match.group(1), '%Y-%m-%d').date()
            
            # 从文件名提取账户ID
            account_match = re.search(r'(\d{12})', file_name)
            account_id = account_match.group(1) if account_match else '314000045768'
            
            session = self.db_manager.get_session()
            imported_count = 0
            
            try:
                # 解析数据
                current_business_type = None
                
                for index, row in df.iterrows():
                    # 使用改进的有效行检查
                    if not self._is_valid_data_row(row):
                        continue
                        
                    first_col = str(row.iloc[0]).strip()
                    
                    # 识别业务类型
                    if first_col in ['日内底仓T0', 'ETF日内申赎', '隔夜底仓', 'ETF赎回涨停票']:
                        current_business_type = first_col
                        print(f"切换到业务类型: {current_business_type}")
                        continue
                    
                    # 跳过表头行
                    if first_col == '证券代码':
                        continue
                    
                    # 处理有效的交易记录
                    if current_business_type and self._is_valid_security_code(first_col):
                        try:
                            trading_record = self._parse_row_to_trading_record(
                                row, account_id, trade_date, current_business_type
                            )
                            if trading_record:
                                session.add(trading_record)
                                imported_count += 1
                                print(f"添加{current_business_type}记录: {trading_record.security_code} {trading_record.security_name} PnL: ¥{trading_record.pnl}")
                                
                                # 更新证券信息
                                self._upsert_security(session, trading_record.security_code, trading_record.security_name)
                                
                        except Exception as e:
                            print(f"解析行数据失败: {e}, 行: {index}")
                            continue
                
                # 记录导入日志
                import_log = ImportLog(
                    file_name=os.path.basename(file_path),
                    file_path=file_path,
                    import_date=trade_date,
                    import_status='success',
                    records_imported=imported_count,
                    import_time=datetime.now()
                )
                session.add(import_log)
                
                session.commit()
                print(f"导入成功: {imported_count} 条记录")
                return imported_count
                
            except Exception as e:
                session.rollback()
                print(f"导入失败: {e}")
                raise
            finally:
                session.close()
                
        except Exception as e:
            print(f"文件处理失败: {e}")
            raise
    
    def _is_valid_security_code(self, code):
        """检查是否是有效的证券代码"""
        if not code:
            return False
        # 匹配6位数字.交易所代码格式
        return bool(re.match(r'^\d{6}\.[A-Z]{2}$', code))
    
    def _parse_row_to_trading_record(self, row, account_id, trade_date, business_type):
        """解析行数据为交易记录"""
        try:
            security_code = str(row.iloc[0]).strip()
            security_name = str(row.iloc[1]).strip() if pd.notna(row.iloc[1]) else ''
            
            # 区分业务类型：
            # "日内底仓T0"、"ETF日内申赎" - 当日交易记录明细
            # "隔夜底仓"、"ETF赎回涨停票" - 当日收盘后留下的持仓
            
            if business_type in ['日内底仓T0']:
                # 日内T0业务: 证券代码, 证券名称, 买入价格, 买入数量, 买入金额, 卖出价格, 卖出数量, 卖出金额, 手续费, 利润
                buy_price = float(row.iloc[2]) if pd.notna(row.iloc[2]) and str(row.iloc[2]).strip() != '' else 0.0
                buy_volume = int(float(row.iloc[3])) if pd.notna(row.iloc[3]) and str(row.iloc[3]).strip() != '' else 0
                buy_amount = float(row.iloc[4]) if pd.notna(row.iloc[4]) and str(row.iloc[4]).strip() != '' else 0.0
                sell_price = float(row.iloc[5]) if pd.notna(row.iloc[5]) and str(row.iloc[5]).strip() != '' else 0.0
                sell_volume = int(float(row.iloc[6])) if pd.notna(row.iloc[6]) and str(row.iloc[6]).strip() != '' else 0
                sell_amount = float(row.iloc[7]) if pd.notna(row.iloc[7]) and str(row.iloc[7]).strip() != '' else 0.0
                commission = float(row.iloc[8]) if pd.notna(row.iloc[8]) and str(row.iloc[8]).strip() != '' else 0.0
                profit = float(row.iloc[9]) if pd.notna(row.iloc[9]) and str(row.iloc[9]).strip() != '' else 0.0
                
                # 创建交易记录（合并买卖为一条记录）
                net_volume = abs(sell_volume) if sell_volume > 0 else abs(buy_volume)
                avg_price = buy_price if buy_volume > 0 else sell_price
                net_amount = abs(sell_amount) if sell_amount > 0 else abs(buy_amount)
                
                return TradingDetails(
                    account_id=account_id,
                    trade_date=trade_date,
                    security_code=security_code,
                    security_name=security_name,
                    business_type=business_type,
                    trade_direction='T0交易',
                    trade_volume=net_volume,
                    trade_price=avg_price,
                    trade_amount=net_amount,
                    commission=commission,
                    stamp_duty=0.0,
                    transfer_fee=0.0,
                    total_fee=commission,
                    net_amount=net_amount,
                    pnl=profit,
                    created_at=datetime.now(),
                    updated_at=datetime.now()
                )
                
            elif business_type == 'ETF日内申赎':
                # ETF申赎业务: 证券代码, 证券名称, 买入价格, 买入数量, 买入金额, 卖出IOPV, 申赎冲击成本, 利润
                price = float(row.iloc[2]) if pd.notna(row.iloc[2]) and str(row.iloc[2]).strip() != '' else 0.0
                volume = int(float(row.iloc[3])) if pd.notna(row.iloc[3]) and str(row.iloc[3]).strip() != '' else 0
                amount = float(row.iloc[4]) if pd.notna(row.iloc[4]) and str(row.iloc[4]).strip() != '' else 0.0
                # 利润可能在第7列或第8列，检查两列
                profit = 0.0
                if pd.notna(row.iloc[7]) and str(row.iloc[7]).strip() != '':
                    try:
                        profit = float(row.iloc[7])
                    except:
                        if pd.notna(row.iloc[8]) and str(row.iloc[8]).strip() != '':
                            try:
                                profit = float(row.iloc[8])
                            except:
                                profit = 0.0
                
                return TradingDetails(
                    account_id=account_id,
                    trade_date=trade_date,
                    security_code=security_code,
                    security_name=security_name,
                    business_type=business_type,
                    trade_direction='申赎',
                    trade_volume=volume,
                    trade_price=price,
                    trade_amount=amount,
                    commission=0.0,
                    stamp_duty=0.0,
                    transfer_fee=0.0,
                    total_fee=0.0,
                    net_amount=amount,
                    pnl=profit,
                    created_at=datetime.now(),
                    updated_at=datetime.now()
                )
                
            elif business_type in ['隔夜底仓', 'ETF赎回涨停票']:
                # 持仓数据: 通常包含持仓数量、成本价、市值等
                # 这些是持仓而非交易，但我们仍记录为特殊的交易记录用于跟踪
                position_price = float(row.iloc[2]) if pd.notna(row.iloc[2]) and str(row.iloc[2]).strip() != '' else 0.0
                position_volume = int(float(row.iloc[3])) if pd.notna(row.iloc[3]) and str(row.iloc[3]).strip() != '' else 0
                position_value = float(row.iloc[4]) if pd.notna(row.iloc[4]) and str(row.iloc[4]).strip() != '' else 0.0
                
                # 对于持仓数据，PnL通常是浮动盈亏
                unrealized_pnl = 0.0
                if len(row) > 5 and pd.notna(row.iloc[5]) and str(row.iloc[5]).strip() != '':
                    try:
                        unrealized_pnl = float(row.iloc[5])
                    except:
                        unrealized_pnl = 0.0
                
                return TradingDetails(
                    account_id=account_id,
                    trade_date=trade_date,
                    security_code=security_code,
                    security_name=security_name,
                    business_type=business_type,
                    trade_direction='持仓',
                    trade_volume=position_volume,
                    trade_price=position_price,
                    trade_amount=position_value,
                    commission=0.0,
                    stamp_duty=0.0,
                    transfer_fee=0.0,
                    total_fee=0.0,
                    net_amount=position_value,
                    pnl=unrealized_pnl,
                    created_at=datetime.now(),
                    updated_at=datetime.now()
                )
            
            return None
            
        except Exception as e:
            raise Exception(f"解析行数据失败: {e}")
            
    def _is_valid_data_row(self, row):
        """检查是否是有效的数据行（非空行）"""
        # 检查前几个关键列是否有有效数据
        for i in range(min(4, len(row))):
            if pd.notna(row.iloc[i]) and str(row.iloc[i]).strip() not in ['', 'nan', 'NaN']:
                return True
        return False
    
    def _upsert_security(self, session, security_code, security_name):
        """更新或插入证券信息"""
        try:
            existing = session.query(Securities).filter_by(security_code=security_code).first()
            if not existing:
                # 确定交易所
                exchange = 'SH' if security_code.endswith('.SH') else 'SZ'
                
                security = Securities(
                    security_code=security_code,
                    security_name=security_name,
                    exchange=exchange,
                    security_type='ETF',  # 假设都是ETF
                    is_active=True,
                    created_at=datetime.now(),
                    updated_at=datetime.now()
                )
                session.add(security)
        except Exception as e:
            logger.warning(f"更新证券信息失败: {e}")

if __name__ == "__main__":
    importer = SimpleDataImporter()
    
    # 测试导入一个文件
    test_file = "/root/titans_github/phoebe/data/华鑫_314000045768_业绩估算_2025-07-22.xlsx"
    
    try:
        result = importer.import_excel_file(test_file)
        print(f"导入完成，共导入 {result} 条记录")
    except Exception as e:
        print(f"导入失败: {e}")
        import traceback
        traceback.print_exc()
