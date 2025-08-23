#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os

# 添加项目路径
project_root = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, project_root)
sys.path.insert(0, os.path.join(project_root, 'src'))

from src.models.database import DatabaseManager
from sqlalchemy import text

def check_tables():
    """检查数据库表结构"""
    try:
        db_manager = DatabaseManager()
        
        # 使用session查询
        session = db_manager.get_session()
        
        try:
            # 检查import_log表是否存在
            result = session.execute(text("SHOW TABLES LIKE 'import_log'"))
            tables = result.fetchall()
            print(f"import_log表存在: {len(tables) > 0}")
            
            if len(tables) > 0:
                # 检查表结构
                result = session.execute(text("DESCRIBE import_log"))
                columns = result.fetchall()
                print("\nimport_log表结构:")
                for col in columns:
                    print(f"  {col}")
            else:
                print("import_log表不存在，创建表...")
                # 重新创建表
                db_manager.create_tables()
                print("表创建完成")
                
        finally:
            session.close()
            
    except Exception as e:
        print(f"检查表失败: {str(e)}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    check_tables()
