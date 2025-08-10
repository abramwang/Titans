#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG

def check_duplicates():
    """检查数据库中的重复数据"""
    connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
    engine = create_engine(connection_string)
    
    print("="*80)
    print("检查数据库重复数据")
    print("="*80)
    
    # 检查daily_account_assets表的重复
    print("\n1. 检查daily_account_assets表重复数据:")
    duplicate_assets = pd.read_sql("""
        SELECT trade_date, COUNT(*) as count
        FROM daily_account_assets 
        GROUP BY trade_date 
        HAVING COUNT(*) > 1
        ORDER BY trade_date
    """, engine)
    
    if len(duplicate_assets) > 0:
        print(f"发现 {len(duplicate_assets)} 个重复日期:")
        print(duplicate_assets)
        
        total_assets = pd.read_sql("SELECT COUNT(*) as total FROM daily_account_assets", engine)
        print(f"daily_account_assets总记录数: {total_assets.iloc[0]['total']}")
    else:
        print("✓ daily_account_assets表无重复数据")
    
    # 检查daily_trading_details表的重复
    print("\n2. 检查daily_trading_details表重复数据:")
    duplicate_details = pd.read_sql("""
        SELECT trade_date, security_code, COUNT(*) as count
        FROM daily_trading_details 
        GROUP BY trade_date, security_code, buy_price, sell_price, profit
        HAVING COUNT(*) > 1
        ORDER BY trade_date, security_code
    """, engine)
    
    if len(duplicate_details) > 0:
        print(f"发现 {len(duplicate_details)} 个重复交易记录:")
        print(duplicate_details.head(10))
        
        total_details = pd.read_sql("SELECT COUNT(*) as total FROM daily_trading_details", engine)
        print(f"daily_trading_details总记录数: {total_details.iloc[0]['total']}")
    else:
        print("✓ daily_trading_details表无重复数据")
    
    # 检查securities表的重复
    print("\n3. 检查securities表重复数据:")
    try:
        duplicate_sec = pd.read_sql("""
            SELECT security_code, COUNT(*) as count
            FROM securities 
            GROUP BY security_code 
            HAVING COUNT(*) > 1
        """, engine)
        
        if len(duplicate_sec) > 0:
            print(f"发现 {len(duplicate_sec)} 个重复证券:")
            print(duplicate_sec)
        else:
            print("✓ securities表无重复数据")
    except Exception as e:
        print(f"securities表检查失败: {e}")
    
    # 显示每个表的记录数统计
    print("\n4. 各表记录数统计:")
    tables = ['daily_account_assets', 'daily_trading_details', 'securities', 
              'trading_summary', 'account_info', 'import_log']
    
    for table in tables:
        try:
            count_result = pd.read_sql(f"SELECT COUNT(*) as count FROM {table}", engine)
            count = count_result.iloc[0]['count']
            print(f"{table}: {count} 条记录")
        except Exception as e:
            print(f"{table}: 查询失败 - {e}")

def clean_duplicates():
    """清理重复数据"""
    connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
    engine = create_engine(connection_string)
    
    print("\n" + "="*80)
    print("清理重复数据")
    print("="*80)
    
    try:
        with engine.connect() as conn:
            # 清理daily_account_assets重复数据 - 保留最新的记录
            print("\n1. 清理daily_account_assets重复数据...")
            result1 = conn.execute(text("""
                DELETE daa1 FROM daily_account_assets daa1
                INNER JOIN daily_account_assets daa2 
                WHERE daa1.id < daa2.id 
                AND daa1.trade_date = daa2.trade_date
            """))
            print(f"删除了 {result1.rowcount} 条重复记录")
            
            # 清理daily_trading_details重复数据
            print("\n2. 清理daily_trading_details重复数据...")
            result2 = conn.execute(text("""
                DELETE dtd1 FROM daily_trading_details dtd1
                INNER JOIN daily_trading_details dtd2 
                WHERE dtd1.id < dtd2.id 
                AND dtd1.trade_date = dtd2.trade_date
                AND dtd1.security_code = dtd2.security_code
                AND COALESCE(dtd1.buy_price, 0) = COALESCE(dtd2.buy_price, 0)
                AND COALESCE(dtd1.sell_price, 0) = COALESCE(dtd2.sell_price, 0)
                AND COALESCE(dtd1.profit, 0) = COALESCE(dtd2.profit, 0)
            """))
            print(f"删除了 {result2.rowcount} 条重复记录")
            
            # 清理securities表重复数据
            print("\n3. 清理securities表重复数据...")
            result3 = conn.execute(text("""
                DELETE s1 FROM securities s1
                INNER JOIN securities s2 
                WHERE s1.id < s2.id 
                AND s1.security_code = s2.security_code
            """))
            print(f"删除了 {result3.rowcount} 条重复记录")
            
            # 提交事务
            conn.commit()
            print("\n✓ 重复数据清理完成")
        
    except Exception as e:
        print(f"清理过程中出错: {e}")

if __name__ == "__main__":
    # 检查重复数据
    check_duplicates()
    
    # 询问是否清理
    print("\n" + "="*80)
    choice = input("是否清理重复数据? (y/n): ").lower().strip()
    if choice in ['y', 'yes', '是']:
        clean_duplicates()
        print("\n重新检查数据:")
        check_duplicates()
    else:
        print("跳过清理操作")
