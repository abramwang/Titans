#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG

def clean_data_simple():
    """简单清理重复数据"""
    connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
    engine = create_engine(connection_string)
    
    print("="*80)
    print("清理重复数据 - 简化版")
    print("="*80)
    
    try:
        with engine.connect() as conn:
            # 先查看表结构
            print("1. 查看daily_account_assets表结构:")
            table_info = pd.read_sql("SHOW COLUMNS FROM daily_account_assets", conn)
            print(table_info[['Field', 'Type']])
            
            # 使用简单的清理方法 - 创建临时表
            print("\n2. 清理daily_account_assets重复数据...")
            
            # 先备份原表
            conn.execute(text("DROP TABLE IF EXISTS daily_account_assets_backup"))
            conn.execute(text("""
                CREATE TABLE daily_account_assets_backup AS 
                SELECT * FROM daily_account_assets
            """))
            
            # 清空原表
            conn.execute(text("DELETE FROM daily_account_assets"))
            
            # 插入去重后的数据
            result1 = conn.execute(text("""
                INSERT INTO daily_account_assets 
                SELECT * FROM daily_account_assets_backup 
                GROUP BY trade_date
            """))
            print(f"daily_account_assets清理完成")
            
            # 查看daily_trading_details表结构
            print("\n查看daily_trading_details表结构:")
            table_info2 = pd.read_sql("SHOW COLUMNS FROM daily_trading_details", conn)
            print(table_info2[['Field', 'Type']])
            
            # 清理daily_trading_details
            print("\n3. 清理daily_trading_details重复数据...")
            
            # 先备份
            conn.execute(text("DROP TABLE IF EXISTS daily_trading_details_backup"))
            conn.execute(text("""
                CREATE TABLE daily_trading_details_backup AS 
                SELECT * FROM daily_trading_details
            """))
            
            # 清空原表
            conn.execute(text("DELETE FROM daily_trading_details"))
            
            # 插入去重后的数据 (只使用实际存在的列)
            conn.execute(text("""
                INSERT INTO daily_trading_details 
                SELECT account_id, trade_date, security_code, 
                       buy_price, buy_quantity, sell_price, sell_quantity, 
                       current_price, current_market_value, cost_amount, profit, trade_type
                FROM daily_trading_details_backup 
                GROUP BY trade_date, security_code, COALESCE(buy_price,0), COALESCE(sell_price,0), COALESCE(profit,0)
            """))
            print(f"daily_trading_details清理完成")
            
            # 清理securities
            print("\n4. 清理securities重复数据...")
            
            # 查看securities表结构
            print("查看securities表结构:")
            table_info3 = pd.read_sql("SHOW COLUMNS FROM securities", conn)
            print(table_info3[['Field', 'Type']])
            
            # 先备份
            conn.execute(text("DROP TABLE IF EXISTS securities_backup"))
            conn.execute(text("""
                CREATE TABLE securities_backup AS 
                SELECT * FROM securities
            """))
            
            # 清空原表
            conn.execute(text("DELETE FROM securities"))
            
            # 插入去重后的数据 (使用正确的列名)
            conn.execute(text("""
                INSERT INTO securities 
                SELECT security_code, security_name, exchange, security_type
                FROM securities_backup 
                GROUP BY security_code
            """))
            print(f"securities清理完成")
            
            # 提交事务
            conn.commit()
            print("\n✓ 所有重复数据清理完成")
            
            # 检查清理后的数据
            print("\n5. 清理后的数据统计:")
            for table in ['daily_account_assets', 'daily_trading_details', 'securities']:
                count_result = pd.read_sql(f"SELECT COUNT(*) as count FROM {table}", conn)
                count = count_result.iloc[0]['count']
                print(f"{table}: {count} 条记录")
        
    except Exception as e:
        print(f"清理过程中出错: {e}")

if __name__ == "__main__":
    clean_data_simple()
