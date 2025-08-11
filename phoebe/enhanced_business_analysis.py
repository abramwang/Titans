#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
基于4个业务部分的增强分析脚本
支持对"日内底仓T0"、"ETF日内申赎"、"隔夜底仓"、"ETF赎回涨停票"四个业务部分的深度分析
"""

import pandas as pd
from sqlalchemy import create_engine
from config import DATABASE_CONFIG
import logging
from datetime import datetime

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class EnhancedBusinessAnalyzer:
    """基于4个业务部分的增强分析器"""
    
    def __init__(self):
        """初始化分析器"""
        self.setup_database()
    
    def setup_database(self):
        """设置数据库连接"""
        try:
            connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
            self.engine = create_engine(connection_string)
            logging.info("数据库连接成功")
        except Exception as e:
            logging.error(f"数据库连接失败: {e}")
            raise
    
    def generate_comprehensive_report(self):
        """生成综合分析报告"""
        print("="*100)
        print("华鑫证券 - 基于4个业务部分的综合分析报告")
        print(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("="*100)
        
        # 1. 业务部分总览
        self.analyze_business_sections()
        
        # 2. 证券表现分析
        self.analyze_security_performance()
        
        # 3. 时间序列分析
        self.analyze_time_series()
        
        # 4. 风险收益分析
        self.analyze_risk_return()
        
        # 5. 业务建议
        self.generate_business_insights()
        
        print("="*100)
        print("报告生成完成")
        print("="*100)
    
    def analyze_business_sections(self):
        """分析业务部分"""
        print("\n📊 1. 业务部分分析")
        print("-" * 50)
        
        # 业务部分概览
        section_overview = pd.read_sql("""
            SELECT 
                section_type,
                data_category,
                COUNT(*) as record_count,
                COUNT(DISTINCT trade_date) as trading_days,
                COUNT(DISTINCT security_code) as unique_securities,
                SUM(profit) as total_profit,
                AVG(profit) as avg_profit,
                STDDEV(profit) as profit_std,
                MAX(profit) as max_profit,
                MIN(profit) as min_profit
            FROM daily_trading_details 
            WHERE section_type IS NOT NULL
            GROUP BY section_type, data_category
            ORDER BY total_profit DESC
        """, self.engine)
        
        print("业务部分盈利概览:")
        for idx, row in section_overview.iterrows():
            print(f"\n🔹 {row['section_type']} ({row['data_category']})")
            print(f"   记录数量: {row['record_count']:,}")
            print(f"   交易天数: {row['trading_days']}")
            print(f"   涉及证券: {row['unique_securities']}")
            print(f"   总盈利: {row['total_profit']:,.2f}")
            print(f"   平均盈利: {row['avg_profit']:,.2f}")
            print(f"   最大盈利: {row['max_profit']:,.2f}")
            print(f"   盈利标准差: {row['profit_std']:,.2f}" if row['profit_std'] else "   盈利标准差: N/A")
        
        # 盈利贡献分析
        contribution_analysis = pd.read_sql("""
            SELECT 
                section_type,
                SUM(profit) as section_profit,
                SUM(profit) / (SELECT SUM(profit) FROM daily_trading_details WHERE section_type IS NOT NULL) * 100 as contribution_pct
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
            GROUP BY section_type
            ORDER BY section_profit DESC
        """, self.engine)
        
        print(f"\n💰 盈利贡献分析:")
        for idx, row in contribution_analysis.iterrows():
            print(f"   {row['section_type']}: {row['section_profit']:,.2f} ({row['contribution_pct']:.1f}%)")
    
    def analyze_security_performance(self):
        """分析证券表现"""
        print("\n📈 2. 证券表现分析")
        print("-" * 50)
        
        # Top 10 最佳表现证券
        top_securities = pd.read_sql("""
            SELECT 
                security_code,
                section_type,
                COUNT(*) as appear_count,
                SUM(profit) as total_profit,
                AVG(profit) as avg_profit,
                MAX(profit) as max_profit
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
            GROUP BY security_code, section_type
            HAVING total_profit > 0
            ORDER BY total_profit DESC
            LIMIT 10
        """, self.engine)
        
        print("🏆 表现最佳的10只证券:")
        for idx, row in top_securities.iterrows():
            print(f"   {idx+1:2d}. {row['security_code']} ({row['section_type']})")
            print(f"       总盈利: {row['total_profit']:,.2f} | 平均: {row['avg_profit']:,.2f} | 出现: {row['appear_count']}次")
        
        # 证券集中度分析
        concentration = pd.read_sql("""
            SELECT 
                COUNT(DISTINCT security_code) as total_securities,
                COUNT(DISTINCT CASE WHEN profit > 100000 THEN security_code END) as high_profit_securities,
                COUNT(DISTINCT CASE WHEN profit > 1000000 THEN security_code END) as super_high_profit_securities
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
        """, self.engine).iloc[0]
        
        print(f"\n🎯 证券集中度分析:")
        print(f"   总证券数: {concentration['total_securities']}")
        print(f"   高盈利证券 (>10万): {concentration['high_profit_securities']}")
        print(f"   超高盈利证券 (>100万): {concentration['super_high_profit_securities']}")
    
    def analyze_time_series(self):
        """分析时间序列"""
        print("\n📅 3. 时间序列分析")
        print("-" * 50)
        
        # 日度盈利趋势
        daily_trend = pd.read_sql("""
            SELECT 
                trade_date,
                SUM(CASE WHEN section_type = '隔夜底仓' THEN profit ELSE 0 END) as overnight_profit,
                SUM(CASE WHEN section_type = 'ETF赎回涨停票' THEN profit ELSE 0 END) as etf_redemption_profit,
                SUM(profit) as total_daily_profit,
                COUNT(*) as daily_records
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
            GROUP BY trade_date
            ORDER BY trade_date
        """, self.engine)
        
        print("📊 最近交易日盈利情况:")
        for idx, row in daily_trend.tail(10).iterrows():
            print(f"   {row['trade_date']}: 总计 {row['total_daily_profit']:,.2f}")
            print(f"     隔夜底仓: {row['overnight_profit']:,.2f} | ETF赎回: {row['etf_redemption_profit']:,.2f}")
        
        # 周度统计
        weekly_stats = daily_trend.copy()
        weekly_stats['week'] = pd.to_datetime(weekly_stats['trade_date']).dt.isocalendar().week
        weekly_summary = weekly_stats.groupby('week').agg({
            'total_daily_profit': 'sum',
            'daily_records': 'sum',
            'trade_date': 'count'
        }).tail(3)
        
        print(f"\n📈 最近3周汇总:")
        for week, row in weekly_summary.iterrows():
            print(f"   第{week}周: {row['total_daily_profit']:,.2f} ({row['trade_date']}个交易日)")
    
    def analyze_risk_return(self):
        """分析风险收益"""
        print("\n⚖️ 4. 风险收益分析")
        print("-" * 50)
        
        # 各业务部分的风险指标
        risk_metrics = pd.read_sql("""
            SELECT 
                section_type,
                COUNT(*) as total_positions,
                SUM(CASE WHEN profit > 0 THEN 1 ELSE 0 END) as profitable_positions,
                SUM(CASE WHEN profit <= 0 THEN 1 ELSE 0 END) as loss_positions,
                AVG(profit) as avg_profit,
                STDDEV(profit) as profit_volatility,
                MAX(profit) as max_profit,
                MIN(profit) as max_loss,
                SUM(profit) as total_profit
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
            GROUP BY section_type
        """, self.engine)
        
        print("📊 各业务部分风险收益指标:")
        for idx, row in risk_metrics.iterrows():
            win_rate = (row['profitable_positions'] / row['total_positions']) * 100 if row['total_positions'] > 0 else 0
            sharpe_ratio = (row['avg_profit'] / row['profit_volatility']) if row['profit_volatility'] and row['profit_volatility'] > 0 else 0
            
            print(f"\n🔸 {row['section_type']}:")
            print(f"   胜率: {win_rate:.1f}% ({row['profitable_positions']}/{row['total_positions']})")
            print(f"   平均盈利: {row['avg_profit']:,.2f}")
            print(f"   盈利波动率: {row['profit_volatility']:,.2f}" if row['profit_volatility'] else "   盈利波动率: N/A")
            print(f"   最大盈利: {row['max_profit']:,.2f}")
            print(f"   最大亏损: {row['max_loss']:,.2f}")
            print(f"   夏普比率: {sharpe_ratio:.2f}" if sharpe_ratio else "   夏普比率: N/A")
    
    def generate_business_insights(self):
        """生成业务洞察"""
        print("\n💡 5. 业务洞察与建议")
        print("-" * 50)
        
        # 获取关键数据用于分析
        total_stats = pd.read_sql("""
            SELECT 
                section_type,
                SUM(profit) as total_profit,
                COUNT(*) as position_count,
                AVG(profit) as avg_profit
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
            GROUP BY section_type
            ORDER BY total_profit DESC
        """, self.engine)
        
        print("🎯 关键发现:")
        
        # 主要盈利来源
        main_profit_source = total_stats.iloc[0]
        print(f"   • 主要盈利来源是'{main_profit_source['section_type']}'，贡献了 {main_profit_source['total_profit']:,.2f} 的利润")
        
        # 业务多样化程度
        business_diversity = len(total_stats)
        print(f"   • 当前涉及 {business_diversity} 个业务部分，业务相对{'多样化' if business_diversity >= 3 else '集中'}")
        
        # 持仓vs交易分析
        position_vs_trading = pd.read_sql("""
            SELECT 
                data_category,
                SUM(profit) as total_profit,
                COUNT(*) as record_count
            FROM daily_trading_details
            GROUP BY data_category
        """, self.engine)
        
        if len(position_vs_trading) > 1:
            position_profit = position_vs_trading[position_vs_trading['data_category'] == 'POSITION']['total_profit'].iloc[0]
            trading_profit = position_vs_trading[position_vs_trading['data_category'] == 'TRADING']['total_profit'].iloc[0] if len(position_vs_trading[position_vs_trading['data_category'] == 'TRADING']) > 0 else 0
            print(f"   • 持仓业务盈利 {position_profit:,.2f}，交易业务盈利 {trading_profit:,.2f}")
        
        print(f"\n📋 建议:")
        print(f"   1. 继续关注'{main_profit_source['section_type']}'业务，这是当前主要盈利来源")
        print(f"   2. 考虑优化风险管理，特别是高波动率的业务部分")
        print(f"   3. 加强对表现优异证券的跟踪和分析")
        print(f"   4. 建议定期回顾业务部分配置，确保风险分散")

def main():
    """主函数"""
    try:
        analyzer = EnhancedBusinessAnalyzer()
        analyzer.generate_comprehensive_report()
    except Exception as e:
        logging.error(f"分析过程中出错: {e}")
        raise

if __name__ == "__main__":
    main()
