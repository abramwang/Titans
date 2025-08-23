#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os

# 添加项目路径
project_root = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, project_root)
sys.path.insert(0, os.path.join(project_root, 'src'))

from src.models.database import DatabaseManager, ImportLog
from sqlalchemy import text

def check_import_log():
    """检查导入日志"""
    try:
        db_manager = DatabaseManager()
        session = db_manager.get_session()
        
        try:
            # 查看导入日志
            logs = session.query(ImportLog).all()
            print(f"导入日志记录数: {len(logs)}")
            
            for log in logs:
                print(f"文件: {log.file_name}, 状态: {log.import_status}, 记录数: {log.records_imported}")
                
        finally:
            session.close()
            
    except Exception as e:
        print(f"检查导入日志失败: {str(e)}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    check_import_log()
