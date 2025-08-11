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
    print("1. 分析Excel文件结构")
    print("2. 详细分析Excel数据")
    print("3. 重新分析数据结构(明确汇总与明细关系)")
    print("4. 创建数据库和表结构")
    print("5. 导入Excel数据到数据库")
    print("6. 基于4个业务部分的增强数据导入")
    print("7. 基于业务部分的增强分析报告")
    print("8. 验证数据质量")
    print("9. 生成完整分析报告")
    print("0. 退出")
    print("0. 退出")
    print("="*60)

def analyze_excel():
    """分析Excel文件"""
    print("\n正在分析Excel文件...")
    os.system(f"{sys.executable} analyze_excel.py")

def detailed_analysis():
    """详细分析"""
    print("\n正在进行详细分析...")
    os.system(f"{sys.executable} detailed_analysis.py")

def reanalyze_structure():
    """重新分析数据结构"""
    print("\n正在重新分析数据结构...")
    os.system(f"{sys.executable} reanalyze_data.py")

def create_database():
    """创建数据库"""
    print("\n正在创建数据库和表结构...")
    os.system(f"{sys.executable} create_database.py")

def enhanced_import():
    """增强版数据导入"""
    print("\n正在运行增强版数据导入...")
    os.system(f"{sys.executable} enhanced_importer.py")

def enhanced_import():
    """基于4个业务部分的增强数据导入"""
    print("\n正在运行基于4个业务部分的增强数据导入...")
    os.system(f"{sys.executable} enhanced_importer.py")

def enhanced_business_analysis():
    """基于业务部分的增强分析报告"""
    print("\n正在生成基于4个业务部分的增强分析报告...")
    
    try:
        import pandas as pd
        from sqlalchemy import create_engine
        from config import DATABASE_CONFIG
        
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        engine = create_engine(connection_string)
        
        print("="*80)
        print("基于4个业务部分的增强分析报告")
        print("="*80)
        
        # 1. 业务部分概览
        section_overview = pd.read_sql("""
            SELECT 
                section_type,
                data_category,
                COUNT(*) as record_count,
                COUNT(DISTINCT trade_date) as trading_days,
                COUNT(DISTINCT security_code) as unique_securities,
                SUM(profit) as total_profit,
                AVG(profit) as avg_profit,
                MAX(profit) as max_profit,
                MIN(profit) as min_profit
            FROM daily_trading_details 
            WHERE section_type IS NOT NULL
            GROUP BY section_type, data_category
            ORDER BY data_category, total_profit DESC
        """, engine)
        
        print("\n1. 业务部分概览:")
        print(section_overview.to_string(index=False))
        
        # 2. 日度业绩趋势
        daily_trend = pd.read_sql("""
            SELECT 
                trade_date,
                section_type,
                COUNT(*) as records,
                SUM(profit) as daily_profit
            FROM daily_trading_details 
            WHERE section_type IS NOT NULL
            GROUP BY trade_date, section_type
            ORDER BY trade_date, section_type
        """, engine)
        
        print("\n2. 日度业绩趋势 (最近10天):")
        print(daily_trend.tail(20).to_string(index=False))
        
        # 3. 交易 vs 持仓数据对比
        category_comparison = pd.read_sql("""
            SELECT 
                data_category,
                COUNT(*) as total_records,
                COUNT(DISTINCT security_code) as unique_securities,
                SUM(profit) as total_profit,
                AVG(profit) as avg_profit
            FROM daily_trading_details
            GROUP BY data_category
        """, engine)
        
        print("\n3. 交易 vs 持仓数据对比:")
        print(category_comparison.to_string(index=False))
        
        # 4. 证券表现排行
        security_performance = pd.read_sql("""
            SELECT 
                security_code,
                section_type,
                COUNT(*) as appear_count,
                SUM(profit) as total_profit,
                AVG(profit) as avg_profit
            FROM daily_trading_details
            WHERE section_type IS NOT NULL
            GROUP BY security_code, section_type
            HAVING total_profit != 0
            ORDER BY total_profit DESC
            LIMIT 15
        """, engine)
        
        print("\n4. 证券表现排行 (前15名):")
        print(security_performance.to_string(index=False))
        
        # 5. 业务部分盈利贡献分析
        profit_contribution = pd.read_sql("""
            SELECT 
                section_type,
                SUM(profit) as section_profit,
                SUM(profit) / (SELECT SUM(profit) FROM daily_trading_details WHERE profit > 0) * 100 as profit_contribution_pct
            FROM daily_trading_details
            WHERE section_type IS NOT NULL AND profit > 0
            GROUP BY section_type
            ORDER BY section_profit DESC
        """, engine)
        
        print("\n5. 盈利贡献分析:")
        print(profit_contribution.to_string(index=False))
        
        print("\n" + "="*80)
        print("分析完成")
        print("="*80)
        
    except Exception as e:
        print(f"生成增强分析报告时出错: {e}")

