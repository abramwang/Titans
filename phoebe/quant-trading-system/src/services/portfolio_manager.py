#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from datetime import datetime, date
from sqlalchemy import func, and_, desc
import logging
import sys
import os

# 添加项目根目录到Python路径
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from models.database import DatabaseManager, CurrentPositions, TradingDetails, Securities

logger = logging.getLogger(__name__)

class PortfolioManager:
    """投资组合管理服务"""
    
    def __init__(self):
        self.db_manager = DatabaseManager()
    
    def get_current_positions(self, account_id=None):
        """获取当前持仓情况"""
        session = self.db_manager.get_session()
        
        try:
            query = session.query(CurrentPositions)
            
            if account_id:
                query = query.filter(CurrentPositions.account_id == account_id)
            
            # 只显示有持仓的记录
            positions = query.filter(CurrentPositions.current_volume > 0).all()
            
            position_data = []
            total_market_value = 0.0
            total_cost_value = 0.0
            total_unrealized_pnl = 0.0
            
            for pos in positions:
                # 获取最新价格（这里简化处理，实际应该从行情数据获取）
                latest_price = self._get_latest_price(session, pos.security_code) or pos.avg_cost
                
                market_value = (pos.current_volume or 0) * latest_price
                cost_value = pos.cost_value or 0.0
                unrealized_pnl = market_value - cost_value
                unrealized_pnl_rate = (unrealized_pnl / cost_value * 100) if cost_value > 0 else 0
                
                position_info = {
                    'account_id': pos.account_id,
                    'security_code': pos.security_code,
                    'security_name': pos.security_name,
                    'business_type': pos.business_type,
                    'current_volume': pos.current_volume,
                    'available_volume': pos.available_volume,
                    'avg_cost': float(pos.avg_cost or 0),
                    'current_price': float(latest_price),
                    'cost_value': float(cost_value),
                    'market_value': float(market_value),
                    'unrealized_pnl': float(unrealized_pnl),
                    'unrealized_pnl_rate': round(unrealized_pnl_rate, 2),
                    'last_updated': pos.last_updated.strftime('%Y-%m-%d %H:%M:%S') if pos.last_updated else None
                }
                
                position_data.append(position_info)
                total_market_value += market_value
                total_cost_value += cost_value
                total_unrealized_pnl += unrealized_pnl
            
            # 计算总体持仓统计
            total_unrealized_pnl_rate = (total_unrealized_pnl / total_cost_value * 100) if total_cost_value > 0 else 0
            
            return {
                'positions': position_data,
                'summary': {
                    'total_positions': len(position_data),
                    'total_market_value': round(total_market_value, 2),
                    'total_cost_value': round(total_cost_value, 2),
                    'total_unrealized_pnl': round(total_unrealized_pnl, 2),
                    'total_unrealized_pnl_rate': round(total_unrealized_pnl_rate, 2)
                }
            }
            
        finally:
            self.db_manager.close_session(session)
    
    def get_position_details(self, security_code, business_type=None, account_id=None):
        """获取特定证券的持仓详情"""
        session = self.db_manager.get_session()
        
        try:
            # 获取持仓信息
            query = session.query(CurrentPositions).filter(
                CurrentPositions.security_code == security_code
            )
            
            if business_type:
                query = query.filter(CurrentPositions.business_type == business_type)
            if account_id:
                query = query.filter(CurrentPositions.account_id == account_id)
            
            position = query.first()
            
            if not position:
                return {'error': f'未找到证券 {security_code} 的持仓信息'}
            
            # 获取该证券的历史交易记录
            trades = session.query(TradingDetails).filter(
                TradingDetails.security_code == security_code
            ).order_by(desc(TradingDetails.trade_date)).limit(50).all()
            
            trade_history = []
            for trade in trades:
                trade_history.append({
                    'trade_date': trade.trade_date.strftime('%Y-%m-%d'),
                    'business_type': trade.business_type,
                    'trade_direction': trade.trade_direction,
                    'trade_volume': trade.trade_volume,
                    'trade_price': float(trade.trade_price or 0),
                    'trade_amount': float(trade.trade_amount or 0),
                    'pnl': float(trade.pnl or 0)
                })
            
            # 计算持仓成本分布
            cost_analysis = self._analyze_position_cost(session, security_code, business_type)
            
            return {
                'position': {
                    'security_code': position.security_code,
                    'security_name': position.security_name,
                    'business_type': position.business_type,
                    'current_volume': position.current_volume,
                    'available_volume': position.available_volume,
                    'avg_cost': float(position.avg_cost or 0),
                    'cost_value': float(position.cost_value or 0),
                    'last_updated': position.last_updated.strftime('%Y-%m-%d %H:%M:%S') if position.last_updated else None
                },
                'trade_history': trade_history,
                'cost_analysis': cost_analysis
            }
            
        finally:
            self.db_manager.close_session(session)
    
    def get_position_changes(self, start_date=None, end_date=None):
        """获取持仓变化情况"""
        session = self.db_manager.get_session()
        
        try:
            if not start_date:
                start_date = date.today()
            if not end_date:
                end_date = date.today()
            
            # 获取期间内的交易记录，按证券分组
            trades = session.query(
                TradingDetails.security_code,
                TradingDetails.security_name,
                TradingDetails.business_type,
                TradingDetails.trade_direction,
                func.sum(TradingDetails.trade_volume).label('total_volume'),
                func.sum(TradingDetails.trade_amount).label('total_amount'),
                func.sum(TradingDetails.pnl).label('total_pnl')
            ).filter(
                and_(
                    TradingDetails.trade_date >= start_date,
                    TradingDetails.trade_date <= end_date
                )
            ).group_by(
                TradingDetails.security_code,
                TradingDetails.security_name,
                TradingDetails.business_type,
                TradingDetails.trade_direction
            ).all()
            
            # 计算净持仓变化
            position_changes = {}
            
            for trade in trades:
                key = (trade.security_code, trade.security_name, trade.business_type)
                
                if key not in position_changes:
                    position_changes[key] = {
                        'security_code': trade.security_code,
                        'security_name': trade.security_name,
                        'business_type': trade.business_type,
                        'buy_volume': 0,
                        'sell_volume': 0,
                        'buy_amount': 0.0,
                        'sell_amount': 0.0,
                        'net_volume': 0,
                        'net_amount': 0.0,
                        'total_pnl': 0.0
                    }
                
                change = position_changes[key]
                
                if trade.trade_direction == '买入':
                    change['buy_volume'] += trade.total_volume or 0
                    change['buy_amount'] += float(trade.total_amount or 0)
                else:
                    change['sell_volume'] += trade.total_volume or 0
                    change['sell_amount'] += float(trade.total_amount or 0)
                
                change['total_pnl'] += float(trade.total_pnl or 0)
            
            # 计算净变化
            for change in position_changes.values():
                change['net_volume'] = change['buy_volume'] - change['sell_volume']
                change['net_amount'] = change['buy_amount'] - change['sell_amount']
            
            return {
                'period': {
                    'start_date': start_date.strftime('%Y-%m-%d'),
                    'end_date': end_date.strftime('%Y-%m-%d')
                },
                'position_changes': list(position_changes.values())
            }
            
        finally:
            self.db_manager.close_session(session)
    
    def get_portfolio_allocation(self, account_id=None):
        """获取投资组合配置分析"""
        session = self.db_manager.get_session()
        
        try:
            query = session.query(CurrentPositions)
            
            if account_id:
                query = query.filter(CurrentPositions.account_id == account_id)
            
            positions = query.filter(CurrentPositions.current_volume > 0).all()
            
            if not positions:
                return {'error': '没有找到持仓数据'}
            
            # 按业务类型分类
            business_allocation = {}
            security_allocation = []
            total_market_value = 0.0
            
            for pos in positions:
                latest_price = self._get_latest_price(session, pos.security_code) or pos.avg_cost
                market_value = (pos.current_volume or 0) * latest_price
                total_market_value += market_value
                
                # 业务类型分配
                business_type = pos.business_type
                if business_type not in business_allocation:
                    business_allocation[business_type] = {
                        'market_value': 0.0,
                        'position_count': 0,
                        'securities': []
                    }
                
                business_allocation[business_type]['market_value'] += market_value
                business_allocation[business_type]['position_count'] += 1
                business_allocation[business_type]['securities'].append({
                    'security_code': pos.security_code,
                    'security_name': pos.security_name,
                    'market_value': market_value
                })
                
                # 证券分配
                security_allocation.append({
                    'security_code': pos.security_code,
                    'security_name': pos.security_name,
                    'business_type': pos.business_type,
                    'market_value': market_value,
                    'current_volume': pos.current_volume,
                    'avg_cost': float(pos.avg_cost or 0)
                })
            
            # 计算百分比
            for business_type in business_allocation:
                allocation = business_allocation[business_type]
                allocation['percentage'] = (allocation['market_value'] / total_market_value * 100) if total_market_value > 0 else 0
                allocation['percentage'] = round(allocation['percentage'], 2)
            
            for security in security_allocation:
                security['percentage'] = (security['market_value'] / total_market_value * 100) if total_market_value > 0 else 0
                security['percentage'] = round(security['percentage'], 2)
            
            # 排序
            security_allocation.sort(key=lambda x: x['market_value'], reverse=True)
            
            return {
                'total_market_value': round(total_market_value, 2),
                'business_allocation': business_allocation,
                'security_allocation': security_allocation,
                'concentration_risk': {
                    'top_5_percentage': sum([s['percentage'] for s in security_allocation[:5]]),
                    'top_10_percentage': sum([s['percentage'] for s in security_allocation[:10]])
                }
            }
            
        finally:
            self.db_manager.close_session(session)
    
    def _get_latest_price(self, session, security_code):
        """获取最新价格（简化实现，实际应该从行情系统获取）"""
        # 这里简化处理，使用最近一次交易的价格作为当前价格
        latest_trade = session.query(TradingDetails).filter(
            TradingDetails.security_code == security_code
        ).order_by(desc(TradingDetails.trade_date)).first()
        
        return float(latest_trade.trade_price) if latest_trade and latest_trade.trade_price else None
    
    def _analyze_position_cost(self, session, security_code, business_type=None):
        """分析持仓成本分布"""
        query = session.query(TradingDetails).filter(
            TradingDetails.security_code == security_code
        )
        
        if business_type:
            query = query.filter(TradingDetails.business_type == business_type)
        
        trades = query.order_by(TradingDetails.trade_date).all()
        
        if not trades:
            return {'error': '没有找到交易记录'}
        
        # 计算成本分布
        cost_levels = []
        current_position = 0
        weighted_cost = 0.0
        
        for trade in trades:
            if trade.trade_direction == '买入':
                old_position = current_position
                old_cost = weighted_cost
                
                current_position += trade.trade_volume or 0
                trade_cost = (trade.trade_volume or 0) * (trade.trade_price or 0)
                
                if current_position > 0:
                    weighted_cost = (old_position * old_cost + trade_cost) / current_position
                
                cost_levels.append({
                    'date': trade.trade_date.strftime('%Y-%m-%d'),
                    'action': '买入',
                    'volume': trade.trade_volume,
                    'price': float(trade.trade_price or 0),
                    'position_after': current_position,
                    'avg_cost_after': round(weighted_cost, 4)
                })
            
            else:  # 卖出
                current_position -= trade.trade_volume or 0
                current_position = max(0, current_position)  # 防止负数
                
                cost_levels.append({
                    'date': trade.trade_date.strftime('%Y-%m-%d'),
                    'action': '卖出',
                    'volume': trade.trade_volume,
                    'price': float(trade.trade_price or 0),
                    'position_after': current_position,
                    'avg_cost_after': round(weighted_cost, 4)
                })
        
        return {
            'current_position': current_position,
            'current_avg_cost': round(weighted_cost, 4),
            'cost_history': cost_levels[-20:]  # 只返回最近20条记录
        }
    
    def update_position_prices(self, price_data):
        """更新持仓价格（用于接收外部行情数据）"""
        session = self.db_manager.get_session()
        
        try:
            updated_count = 0
            
            for security_code, price in price_data.items():
                positions = session.query(CurrentPositions).filter(
                    CurrentPositions.security_code == security_code
                ).all()
                
                for position in positions:
                    position.current_price = price
                    position.market_value = (position.current_volume or 0) * price
                    position.unrealized_pnl = position.market_value - (position.cost_value or 0)
                    
                    if position.cost_value and position.cost_value > 0:
                        position.unrealized_pnl_rate = (position.unrealized_pnl / position.cost_value) * 100
                    
                    position.last_updated = datetime.now()
                    updated_count += 1
            
            session.commit()
            return {'success': True, 'updated_positions': updated_count}
            
        except Exception as e:
            session.rollback()
            logger.error(f"更新持仓价格失败: {e}")
            return {'success': False, 'error': str(e)}
        finally:
            self.db_manager.close_session(session)
