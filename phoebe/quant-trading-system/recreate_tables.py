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

def recreate_tables():
    """重新创建数据库表"""
    try:
        db_manager = DatabaseManager()
        session = db_manager.get_session()
        
        try:
            print("删除现有的import_log表...")
            session.execute(text("DROP TABLE IF EXISTS import_log"))
            session.commit()
            
            print("重新创建所有表...")
            db_manager.create_tables()
            
            print("检查新的表结构...")
            result = session.execute(text("DESCRIBE import_log"))
            columns = result.fetchall()
            print("\n新的import_log表结构:")
            for col in columns:
                print(f"  {col}")
                
        finally:
            session.close()
            
    except Exception as e:
        print(f"重新创建表失败: {str(e)}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    recreate_tables()
