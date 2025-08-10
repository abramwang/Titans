#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def query_imported_data():
    """查询已导入的数据"""
    try:
        # 连接数据库
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        engine = create_engine(connection_string)
        
        print("=" * 60)
        print("数据导入验证报告")
        print("=" * 60)
        
        # 1. 查看导入的数据量统计
        with engine.connect() as conn:
            # 账户信息
            result = conn.execute(text("SELECT COUNT(*) as count FROM account_info"))
            account_count = result.fetchone()[0]
            print(f"1. 账户信息: {account_count} 条记录")
            
            # 证券信息
            result = conn.execute(text("SELECT COUNT(*) as count FROM securities"))
            securities_count = result.fetchone()[0]
            print(f"2. 证券信息: {securities_count} 条记录")
            
            # 日度账户资产
            result = conn.execute(text("SELECT COUNT(*) as count FROM daily_account_assets"))
            daily_assets_count = result.fetchone()[0]
            print(f"3. 日度账户资产: {daily_assets_count} 条记录")
            
            # 交易明细
            result = conn.execute(text("SELECT COUNT(*) as count FROM daily_trading_details"))
            trading_count = result.fetchone()[0]
            print(f"4. 交易明细: {trading_count} 条记录")
            
            # 盈亏汇总
            result = conn.execute(text("SELECT COUNT(*) as count FROM daily_pnl_summary"))
            pnl_count = result.fetchone()[0]
            print(f"5. 盈亏汇总: {pnl_count} 条记录")
            
            # 导入日志
            result = conn.execute(text("SELECT COUNT(*) as count FROM import_logs"))
            log_count = result.fetchone()[0]
            print(f"6. 导入日志: {log_count} 条记录")
        
        print("\n" + "=" * 60)
        print("证券交易统计（前10名）")
        print("=" * 60)
        
        # 2. 使用视图查看证券交易统计
        securities_stats = pd.read_sql("""
            SELECT 
                security_code,
                security_name,
                total_trades,
                profitable_trades,
                loss_trades,
                ROUND(total_profit, 2) as total_profit,
                ROUND(avg_profit, 2) as avg_profit
            FROM v_security_trading_stats 
            ORDER BY total_profit DESC 
            LIMIT 10
        """, engine)
        
        if not securities_stats.empty:
            print(securities_stats.to_string(index=False))
        else:
            print("暂无证券交易统计数据")
        
        print("\n" + "=" * 60)
        print("日度绩效（最近10天）")
        print("=" * 60)
        
        # 3. 查看日度绩效
        daily_performance = pd.read_sql("""
            SELECT 
                trade_date,
                ROUND(start_assets, 2) as start_assets,
                ROUND(end_assets, 2) as end_assets,
                ROUND(estimated_profit, 2) as estimated_profit,
                ROUND(cumulative_profit, 2) as cumulative_profit,
                total_trades,
                win_rate
            FROM v_daily_performance 
            ORDER BY trade_date DESC 
            LIMIT 10
        """, engine)
        
        if not daily_performance.empty:
            print(daily_performance.to_string(index=False))
        else:
            print("暂无日度绩效数据")
        
        print("\n" + "=" * 60)
        print("导入日志状态")
        print("=" * 60)
        
        # 4. 查看导入日志
        # 4. 查看导入日志
        import_logs = pd.read_sql("""
            SELECT 
                file_name,
                file_type,
                trade_date,
                import_status,
                import_records
            FROM import_logs 
            ORDER BY file_name DESC
        """, engine)
        
        if not import_logs.empty:
            print(import_logs.to_string(index=False))
        else:
            print("暂无导入日志")
        
        print("\n" + "=" * 60)
        print("最新交易明细（前10条）")
        print("=" * 60)
        
        # 5. 查看最新交易明细
        recent_trades = pd.read_sql("""
            SELECT 
                dtd.trade_date,
                dtd.security_code,
                s.security_name,
                ROUND(dtd.buy_price, 4) as buy_price,
                dtd.buy_quantity,
                ROUND(dtd.profit, 2) as profit
            FROM daily_trading_details dtd
            LEFT JOIN securities s ON dtd.security_code = s.security_code
            WHERE dtd.profit IS NOT NULL
            ORDER BY dtd.trade_date DESC, dtd.profit DESC
            LIMIT 10
        """, engine)
        
        if not recent_trades.empty:
            print(recent_trades.to_string(index=False))
        else:
            print("暂无交易明细")
        
        print("\n" + "=" * 60)
        print("数据验证完成！")
        print("=" * 60)
        
        return True
        
    except Exception as e:
        logger.error(f"查询数据失败: {e}")
        return False

if __name__ == "__main__":
    success = query_imported_data()
    if not success:
        print("数据查询失败！")
