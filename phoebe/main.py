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
    print("6. 验证导入的数据")
    print("7. 生成高级分析报告")
    print("8. 查看系统状态")
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
            choice = input("请选择操作 (0-8): ").strip()
            
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
                verify_data()
            elif choice == '7':
                advanced_analysis()
            elif choice == '8':
                show_status()
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
