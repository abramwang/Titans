#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG

def upgrade_database_for_business_sections():
    """升级数据库以支持4个业务部分"""
    print("="*80)
    print("升级数据库结构以支持4个业务部分")
    print("="*80)
    
    connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
    engine = create_engine(connection_string)
    
    try:
        with engine.connect() as conn:
            # 检查当前表结构
            print("1. 检查当前daily_trading_details表结构...")
            columns = conn.execute(text("SHOW COLUMNS FROM daily_trading_details")).fetchall()
            existing_columns = [col[0] for col in columns]
            print(f"   现有字段: {existing_columns}")
            
            # 添加section_type字段
            if 'section_type' not in existing_columns:
                print("\n2. 添加section_type字段...")
                conn.execute(text("""
                    ALTER TABLE daily_trading_details 
                    ADD COLUMN section_type VARCHAR(20) DEFAULT NULL 
                    COMMENT '业务部分类型：日内底仓T0/ETF日内申赎/隔夜底仓/ETF赎回涨停票'
                """))
                print("   ✓ section_type字段添加成功")
            else:
                print("\n2. section_type字段已存在，跳过添加")
            
            # 添加position_type字段
            if 'position_type' not in existing_columns:
                print("\n3. 添加position_type字段...")
                conn.execute(text("""
                    ALTER TABLE daily_trading_details 
                    ADD COLUMN position_type VARCHAR(20) DEFAULT 'UNKNOWN' 
                    COMMENT '持仓类型：INTRADAY-日内/OVERNIGHT-隔夜'
                """))
                print("   ✓ position_type字段添加成功")
            else:
                print("\n3. position_type字段已存在，跳过添加")
            
            # 添加data_category字段
            if 'data_category' not in existing_columns:
                print("\n4. 添加data_category字段...")
                conn.execute(text("""
                    ALTER TABLE daily_trading_details 
                    ADD COLUMN data_category ENUM('TRADING', 'POSITION') DEFAULT 'TRADING' 
                    COMMENT '数据类别：TRADING-交易数据/POSITION-持仓数据'
                """))
                print("   ✓ data_category字段添加成功")
            else:
                print("\n4. data_category字段已存在，跳过添加")
            
            # 创建业务部分索引
            print("\n5. 创建业务部分相关索引...")
            try:
                conn.execute(text("""
                    CREATE INDEX idx_section_type ON daily_trading_details(section_type)
                """))
                print("   ✓ section_type索引创建成功")
            except Exception as e:
                if "Duplicate key name" in str(e):
                    print("   ✓ section_type索引已存在")
                else:
                    print(f"   ❌ 创建索引失败: {e}")
            
            try:
                conn.execute(text("""
                    CREATE INDEX idx_data_category ON daily_trading_details(data_category)
                """))
                print("   ✓ data_category索引创建成功")
            except Exception as e:
                if "Duplicate key name" in str(e):
                    print("   ✓ data_category索引已存在")
                else:
                    print(f"   ❌ 创建索引失败: {e}")
            
            # 提交所有更改
            conn.commit()
            
            print("\n6. 验证表结构...")
            updated_columns = conn.execute(text("SHOW COLUMNS FROM daily_trading_details")).fetchall()
            print("   更新后的字段:")
            for col in updated_columns:
                print(f"     - {col[0]}: {col[1]}")
            
            print("\n✅ 数据库升级完成！")
            print("现在支持的业务部分:")
            print("   • 日内底仓T0 (交易数据)")
            print("   • ETF日内申赎 (交易数据)")
            print("   • 隔夜底仓 (持仓数据)")
            print("   • ETF赎回涨停票 (持仓数据)")
            
    except Exception as e:
        print(f"❌ 数据库升级失败: {e}")

if __name__ == "__main__":
    upgrade_database_for_business_sections()
