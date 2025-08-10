#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG
import matplotlib.pyplot as plt
import seaborn as sns
from datetime import datetime, timedelta
import logging

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class TradingAnalyzer:
    def __init__(self):
        """初始化分析器"""
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        self.engine = create_engine(connection_string)
    
    def generate_performance_report(self):
        """生成综合绩效报告"""
        print("=" * 80)
        print("华鑫证券账户综合绩效分析报告")
        print("=" * 80)
        print(f"报告生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print()
        print("数据源说明:")
        print("- 业绩统计文件: 提供日度账户资产变化和汇总绩效")
        print("- 业绩估算文件: 提供具体的交易明细和证券操作")
        print()
        
        # 1. 总体概览
        self.overview_analysis()
        
        # 2. 数据完整性分析
        self.data_integrity_analysis()
        
        # 3. 收益分析（基于汇总数据）
        self.profit_analysis()
        
        # 4. 交易分析（基于明细数据）
        self.trading_analysis()
        
        # 5. 证券分析
        self.security_analysis()
        
        # 6. 数据关联分析
        self.correlation_analysis()
        
        # 7. 风险分析
        self.risk_analysis()
        
        # 8. 时间序列分析
        self.time_series_analysis()
        
        print("=" * 80)
        print("报告生成完成")
        print("=" * 80)
    
    def overview_analysis(self):
        """总体概览分析"""
        print("1. 总体概览")
        print("-" * 40)
        
        # 获取基本统计信息
        with self.engine.connect() as conn:
            # 总交易天数
            result = conn.execute(text("SELECT COUNT(DISTINCT trade_date) as days FROM daily_account_assets"))
            total_days = result.fetchone()[0]
            
            # 总交易笔数
            result = conn.execute(text("SELECT COUNT(*) as trades FROM daily_trading_details WHERE profit IS NOT NULL"))
            total_trades = result.fetchone()[0]
            
            # 总证券数量
            result = conn.execute(text("SELECT COUNT(DISTINCT security_code) as securities FROM daily_trading_details"))
            total_securities = result.fetchone()[0]
            
            # 最新资产
            result = conn.execute(text("SELECT end_assets FROM daily_account_assets ORDER BY trade_date DESC LIMIT 1"))
            latest_assets = result.fetchone()[0]
            
            # 初始资产
            result = conn.execute(text("SELECT start_assets FROM daily_account_assets ORDER BY trade_date ASC LIMIT 1"))
            initial_assets = result.fetchone()[0]
            
            # 累计收益
            result = conn.execute(text("SELECT cumulative_profit FROM daily_account_assets ORDER BY trade_date DESC LIMIT 1"))
            total_profit = result.fetchone()[0] or 0
            
        print(f"交易天数: {total_days} 天")
        print(f"总交易笔数: {total_trades} 笔")
        print(f"交易证券数量: {total_securities} 只")
        print(f"初始资产: {initial_assets:,.2f} 元")
        print(f"最新资产: {latest_assets:,.2f} 元")
        print(f"累计收益: {total_profit:,.2f} 元")
        
        if initial_assets and initial_assets > 0:
            total_return_rate = (total_profit / initial_assets) * 100
            print(f"总收益率: {total_return_rate:.2f}%")
        
        print()
    
    def data_integrity_analysis(self):
        """数据完整性分析"""
        print("2. 数据完整性分析")
        print("-" * 40)
        
        with self.engine.connect() as conn:
            # 检查汇总数据覆盖范围
            summary_stats = conn.execute(text("""
                SELECT 
                    COUNT(*) as total_days,
                    COUNT(estimated_profit) as days_with_profit,
                    MIN(trade_date) as start_date,
                    MAX(trade_date) as end_date
                FROM daily_account_assets
            """)).fetchone()
            
            print(f"汇总数据覆盖: {summary_stats[2]} 到 {summary_stats[3]} ({summary_stats[0]} 天)")
            print(f"有盈利数据的天数: {summary_stats[1]} 天")
            
            # 检查交易明细覆盖范围
            detail_stats = conn.execute(text("""
                SELECT 
                    COUNT(DISTINCT trade_date) as trading_days,
                    COUNT(*) as total_trades,
                    MIN(trade_date) as start_date,
                    MAX(trade_date) as end_date
                FROM daily_trading_details
            """)).fetchone()
            
            print(f"交易明细覆盖: {detail_stats[2]} 到 {detail_stats[3]} ({detail_stats[0]} 个交易日)")
            print(f"总交易记录: {detail_stats[1]} 条")
            
            # 检查数据匹配情况
            match_stats = conn.execute(text("""
                SELECT 
                    COUNT(CASE WHEN dtd.trade_date IS NOT NULL THEN 1 END) as matched_days,
                    COUNT(CASE WHEN dtd.trade_date IS NULL THEN 1 END) as summary_only_days
                FROM daily_account_assets daa 
                LEFT JOIN (SELECT DISTINCT trade_date FROM daily_trading_details) dtd 
                ON daa.trade_date = dtd.trade_date
                WHERE daa.estimated_profit IS NOT NULL
            """)).fetchone()
            
            print(f"汇总与明细匹配天数: {match_stats[0]} 天")
            print(f"仅有汇总数据天数: {match_stats[1]} 天")
        
        print()
    
    def profit_analysis(self):
        """收益分析"""
        print("2. 收益分析")
        print("-" * 40)
        
        # 获取日度收益数据
        daily_profits = pd.read_sql("""
            SELECT 
                trade_date,
                estimated_profit,
                cumulative_profit,
                start_assets,
                end_assets
            FROM daily_account_assets 
            ORDER BY trade_date
        """, self.engine)
        
        if not daily_profits.empty:
            # 计算收益统计
            avg_daily_profit = daily_profits['estimated_profit'].mean()
            max_daily_profit = daily_profits['estimated_profit'].max()
            min_daily_profit = daily_profits['estimated_profit'].min()
            profit_std = daily_profits['estimated_profit'].std()
            
            # 盈利天数统计
            profitable_days = len(daily_profits[daily_profits['estimated_profit'] > 0])
            total_days = len(daily_profits)
            win_rate = (profitable_days / total_days) * 100 if total_days > 0 else 0
            
            print(f"日均收益: {avg_daily_profit:,.2f} 元")
            print(f"最大单日收益: {max_daily_profit:,.2f} 元")
            print(f"最大单日亏损: {min_daily_profit:,.2f} 元")
            print(f"收益标准差: {profit_std:,.2f} 元")
            print(f"盈利天数: {profitable_days}/{total_days} 天")
            print(f"胜率: {win_rate:.2f}%")
            
            # 最大回撤分析
            daily_profits['rolling_max'] = daily_profits['cumulative_profit'].expanding().max()
            daily_profits['drawdown'] = daily_profits['cumulative_profit'] - daily_profits['rolling_max']
            max_drawdown = daily_profits['drawdown'].min()
            print(f"最大回撤: {max_drawdown:,.2f} 元")
        
        print()
    
    def trading_analysis(self):
        """交易分析"""
        print("3. 交易分析")
        print("-" * 40)
        
        # 交易统计
        trading_stats = pd.read_sql("""
            SELECT 
                COUNT(*) as total_trades,
                COUNT(CASE WHEN profit > 0 THEN 1 END) as profitable_trades,
                COUNT(CASE WHEN profit < 0 THEN 1 END) as loss_trades,
                AVG(profit) as avg_profit,
                MAX(profit) as max_profit,
                MIN(profit) as min_profit,
                SUM(profit) as total_profit
            FROM daily_trading_details 
            WHERE profit IS NOT NULL
        """, self.engine)
        
        if not trading_stats.empty:
            stats = trading_stats.iloc[0]
            trading_win_rate = (stats['profitable_trades'] / stats['total_trades']) * 100 if stats['total_trades'] > 0 else 0
            
            print(f"总交易笔数: {stats['total_trades']} 笔")
            print(f"盈利交易: {stats['profitable_trades']} 笔")
            print(f"亏损交易: {stats['loss_trades']} 笔")
            print(f"交易胜率: {trading_win_rate:.2f}%")
            print(f"平均单笔收益: {stats['avg_profit']:,.2f} 元")
            print(f"最大单笔盈利: {stats['max_profit']:,.2f} 元")
            print(f"最大单笔亏损: {stats['min_profit']:,.2f} 元")
            print(f"交易总盈利: {stats['total_profit']:,.2f} 元")
        
        # 交易频率分析
        daily_trade_count = pd.read_sql("""
            SELECT 
                trade_date,
                COUNT(*) as trades_per_day
            FROM daily_trading_details 
            WHERE profit IS NOT NULL
            GROUP BY trade_date
            ORDER BY trade_date
        """, self.engine)
        
        if not daily_trade_count.empty:
            avg_trades_per_day = daily_trade_count['trades_per_day'].mean()
            max_trades_per_day = daily_trade_count['trades_per_day'].max()
            print(f"日均交易笔数: {avg_trades_per_day:.1f} 笔")
            print(f"单日最多交易: {max_trades_per_day} 笔")
        
        print()
    
    def correlation_analysis(self):
        """数据关联分析"""
        print("6. 汇总与明细数据关联分析")
        print("-" * 40)
        
        # 对比汇总盈利与明细盈利的一致性
        correlation_data = pd.read_sql("""
            SELECT 
                daa.trade_date,
                daa.estimated_profit as summary_profit,
                COALESCE(SUM(dtd.profit), 0) as detail_profit,
                daa.estimated_profit - COALESCE(SUM(dtd.profit), 0) as difference,
                COUNT(*) as trade_count
            FROM daily_account_assets daa 
            LEFT JOIN daily_trading_details dtd ON daa.trade_date = dtd.trade_date 
            WHERE daa.estimated_profit IS NOT NULL
            GROUP BY daa.trade_date, daa.estimated_profit
            ORDER BY ABS(daa.estimated_profit - COALESCE(SUM(dtd.profit), 0)) DESC
        """, self.engine)
        
        if not correlation_data.empty:
            # 计算相关性
            valid_data = correlation_data[correlation_data['detail_profit'] != 0]
            if len(valid_data) > 1:
                correlation = valid_data['summary_profit'].corr(valid_data['detail_profit'])
                print(f"汇总盈利与明细盈利相关系数: {correlation:.3f}")
            
            # 显示差异最大的日期
            print("\n盈利差异最大的交易日:")
            top_differences = correlation_data.head(5)
            for _, row in top_differences.iterrows():
                print(f"  {row['trade_date']}: 汇总 {row['summary_profit']:,.2f}, 明细 {row['detail_profit']:,.2f}, 差异 {row['difference']:,.2f} (交易 {row['trade_count']} 笔)")
            
            # 统计差异分布
            abs_differences = correlation_data['difference'].abs()
            print(f"\n差异统计:")
            print(f"  平均绝对差异: {abs_differences.mean():,.2f} 元")
            print(f"  最大绝对差异: {abs_differences.max():,.2f} 元")
            print(f"  差异超过1000元的天数: {len(abs_differences[abs_differences > 1000])} 天")
        
        print()
    
    def security_analysis(self):
        """证券分析"""
        print("4. 证券分析")
        print("-" * 40)
        
        # 证券收益排行
        security_performance = pd.read_sql("""
            SELECT 
                dtd.security_code,
                s.security_name,
                COUNT(*) as trade_count,
                SUM(dtd.profit) as total_profit,
                AVG(dtd.profit) as avg_profit,
                MAX(dtd.profit) as max_profit,
                MIN(dtd.profit) as min_profit
            FROM daily_trading_details dtd
            LEFT JOIN securities s ON dtd.security_code = s.security_code
            WHERE dtd.profit IS NOT NULL
            GROUP BY dtd.security_code, s.security_name
            ORDER BY total_profit DESC
            LIMIT 10
        """, self.engine)
        
        if not security_performance.empty:
            print("最佳收益证券 (前10名):")
            print(security_performance.to_string(index=False, float_format='%.2f'))
        
        # 交易最频繁的证券
        most_traded = pd.read_sql("""
            SELECT 
                dtd.security_code,
                s.security_name,
                COUNT(*) as trade_count,
                SUM(dtd.profit) as total_profit
            FROM daily_trading_details dtd
            LEFT JOIN securities s ON dtd.security_code = s.security_code
            WHERE dtd.profit IS NOT NULL
            GROUP BY dtd.security_code, s.security_name
            ORDER BY trade_count DESC
            LIMIT 5
        """, self.engine)
        
        if not most_traded.empty:
            print("\n交易最频繁证券 (前5名):")
            print(most_traded.to_string(index=False, float_format='%.2f'))
        
        print()
    
    def risk_analysis(self):
        """风险分析"""
        print("5. 风险分析")
        print("-" * 40)
        
        # 获取日度收益数据计算波动率
        daily_returns = pd.read_sql("""
            SELECT 
                trade_date,
                estimated_profit,
                start_assets,
                end_assets
            FROM daily_account_assets 
            WHERE start_assets > 0
            ORDER BY trade_date
        """, self.engine)
        
        if len(daily_returns) > 1:
            # 计算日收益率
            daily_returns['return_rate'] = daily_returns['estimated_profit'] / daily_returns['start_assets']
            
            # 风险指标
            volatility = daily_returns['return_rate'].std()
            avg_return_rate = daily_returns['return_rate'].mean()
            
            # 夏普比率 (假设无风险利率为年化3%)
            risk_free_rate = 0.03 / 252  # 日无风险利率
            if volatility > 0:
                sharpe_ratio = (avg_return_rate - risk_free_rate) / volatility
            else:
                sharpe_ratio = 0
            
            # VaR (95% 置信度)
            var_95 = np.percentile(daily_returns['estimated_profit'], 5)
            
            print(f"日收益率波动率: {volatility*100:.2f}%")
            print(f"年化波动率: {volatility*np.sqrt(252)*100:.2f}%")
            print(f"夏普比率: {sharpe_ratio:.3f}")
            print(f"VaR (95%置信度): {var_95:,.2f} 元")
            
            # 连续亏损分析
            daily_returns['is_loss'] = daily_returns['estimated_profit'] < 0
            daily_returns['loss_group'] = (daily_returns['is_loss'] != daily_returns['is_loss'].shift()).cumsum()
            
            max_consecutive_losses = 0
            if daily_returns['is_loss'].any():
                loss_streaks = daily_returns[daily_returns['is_loss']].groupby('loss_group').size()
                max_consecutive_losses = loss_streaks.max() if not loss_streaks.empty else 0
            
            print(f"最大连续亏损天数: {max_consecutive_losses} 天")
        
        print()
    
    def time_series_analysis(self):
        """时间序列分析"""
        print("6. 时间趋势分析")
        print("-" * 40)
        
        # 按周分析
        weekly_performance = pd.read_sql("""
            SELECT 
                YEAR(trade_date) as year,
                WEEK(trade_date) as week,
                SUM(estimated_profit) as weekly_profit,
                AVG(start_assets) as avg_assets
            FROM daily_account_assets 
            GROUP BY YEAR(trade_date), WEEK(trade_date)
            ORDER BY year, week
        """, self.engine)
        
        if not weekly_performance.empty:
            weekly_avg = weekly_performance['weekly_profit'].mean()
            print(f"周均收益: {weekly_avg:,.2f} 元")
            
            best_week = weekly_performance.loc[weekly_performance['weekly_profit'].idxmax()]
            worst_week = weekly_performance.loc[weekly_performance['weekly_profit'].idxmin()]
            
            print(f"最佳周收益: {best_week['weekly_profit']:,.2f} 元 ({best_week['year']}年第{best_week['week']}周)")
            print(f"最差周收益: {worst_week['weekly_profit']:,.2f} 元 ({worst_week['year']}年第{worst_week['week']}周)")
        
        # 交易活跃度分析
        trading_activity = pd.read_sql("""
            SELECT 
                DAYOFWEEK(trade_date) as weekday,
                COUNT(*) as trade_count,
                AVG(profit) as avg_profit
            FROM daily_trading_details 
            WHERE profit IS NOT NULL
            GROUP BY DAYOFWEEK(trade_date)
            ORDER BY weekday
        """, self.engine)
        
        if not trading_activity.empty:
            weekday_names = ['', '周一', '周二', '周三', '周四', '周五', '周六', '周日']
            trading_activity['weekday_name'] = trading_activity['weekday'].map(lambda x: weekday_names[x])
            
            print("\n各工作日交易统计:")
            for _, row in trading_activity.iterrows():
                print(f"{row['weekday_name']}: {row['trade_count']} 笔交易, 平均收益 {row['avg_profit']:,.2f} 元")
        
        print()
    
    def export_detailed_report(self, filename=None):
        """导出详细报告到Excel"""
        if filename is None:
            filename = f"trading_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.xlsx"
        
        try:
            with pd.ExcelWriter(filename, engine='openpyxl') as writer:
                # 日度资产表
                daily_assets = pd.read_sql("SELECT * FROM daily_account_assets ORDER BY trade_date DESC", self.engine)
                daily_assets.to_excel(writer, sheet_name='日度资产', index=False)
                
                # 交易明细表
                trading_details = pd.read_sql("""
                    SELECT dtd.*, s.security_name 
                    FROM daily_trading_details dtd 
                    LEFT JOIN securities s ON dtd.security_code = s.security_code 
                    ORDER BY trade_date DESC, profit DESC
                """, self.engine)
                trading_details.to_excel(writer, sheet_name='交易明细', index=False)
                
                # 证券统计表
                security_stats = pd.read_sql("SELECT * FROM v_security_trading_stats", self.engine)
                security_stats.to_excel(writer, sheet_name='证券统计', index=False)
                
                # 日度绩效表
                daily_performance = pd.read_sql("SELECT * FROM v_daily_performance ORDER BY trade_date DESC", self.engine)
                daily_performance.to_excel(writer, sheet_name='日度绩效', index=False)
            
            print(f"详细报告已导出到: {filename}")
            return True
            
        except Exception as e:
            logger.error(f"导出报告失败: {e}")
            return False

def main():
    """主函数"""
    analyzer = TradingAnalyzer()
    
    # 生成综合报告
    analyzer.generate_performance_report()
    
    # 询问是否导出详细报告
    try:
        export_choice = input("\n是否导出详细Excel报告? (y/n): ").lower().strip()
        if export_choice in ['y', 'yes', '是']:
            analyzer.export_detailed_report()
    except KeyboardInterrupt:
        print("\n程序结束")

if __name__ == "__main__":
    main()
