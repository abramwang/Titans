#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pymysql
from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def create_database_and_tables():
    """创建数据库和表结构"""
    try:
        # 首先连接到MySQL服务器（不指定数据库）
        connection_string_base = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/?charset={DATABASE_CONFIG['charset']}"
        engine_base = create_engine(connection_string_base)
        
        # 创建数据库
        with engine_base.connect() as conn:
            conn.execute(text(f"CREATE DATABASE IF NOT EXISTS {DATABASE_CONFIG['database']} DEFAULT CHARACTER SET utf8mb4 DEFAULT COLLATE utf8mb4_unicode_ci"))
            logger.info(f"数据库 {DATABASE_CONFIG['database']} 创建成功或已存在")
        
        # 连接到新创建的数据库
        connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        engine = create_engine(connection_string)
        
        # 读取SQL文件并执行
        with open('database_design.sql', 'r', encoding='utf-8') as f:
            sql_content = f.read()
        
        # 更智能地分割SQL语句
        sql_statements = []
        current_statement = ""
        in_comment = False
        
        for line in sql_content.split('\n'):
            line = line.strip()
            
            # 跳过注释行
            if line.startswith('--') or line.startswith('#'):
                continue
            
            # 跳过空行
            if not line:
                continue
                
            # 跳过数据库创建和USE语句
            if line.upper().startswith(('CREATE DATABASE', 'USE ')):
                continue
            
            current_statement += " " + line
            
            # 如果遇到分号，表示语句结束
            if line.endswith(';'):
                stmt = current_statement.strip()
                if stmt and not stmt.startswith('--'):
                    sql_statements.append(stmt[:-1])  # 移除最后的分号
                current_statement = ""
        
        # 添加最后一个语句（如果有的话）
        if current_statement.strip():
            stmt = current_statement.strip()
            if stmt and not stmt.startswith('--'):
                sql_statements.append(stmt)
        
        with engine.connect() as conn:
            for sql in sql_statements:
                if sql.strip():
                    try:
                        conn.execute(text(sql))
                        # 获取语句的前50个字符用于日志
                        stmt_preview = sql.replace('\n', ' ').strip()[:50]
                        logger.info(f"执行SQL成功: {stmt_preview}...")
                    except Exception as e:
                        stmt_preview = sql.replace('\n', ' ').strip()[:50]
                        logger.warning(f"执行SQL失败: {stmt_preview}..., 错误: {e}")
            
            # 提交事务
            conn.commit()
        
        logger.info("所有表创建成功！")
        return True
        
    except Exception as e:
        logger.error(f"创建数据库和表失败: {e}")
        return False

if __name__ == "__main__":
    success = create_database_and_tables()
    if success:
        print("数据库初始化成功！")
    else:
        print("数据库初始化失败！")
