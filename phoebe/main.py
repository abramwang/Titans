#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
from datetime import datetime

def print_menu():
    """打印主菜单"""
    print("\n" + "="*60)
    print("华鑫证券业绩数据管理系统")
    print("="*60)
    print("1. 基于4个业务部分的数据导入")
    print("2. 基于业务部分的分析报告")
    print("3. 数据验证对比分析")
    print("4. 验证数据质量")
    print("0. 退出")
    print("="*60)

def enhanced_import():
    """基于4个业务部分的增强数据导入"""
    print("\n正在运行基于4个业务部分的数据导入...")
    os.system(f"{sys.executable} enhanced_importer.py")

def enhanced_business_analysis():
    """基于业务部分的增强分析报告"""
    print("\n正在生成基于4个业务部分的分析报告...")
    os.system(f"{sys.executable} enhanced_business_analysis.py")

def data_validation_comparison():
    """数据验证对比分析"""
    print("\n正在进行数据验证对比分析...")
    os.system(f"{sys.executable} data_validation_comparator.py")

def verify_data_quality():
    """验证数据质量"""
    print("\n正在验证导入的数据...")
    
    try:
        import pandas as pd
        from sqlalchemy import create_engine, text
        from config import DATABASE_CONFIG
        
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        engine = create_engine(connection_string)
        
        print("="*60)
        print("数据导入验证报告")
        print("="*60)
        
        with engine.connect() as conn:
            # 检查各表的记录数
            tables = [
                ('account_info', '账户信息'),
                ('securities', '证券信息'), 
                ('daily_account_assets', '日度账户资产'),
                ('daily_trading_details', '交易明细'),
                ('profit_loss_summary', '盈亏汇总'),
                ('import_log', '导入日志')
            ]
            
            for table_name, table_desc in tables:
                try:
                    result = conn.execute(text(f"SELECT COUNT(*) FROM {table_name}"))
                    count = result.fetchone()[0]
                    print(f"{count:>2}. {table_desc}: {count} 条记录")
                except Exception as e:
                    print(f"{count:>2}. {table_desc}: 表不存在或查询失败")
            
            print("\n" + "="*60)
            print("证券交易统计（前10名）")
            print("="*60)
            
            # 证券交易统计
            trading_stats = pd.read_sql("""
                SELECT 
                    security_code,
                    security_name,
                    COUNT(*) as total_trades,
                    SUM(CASE WHEN profit > 0 THEN 1 ELSE 0 END) as profitable_trades,
                    SUM(CASE WHEN profit < 0 THEN 1 ELSE 0 END) as loss_trades,
                    SUM(profit) as total_profit,
                    AVG(profit) as avg_profit
                FROM daily_trading_details dt
                LEFT JOIN securities s ON dt.security_code = s.security_code
                GROUP BY dt.security_code, security_name
                ORDER BY total_profit DESC
                LIMIT 10
            """, engine)
            
            print(trading_stats.to_string(index=False))
            
            print("\n" + "="*60)
            print("日度绩效（最近10天）")
            print("="*60)
            
            # 日度绩效
            daily_performance = pd.read_sql("""
                SELECT 
                    trade_date,
                    COUNT(*) as trades,
                    SUM(profit) as daily_profit,
                    AVG(profit) as avg_profit
                FROM daily_trading_details
                GROUP BY trade_date
                ORDER BY trade_date DESC
                LIMIT 10
            """, engine)
            
            if len(daily_performance) > 0:
                print(daily_performance.to_string(index=False))
            else:
                print("暂无日度绩效数据")
            
            print("\n" + "="*60)
            print("导入日志状态")
            print("="*60)
            
            # 导入日志
            import_logs = pd.read_sql("""
                SELECT 
                    file_name,
                    file_type,
                    trade_date,
                    import_status,
                    import_records
                FROM import_log
                ORDER BY id DESC
                LIMIT 30
            """, engine)
            
            print(import_logs.to_string(index=False))
            
            print("\n" + "="*60)
            print("最新交易明细（前10条）")
            print("="*60)
            
            # 最新交易明细
            recent_trades = pd.read_sql("""
                SELECT 
                    trade_date,
                    security_code,
                    security_name,
                    buy_price,
                    buy_quantity,
                    profit
                FROM daily_trading_details dt
                LEFT JOIN securities s ON dt.security_code = s.security_code
                ORDER BY trade_date DESC, profit DESC
                LIMIT 10
            """, engine)
            
            print(recent_trades.to_string(index=False))
            
        print("\n" + "="*60)
        print("数据验证完成！")
        print("="*60)
        
    except Exception as e:
        print(f"发生错误: {e}")
    
    input("\n按回车键继续...")

def main():
    """主函数"""
    while True:
        try:
            print_menu()
            choice = input("请选择操作 (0-4): ").strip()
            
            if choice == '0':
                print("\n感谢使用！再见！")
                break
            elif choice == '1':
                enhanced_import()
            elif choice == '2':
                enhanced_business_analysis()
            elif choice == '3':
                data_validation_comparison()
            elif choice == '4':
                verify_data_quality()
            else:
                print("\n无效选择，请重新输入。")
                
            if choice != '0':
                input("\n按回车键继续...")
                
        except KeyboardInterrupt:
            print("\n\n程序被用户中断。")
            break
        except EOFError:
            print("\n发生错误: EOF when reading a line")
            break
        except Exception as e:
            print(f"\n发生错误: {e}")
            input("按回车键继续...")

if __name__ == "__main__":
    main()
