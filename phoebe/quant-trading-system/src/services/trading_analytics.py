#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from datetime import datetime, date, timedelta
from sqlalchemy import func, and_, or_
import logging
import sys
import os

# 添加项目根目录到Python路径
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from models.database import DatabaseManager, TradingDetails, CurrentPositions, DailyAccountAssets

logger = logging.getLogger(__name__)

class TradingAnalytics:
    """交易分析服务 - 生成各种分析报告和统计数据"""
    
    def __init__(self):
        self.db_manager = DatabaseManager()
    
    def get_daily_summary(self, trade_date):
        """获取指定日期的交易汇总"""
        session = self.db_manager.get_session()
        
        try:
            # 获取当日交易统计
            trades_query = session.query(
                TradingDetails.business_type,
                TradingDetails.trade_direction,
                func.count(TradingDetails.id).label('trade_count'),
                func.sum(TradingDetails.trade_volume).label('total_volume'),
                func.sum(TradingDetails.trade_amount).label('total_amount'),
                func.sum(TradingDetails.total_fee).label('total_fee'),
                func.sum(TradingDetails.pnl).label('total_pnl')
            ).filter(
                TradingDetails.trade_date == trade_date
            ).group_by(
                TradingDetails.business_type,
                TradingDetails.trade_direction
            ).all()
            
            # 按业务类型组织数据
            summary = {
                'trade_date': trade_date.strftime('%Y-%m-%d'),
                'business_summary': {},
                'overall_summary': {
                    'total_trades': 0,
                    'total_volume': 0,
                    'total_amount': 0.0,
                    'total_fee': 0.0,
                    'total_pnl': 0.0,
                    'buy_amount': 0.0,
                    'sell_amount': 0.0
                }
            }
            
            for trade in trades_query:
                business_type = trade.business_type
                direction = trade.trade_direction
                
                if business_type not in summary['business_summary']:
                    summary['business_summary'][business_type] = {
                        'buy_trades': 0,
                        'sell_trades': 0,
                        'buy_volume': 0,
                        'sell_volume': 0,
                        'buy_amount': 0.0,
                        'sell_amount': 0.0,
                        'total_fee': 0.0,
                        'total_pnl': 0.0
                    }
                
                biz_summary = summary['business_summary'][business_type]
                
                if direction == '买入':
                    biz_summary['buy_trades'] = trade.trade_count
                    biz_summary['buy_volume'] = trade.total_volume or 0
                    biz_summary['buy_amount'] = float(trade.total_amount or 0)
                    summary['overall_summary']['buy_amount'] += float(trade.total_amount or 0)
                else:
                    biz_summary['sell_trades'] = trade.trade_count
                    biz_summary['sell_volume'] = trade.total_volume or 0
                    biz_summary['sell_amount'] = float(trade.total_amount or 0)
                    summary['overall_summary']['sell_amount'] += float(trade.total_amount or 0)
                
                biz_summary['total_fee'] += float(trade.total_fee or 0)
                biz_summary['total_pnl'] += float(trade.total_pnl or 0)
                
                summary['overall_summary']['total_trades'] += trade.trade_count
                summary['overall_summary']['total_volume'] += trade.total_volume or 0
                summary['overall_summary']['total_amount'] += float(trade.total_amount or 0)
                summary['overall_summary']['total_fee'] += float(trade.total_fee or 0)
                summary['overall_summary']['total_pnl'] += float(trade.total_pnl or 0)
            
            # 获取当日最活跃的证券
            top_securities = session.query(
                TradingDetails.security_code,
                TradingDetails.security_name,
                func.count(TradingDetails.id).label('trade_count'),
                func.sum(TradingDetails.trade_amount).label('total_amount'),
                func.sum(TradingDetails.pnl).label('total_pnl')
            ).filter(
                TradingDetails.trade_date == trade_date
            ).group_by(
                TradingDetails.security_code,
                TradingDetails.security_name
            ).order_by(
                func.sum(TradingDetails.trade_amount).desc()
            ).limit(10).all()
            
            summary['top_securities'] = [
                {
                    'security_code': sec.security_code,
                    'security_name': sec.security_name,
                    'trade_count': sec.trade_count,
                    'total_amount': float(sec.total_amount or 0),
                    'total_pnl': float(sec.total_pnl or 0)
                }
                for sec in top_securities
            ]
            
            return summary
            
        finally:
            self.db_manager.close_session(session)
    
    def generate_pnl_report(self, start_date=None, end_date=None):
        """生成PnL报告"""
        session = self.db_manager.get_session()
        
        try:
            if not start_date:
                start_date = date.today() - timedelta(days=30)
            if not end_date:
                end_date = date.today()
            
            # 获取期间内的每日PnL数据
            daily_pnl = session.query(
                TradingDetails.trade_date,
                TradingDetails.business_type,
                func.sum(TradingDetails.pnl).label('daily_pnl'),
                func.sum(TradingDetails.trade_amount).label('daily_amount'),
                func.sum(TradingDetails.total_fee).label('daily_fee')
            ).filter(
                and_(
                    TradingDetails.trade_date >= start_date,
                    TradingDetails.trade_date <= end_date
                )
            ).group_by(
                TradingDetails.trade_date,
                TradingDetails.business_type
            ).order_by(
                TradingDetails.trade_date
            ).all()
            
            # 组织数据
            pnl_data = {}
            business_types = set()
            
            for record in daily_pnl:
                trade_date_str = record.trade_date.strftime('%Y-%m-%d')
                business_type = record.business_type
                business_types.add(business_type)
                
                if trade_date_str not in pnl_data:
                    pnl_data[trade_date_str] = {}
                
                pnl_data[trade_date_str][business_type] = {
                    'pnl': float(record.daily_pnl or 0),
                    'amount': float(record.daily_amount or 0),
                    'fee': float(record.daily_fee or 0)
                }
            
            # 计算累计数据
            dates = sorted(pnl_data.keys())
            cumulative_pnl = {}
            total_cumulative = 0.0
            
            for business_type in business_types:
                cumulative_pnl[business_type] = 0.0
            
            chart_data = {
                'dates': dates,
                'daily_pnl': {bt: [] for bt in business_types},
                'cumulative_pnl': {bt: [] for bt in business_types},
                'total_daily_pnl': [],
                'total_cumulative_pnl': []
            }
            
            for trade_date in dates:
                daily_total = 0.0
                
                for business_type in business_types:
                    daily_pnl_value = 0.0
                    if business_type in pnl_data[trade_date]:
                        daily_pnl_value = pnl_data[trade_date][business_type]['pnl']
                    
                    cumulative_pnl[business_type] += daily_pnl_value
                    daily_total += daily_pnl_value
                    
                    chart_data['daily_pnl'][business_type].append(daily_pnl_value)
                    chart_data['cumulative_pnl'][business_type].append(cumulative_pnl[business_type])
                
                total_cumulative += daily_total
                chart_data['total_daily_pnl'].append(daily_total)
                chart_data['total_cumulative_pnl'].append(total_cumulative)
            
            return {
                'period': {
                    'start_date': start_date.strftime('%Y-%m-%d'),
                    'end_date': end_date.strftime('%Y-%m-%d')
                },
                'chart_data': chart_data,
                'summary': {
                    'total_pnl': total_cumulative,
                    'business_pnl': {bt: cumulative_pnl[bt] for bt in business_types},
                    'trading_days': len(dates)
                }
            }
            
        finally:
            self.db_manager.close_session(session)
    
    def calculate_performance_metrics(self, start_date=None, end_date=None):
        """计算绩效指标 - 类似quantstats的指标"""
        session = self.db_manager.get_session()
        
        try:
            if not start_date:
                start_date = date.today() - timedelta(days=30)
            if not end_date:
                end_date = date.today()
            
            # 获取每日PnL数据
            daily_pnl_query = session.query(
                TradingDetails.trade_date,
                func.sum(TradingDetails.pnl).label('daily_pnl')
            ).filter(
                and_(
                    TradingDetails.trade_date >= start_date,
                    TradingDetails.trade_date <= end_date
                )
            ).group_by(
                TradingDetails.trade_date
            ).order_by(
                TradingDetails.trade_date
            ).all()
            
            if not daily_pnl_query:
                return {'error': '指定期间内没有交易数据'}
            
            # 转换为numpy数组进行计算
            daily_returns = np.array([float(record.daily_pnl or 0) for record in daily_pnl_query])
            cumulative_returns = np.cumsum(daily_returns)
            
            # 计算基础指标
            total_return = cumulative_returns[-1]
            trading_days = len(daily_returns)
            avg_daily_return = np.mean(daily_returns)
            volatility = np.std(daily_returns) if len(daily_returns) > 1 else 0
            
            # 计算最大回撤
            peak = np.maximum.accumulate(cumulative_returns)
            drawdown = cumulative_returns - peak
            max_drawdown = np.min(drawdown)
            max_drawdown_pct = (max_drawdown / peak[np.argmin(drawdown)]) * 100 if peak[np.argmin(drawdown)] != 0 else 0
            
            # 计算夏普比率 (假设无风险利率为0)
            sharpe_ratio = avg_daily_return / volatility if volatility > 0 else 0
            
            # 计算胜率
            winning_days = np.sum(daily_returns > 0)
            win_rate = (winning_days / trading_days) * 100
            
            # 计算盈亏比
            winning_returns = daily_returns[daily_returns > 0]
            losing_returns = daily_returns[daily_returns < 0]
            
            avg_win = np.mean(winning_returns) if len(winning_returns) > 0 else 0
            avg_loss = np.mean(losing_returns) if len(losing_returns) > 0 else 0
            profit_loss_ratio = abs(avg_win / avg_loss) if avg_loss != 0 else float('inf')
            
            # 计算连续盈亏天数
            def calculate_consecutive(returns, condition_func):
                consecutive = []
                current_streak = 0
                
                for ret in returns:
                    if condition_func(ret):
                        current_streak += 1
                    else:
                        if current_streak > 0:
                            consecutive.append(current_streak)
                        current_streak = 0
                
                if current_streak > 0:
                    consecutive.append(current_streak)
                
                return consecutive
            
            winning_streaks = calculate_consecutive(daily_returns, lambda x: x > 0)
            losing_streaks = calculate_consecutive(daily_returns, lambda x: x < 0)
            
            max_winning_streak = max(winning_streaks) if winning_streaks else 0
            max_losing_streak = max(losing_streaks) if losing_streaks else 0
            
            # 计算月度统计
            monthly_stats = self._calculate_monthly_stats(daily_pnl_query)
            
            return {
                'period': {
                    'start_date': start_date.strftime('%Y-%m-%d'),
                    'end_date': end_date.strftime('%Y-%m-%d'),
                    'trading_days': trading_days
                },
                'returns': {
                    'total_return': round(total_return, 2),
                    'avg_daily_return': round(avg_daily_return, 2),
                    'volatility': round(volatility, 2),
                    'sharpe_ratio': round(sharpe_ratio, 4)
                },
                'risk': {
                    'max_drawdown': round(max_drawdown, 2),
                    'max_drawdown_pct': round(max_drawdown_pct, 2),
                    'volatility': round(volatility, 2)
                },
                'win_loss': {
                    'win_rate': round(win_rate, 2),
                    'winning_days': int(winning_days),
                    'losing_days': int(trading_days - winning_days),
                    'avg_win': round(avg_win, 2),
                    'avg_loss': round(avg_loss, 2),
                    'profit_loss_ratio': round(profit_loss_ratio, 2) if profit_loss_ratio != float('inf') else 'N/A'
                },
                'streaks': {
                    'max_winning_streak': max_winning_streak,
                    'max_losing_streak': max_losing_streak,
                    'current_streak': self._calculate_current_streak(daily_returns)
                },
                'monthly_stats': monthly_stats,
                'daily_returns': daily_returns.tolist(),
                'cumulative_returns': cumulative_returns.tolist(),
                'dates': [record.trade_date.strftime('%Y-%m-%d') for record in daily_pnl_query]
            }
            
        finally:
            self.db_manager.close_session(session)
    
    def _calculate_monthly_stats(self, daily_pnl_query):
        """计算月度统计"""
        monthly_data = {}
        
        for record in daily_pnl_query:
            month_key = record.trade_date.strftime('%Y-%m')
            if month_key not in monthly_data:
                monthly_data[month_key] = []
            monthly_data[month_key].append(float(record.daily_pnl or 0))
        
        monthly_stats = []
        for month, returns in monthly_data.items():
            monthly_return = sum(returns)
            trading_days = len(returns)
            win_days = len([r for r in returns if r > 0])
            win_rate = (win_days / trading_days) * 100 if trading_days > 0 else 0
            
            monthly_stats.append({
                'month': month,
                'return': round(monthly_return, 2),
                'trading_days': trading_days,
                'win_rate': round(win_rate, 2),
                'avg_daily': round(monthly_return / trading_days, 2) if trading_days > 0 else 0
            })
        
        return sorted(monthly_stats, key=lambda x: x['month'])
    
    def _calculate_current_streak(self, daily_returns):
        """计算当前连续盈亏天数"""
        if len(daily_returns) == 0:
            return 0
        
        current_streak = 0
        last_return = daily_returns[-1]
        
        if last_return > 0:
            # 当前是盈利，计算连续盈利天数
            for i in range(len(daily_returns) - 1, -1, -1):
                if daily_returns[i] > 0:
                    current_streak += 1
                else:
                    break
        elif last_return < 0:
            # 当前是亏损，计算连续亏损天数
            for i in range(len(daily_returns) - 1, -1, -1):
                if daily_returns[i] < 0:
                    current_streak -= 1
                else:
                    break
        
        return current_streak
    
    def get_trading_calendar(self, year=None, month=None):
        """获取交易日历"""
        session = self.db_manager.get_session()
        
        try:
            if not year:
                year = date.today().year
            if not month:
                month = date.today().month
            
            # 获取指定月份的交易日
            start_date = date(year, month, 1)
            if month == 12:
                end_date = date(year + 1, 1, 1) - timedelta(days=1)
            else:
                end_date = date(year, month + 1, 1) - timedelta(days=1)
            
            trading_days = session.query(
                TradingDetails.trade_date,
                func.sum(TradingDetails.pnl).label('daily_pnl'),
                func.count(TradingDetails.id).label('trade_count')
            ).filter(
                and_(
                    TradingDetails.trade_date >= start_date,
                    TradingDetails.trade_date <= end_date
                )
            ).group_by(
                TradingDetails.trade_date
            ).order_by(
                TradingDetails.trade_date
            ).all()
            
            calendar_data = []
            for day in trading_days:
                calendar_data.append({
                    'date': day.trade_date.strftime('%Y-%m-%d'),
                    'pnl': float(day.daily_pnl or 0),
                    'trade_count': day.trade_count,
                    'pnl_status': 'profit' if day.daily_pnl > 0 else 'loss' if day.daily_pnl < 0 else 'flat'
                })
            
            return {
                'year': year,
                'month': month,
                'trading_days': calendar_data,
                'summary': {
                    'total_trading_days': len(calendar_data),
                    'profit_days': len([d for d in calendar_data if d['pnl'] > 0]),
                    'loss_days': len([d for d in calendar_data if d['pnl'] < 0]),
                    'flat_days': len([d for d in calendar_data if d['pnl'] == 0]),
                    'total_pnl': sum([d['pnl'] for d in calendar_data])
                }
            }
            
        finally:
            self.db_manager.close_session(session)