def import_data():
    """导入数据"""
    print("\n正在导入Excel数据到数据库...")
    os.system(f"{sys.executable} data_importer.py")

def verify_data():
    """验证数据"""
    print("\n正在验证导入的数据...")
    os.system(f"{sys.executable} verify_data.py")

def advanced_analysis():
    """高级分析"""
    print("\n正在生成高级分析报告...")
    os.system(f"{sys.executable} advanced_analysis.py")

def show_status():
    """显示系统状态"""
    print("\n系统状态:")
    print("-"*40)
    
    # 检查文件
    files_to_check = [
        'analyze_excel.py',
        'detailed_analysis.py', 
        'create_database.py',
        'data_importer.py',
        'verify_data.py',
        'advanced_analysis.py',
        'config.py',
        'database_design.sql'
    ]
    
    print("核心文件检查:")
    for file in files_to_check:
        status = "✓" if os.path.exists(file) else "✗"
        print(f"  {status} {file}")
    
    # 检查数据文件
    data_files = []
    if os.path.exists('data'):
        data_files = [f for f in os.listdir('data') if f.endswith('.xlsx')]
    
    print(f"\nExcel数据文件: {len(data_files)} 个")
    if data_files:
        for file in data_files[:5]:  # 只显示前5个
            print(f"  - {file}")
        if len(data_files) > 5:
            print(f"  ... 还有 {len(data_files) - 5} 个文件")
    
    # 检查数据库连接
    try:
        from config import DATABASE_CONFIG
        print(f"\n数据库配置:")
        print(f"  主机: {DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}")
        print(f"  数据库: {DATABASE_CONFIG['database']}")
        print(f"  用户: {DATABASE_CONFIG['user']}")
    except Exception as e:
        print(f"\n数据库配置错误: {e}")

def main():
    """主函数"""
    while True:
        try:
            print_menu()
            choice = input("请选择操作 (0-9): ").strip()
            
            if choice == '0':
                print("\n感谢使用！再见！")
                break
            elif choice == '1':
                analyze_excel()
            elif choice == '2':
                detailed_analysis()
            elif choice == '3':
                reanalyze_structure()
            elif choice == '4':
                create_database()
            elif choice == '5':
                import_data()
            elif choice == '6':
                enhanced_import()
            elif choice == '7':
                enhanced_business_analysis()
            elif choice == '8':
                verify_data()
            elif choice == '9':
                advanced_analysis()
            elif choice == '0':
                print("\n感谢使用！再见！")
                break
            else:
                print("\n无效选择，请重新输入！")
            
            input("\n按回车键继续...")
            
        except KeyboardInterrupt:
            print("\n\n程序被用户中断，再见！")
            break
        except Exception as e:
            print(f"\n发生错误: {e}")
            input("按回车键继续...")

if __name__ == "__main__":
    main()
