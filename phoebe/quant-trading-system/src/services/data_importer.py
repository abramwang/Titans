#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from datetime import datetime, date
import os
import glob
import logging
import re
import sys

# 添加项目根目录到Python路径
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from models.database import DatabaseManager, TradingDetails, CurrentPositions, DailyAccountAssets, ImportLog, Securities, AccountInfo
from config import DATABASE_CONFIG

logger = logging.getLogger(__name__)

class DataImporter:
    """数据导入器 - 增量导入Excel数据"""
    
    def __init__(self, data_path="/root/titans_github/phoebe/data"):
        self.db_manager = DatabaseManager()
        self.data_path = data_path
        self.data_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), 'data')
        
        # 业务类型识别关键词
        self.business_type_keywords = {
            '日内底仓T0': ['日内底仓T0', 'T0', '日内底仓'],
            'ETF日内申赎': ['ETF日内申赎', '申赎', 'ETF申购', 'ETF赎回'],
            '隔夜底仓': ['隔夜底仓', '底仓', '隔夜持仓'],
            'ETF赎回涨停票': ['ETF赎回涨停票', '涨停票', '涨停', '赎回涨停']
        }
    
    def import_daily_data(self, target_date=None):
        """导入指定日期的数据，如果不指定则导入所有未导入的数据"""
        session = self.db_manager.get_session()
        results = []
        
        try:
            # 获取需要导入的文件
            files_to_import = self._get_files_to_import(target_date)
            
            for file_path in files_to_import:
                try:
                    result = self._import_single_file(file_path, session)
                    results.append(result)
                    logger.info(f"成功导入文件: {file_path}")
                except Exception as e:
                    error_msg = f"导入文件失败 {file_path}: {str(e)}"
                    logger.error(error_msg)
                    results.append({
                        'file': file_path,
                        'status': 'failed',
                        'error': error_msg
                    })
            
            session.commit()
            return results
            
        except Exception as e:
            session.rollback()
            logger.error(f"批量导入失败: {e}")
            raise
        finally:
            self.db_manager.close_session(session)
    
    def _get_files_to_import(self, target_date=None):
        """获取需要导入的文件列表"""
        # 获取所有Excel文件
        pattern = os.path.join(self.data_path, "华鑫_*_业绩估算_*.xlsx")
        all_files = glob.glob(pattern)
        
        # 如果指定了日期，只导入该日期的文件
        if target_date:
            if isinstance(target_date, str):
                target_date = datetime.strptime(target_date, '%Y-%m-%d').date()
            date_str = target_date.strftime('%Y-%m-%d')
            all_files = [f for f in all_files if date_str in f]
        
        # 检查哪些文件还未导入
        session = self.db_manager.get_session()
        try:
            imported_files = session.query(ImportLog.file_name).filter(
                ImportLog.import_status == 'success'
            ).all()
            imported_file_names = {row.file_name for row in imported_files}
            
            files_to_import = []
            for file_path in all_files:
                file_name = os.path.basename(file_path)
                if file_name not in imported_file_names:
                    files_to_import.append(file_path)
            
            return sorted(files_to_import)
            
        finally:
            self.db_manager.close_session(session)
    
    def _import_single_file(self, file_path, session):
        """导入单个文件"""
        file_name = os.path.basename(file_path)
        
        # 从文件名提取日期和账户信息
        date_match = re.search(r'(\d{4}-\d{2}-\d{2})', file_name)
        account_match = re.search(r'华鑫_(\d+)_', file_name)
        
        if not date_match or not account_match:
            raise ValueError(f"无法从文件名提取日期或账户信息: {file_name}")
        
        trade_date = datetime.strptime(date_match.group(1), '%Y-%m-%d').date()
        account_id = account_match.group(1)
        
        # 记录导入开始
        import_log = ImportLog(
            file_name=file_name,
            file_path=file_path,
            import_date=trade_date,
            import_status='processing',
            import_time=datetime.now()
        )
        session.add(import_log)
        session.flush()
        
        try:
            # 读取Excel文件
            df = pd.read_excel(file_path)
            
            # 识别业务部分
            sections = self._identify_business_sections(df)
            
            records_imported = 0
            
            # 处理每个业务部分
            for business_type, start_row in sections.items():
                section_data = self._extract_section_data(df, start_row, sections, business_type)
                if not section_data.empty:
                    count = self._process_section_data(session, section_data, account_id, trade_date, business_type)
                    records_imported += count
            
            # 更新持仓数据
            self._update_current_positions(session, account_id, trade_date)
            
            # 更新导入日志
            import_log.import_status = 'success'
            import_log.records_imported = records_imported
            
            return {
                'file': file_name,
                'status': 'success',
                'records_imported': records_imported,
                'trade_date': trade_date.strftime('%Y-%m-%d')
            }
            
        except Exception as e:
            import_log.import_status = 'failed'
            import_log.error_message = str(e)
            raise
    
    def _identify_business_sections(self, df):
        """识别Excel文件中的业务部分"""
        sections = {}
        
        for idx, row in df.iterrows():
            first_col = str(row.iloc[0]) if pd.notna(row.iloc[0]) else ""
            
            # 检查每个业务类型的关键词
            for business_type, keywords in self.business_type_keywords.items():
                for keyword in keywords:
                    if keyword in first_col:
                        sections[business_type] = idx
                        logger.debug(f"找到 {business_type} 部分，起始行: {idx}")
                        break
        
        return sections
    
    def _extract_section_data(self, df, start_row, sections, current_business_type):
        """提取指定业务部分的数据"""
        # 找到下一个部分的起始行
        section_starts = sorted(sections.values())
        current_idx = section_starts.index(start_row)
        
        if current_idx < len(section_starts) - 1:
            end_row = section_starts[current_idx + 1]
        else:
            end_row = len(df)
        
        # 提取数据部分
        section_df = df.iloc[start_row + 1:end_row].copy()
        
        # 清理空行
        section_df = section_df.dropna(how='all')
        
        # 重置索引
        section_df = section_df.reset_index(drop=True)
        
        return section_df
    
    def _process_section_data(self, session, data, account_id, trade_date, business_type):
        """处理业务部分数据"""
        if data.empty:
            return 0
        
        records_count = 0
        
        for _, row in data.iterrows():
            try:
                # 提取交易数据
                trading_detail = self._parse_trading_row(row, account_id, trade_date, business_type)
                if trading_detail:
                    session.add(trading_detail)
                    records_count += 1
                    
                    # 同时更新证券基础信息
                    self._upsert_security_info(session, trading_detail.security_code, trading_detail.security_name)
                    
            except Exception as e:
                logger.warning(f"解析交易记录失败: {e}, 行数据: {row.to_dict()}")
                continue
        
        return records_count
    
    def _parse_trading_row(self, row, account_id, trade_date, business_type):
        """解析交易记录行"""
        # 这里需要根据实际的Excel格式来解析
        # 假设的列结构，需要根据实际情况调整
        try:
            # 检查是否包含有效的交易数据
            if pd.isna(row.iloc[0]) or str(row.iloc[0]).strip() == '':
                return None
            
            security_code = str(row.iloc[0]).strip() if pd.notna(row.iloc[0]) else ''
            security_name = str(row.iloc[1]).strip() if pd.notna(row.iloc[1]) else ''
            
            # 如果证券代码为空或不是有效格式，跳过
            if not security_code or not re.match(r'\d{6}', security_code):
                return None
            
            trading_detail = TradingDetails(
                account_id=account_id,
                trade_date=trade_date,
                security_code=security_code,
                security_name=security_name,
                business_type=business_type,
                trade_direction=str(row.iloc[2]).strip() if pd.notna(row.iloc[2]) else '',
                trade_volume=int(float(row.iloc[3])) if pd.notna(row.iloc[3]) else 0,
                trade_price=float(row.iloc[4]) if pd.notna(row.iloc[4]) else 0.0,
                trade_amount=float(row.iloc[5]) if pd.notna(row.iloc[5]) else 0.0,
                commission=float(row.iloc[6]) if pd.notna(row.iloc[6]) else 0.0,
                stamp_duty=float(row.iloc[7]) if pd.notna(row.iloc[7]) else 0.0,
                transfer_fee=float(row.iloc[8]) if pd.notna(row.iloc[8]) else 0.0,
                total_fee=float(row.iloc[9]) if pd.notna(row.iloc[9]) else 0.0,
                net_amount=float(row.iloc[10]) if pd.notna(row.iloc[10]) else 0.0,
                pnl=float(row.iloc[11]) if pd.notna(row.iloc[11]) else 0.0
            )
            
            return trading_detail
            
        except Exception as e:
            logger.warning(f"解析交易记录失败: {e}")
            return None
    
    def _upsert_security_info(self, session, security_code, security_name):
        """更新或插入证券信息"""
        if not security_code or not security_name:
            return
        
        # 检查是否已存在
        existing = session.query(Securities).filter(Securities.security_code == security_code).first()
        
        if not existing:
            # 判断交易所
            exchange = 'SZ' if security_code.startswith(('0', '1', '3')) else 'SH'
            
            security = Securities(
                security_code=security_code,
                security_name=security_name,
                exchange=exchange,
                security_type='ETF' if 'ETF' in security_name else 'STOCK'
            )
            session.add(security)
    
    def _update_current_positions(self, session, account_id, trade_date):
        """更新当前持仓"""
        # 获取该账户当日的所有交易记录
        trades = session.query(TradingDetails).filter(
            TradingDetails.account_id == account_id,
            TradingDetails.trade_date == trade_date
        ).all()
        
        # 按证券代码分组计算持仓变化
        position_changes = {}
        
        for trade in trades:
            key = (trade.security_code, trade.business_type)
            if key not in position_changes:
                position_changes[key] = {
                    'security_name': trade.security_name,
                    'volume_change': 0,
                    'amount_change': 0.0,
                    'trades': []
                }
            
            # 计算持仓变化（买入为正，卖出为负）
            multiplier = 1 if trade.trade_direction == '买入' else -1
            position_changes[key]['volume_change'] += trade.trade_volume * multiplier
            position_changes[key]['amount_change'] += trade.net_amount * multiplier
            position_changes[key]['trades'].append(trade)
        
        # 更新持仓表
        for (security_code, business_type), change_info in position_changes.items():
            # 查找现有持仓
            position = session.query(CurrentPositions).filter(
                CurrentPositions.account_id == account_id,
                CurrentPositions.security_code == security_code,
                CurrentPositions.business_type == business_type
            ).first()
            
            if not position:
                # 创建新持仓
                position = CurrentPositions(
                    account_id=account_id,
                    security_code=security_code,
                    security_name=change_info['security_name'],
                    business_type=business_type,
                    current_volume=0,
                    available_volume=0,
                    cost_value=0.0
                )
                session.add(position)
            
            # 更新持仓数量和成本
            old_volume = position.current_volume or 0
            old_cost = position.cost_value or 0.0
            
            new_volume = old_volume + change_info['volume_change']
            new_cost = old_cost + change_info['amount_change']
            
            position.current_volume = new_volume
            position.available_volume = new_volume  # 简化处理，实际可能需要考虑锁定数量
            position.cost_value = new_cost
            
            # 计算平均成本
            if new_volume > 0:
                position.avg_cost = new_cost / new_volume
            else:
                position.avg_cost = 0.0
            
            position.last_updated = datetime.now()
    
    def get_import_status(self):
        """获取导入状态"""
        session = self.db_manager.get_session()
        try:
            logs = session.query(ImportLog).order_by(ImportLog.import_time.desc()).limit(20).all()
            return [
                {
                    'file_name': log.file_name,
                    'import_date': log.import_date.strftime('%Y-%m-%d') if log.import_date else None,
                    'status': log.import_status,
                    'records_imported': log.records_imported,
                    'import_time': log.import_time.strftime('%Y-%m-%d %H:%M:%S') if log.import_time else None,
                    'error_message': log.error_message
                }
                for log in logs
            ]
        finally:
            self.db_manager.close_session(session)
