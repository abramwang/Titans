#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from sqlalchemy import create_engine, Column, Integer, String, DateTime, Date, Text, Boolean, Numeric
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime
import sys
import os

# 添加项目根目录到Python路径
project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, project_root)

from config import DATABASE_CONFIG

Base = declarative_base()

class AccountInfo(Base):
    """账户信息表"""
    __tablename__ = 'account_info'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    account_id = Column(String(50), nullable=False, unique=True, comment='账户编号')
    account_name = Column(String(100), nullable=False, comment='账户名称')
    broker = Column(String(50), nullable=False, comment='券商名称')
    created_at = Column(DateTime, default=datetime.now)
    updated_at = Column(DateTime, default=datetime.now, onupdate=datetime.now)

class Securities(Base):
    """证券基础信息表"""
    __tablename__ = 'securities'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    security_code = Column(String(20), nullable=False, unique=True, comment='证券代码')
    security_name = Column(String(100), nullable=False, comment='证券名称')
    exchange = Column(String(10), nullable=False, comment='交易所')
    security_type = Column(String(20), default='ETF', comment='证券类型')
    is_active = Column(Boolean, default=True, comment='是否有效')
    created_at = Column(DateTime, default=datetime.now)
    updated_at = Column(DateTime, default=datetime.now, onupdate=datetime.now)

class DailyAccountAssets(Base):
    """日度账户资产表"""
    __tablename__ = 'daily_account_assets'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    account_id = Column(String(50), nullable=False, comment='账户编号')
    trade_date = Column(Date, nullable=False, comment='交易日期')
    start_assets = Column(Numeric(15, 2), comment='日初账户资产')
    end_assets = Column(Numeric(15, 2), comment='日末账户资产')
    net_value = Column(Numeric(10, 4), comment='净值')
    custody_net_value = Column(Numeric(10, 4), comment='托管净值')
    estimated_profit = Column(Numeric(15, 2), comment='预估盈利')
    floating_pnl = Column(Numeric(15, 2), comment='浮动盈亏')
    realized_pnl = Column(Numeric(15, 2), comment='已实现盈亏')
    total_pnl = Column(Numeric(15, 2), comment='总盈亏')
    return_rate = Column(Numeric(8, 4), comment='收益率')
    cumulative_return = Column(Numeric(8, 4), comment='累计收益率')
    created_at = Column(DateTime, default=datetime.now)
    updated_at = Column(DateTime, default=datetime.now, onupdate=datetime.now)

class TradingDetails(Base):
    """交易明细表"""
    __tablename__ = 'trading_details'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    account_id = Column(String(50), nullable=False, comment='账户编号')
    trade_date = Column(Date, nullable=False, comment='交易日期')
    security_code = Column(String(20), nullable=False, comment='证券代码')
    security_name = Column(String(100), nullable=False, comment='证券名称')
    business_type = Column(String(50), nullable=False, comment='业务类型')
    business_category = Column(String(20), nullable=False, default='trading', comment='业务分类: trading=交易明细, position=持仓明细')
    trade_direction = Column(String(10), nullable=False, comment='买卖方向')
    trade_volume = Column(Integer, comment='成交数量')
    trade_price = Column(Numeric(10, 4), comment='成交价格')
    trade_amount = Column(Numeric(15, 2), comment='成交金额')
    commission = Column(Numeric(10, 2), comment='手续费')
    stamp_duty = Column(Numeric(10, 2), comment='印花税')
    transfer_fee = Column(Numeric(10, 2), comment='过户费')
    total_fee = Column(Numeric(10, 2), comment='总费用')
    net_amount = Column(Numeric(15, 2), comment='净额')
    pnl = Column(Numeric(15, 2), comment='盈亏')
    created_at = Column(DateTime, default=datetime.now)
    updated_at = Column(DateTime, default=datetime.now, onupdate=datetime.now)

class CurrentPositions(Base):
    """当前持仓表"""
    __tablename__ = 'current_positions'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    account_id = Column(String(50), nullable=False, comment='账户编号')
    security_code = Column(String(20), nullable=False, comment='证券代码')
    security_name = Column(String(100), nullable=False, comment='证券名称')
    business_type = Column(String(50), nullable=False, comment='业务类型')
    current_volume = Column(Integer, default=0, comment='当前持仓数量')
    available_volume = Column(Integer, default=0, comment='可用数量')
    avg_cost = Column(Numeric(10, 4), comment='平均成本')
    current_price = Column(Numeric(10, 4), comment='当前价格')
    market_value = Column(Numeric(15, 2), comment='市值')
    cost_value = Column(Numeric(15, 2), comment='成本')
    unrealized_pnl = Column(Numeric(15, 2), comment='浮动盈亏')
    unrealized_pnl_rate = Column(Numeric(8, 4), comment='浮动盈亏比例')
    last_updated = Column(DateTime, default=datetime.now, onupdate=datetime.now)
    created_at = Column(DateTime, default=datetime.now)

class ImportLog(Base):
    """数据导入日志表"""
    __tablename__ = 'import_log'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    file_name = Column(String(255), nullable=False, comment='文件名')
    file_path = Column(Text, comment='文件路径')
    import_date = Column(Date, nullable=False, comment='导入日期')
    import_status = Column(String(20), nullable=False, comment='导入状态')
    records_imported = Column(Integer, default=0, comment='导入记录数')
    error_message = Column(Text, comment='错误信息')
    import_time = Column(DateTime, default=datetime.now, comment='导入时间')

class DatabaseManager:
    """数据库管理器"""
    
    def __init__(self):
        # 构建数据库连接字符串
        connection_string = (
            f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}"
            f"@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}"
            f"/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
        )
        
        self.engine = create_engine(connection_string, echo=False)
        self.SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=self.engine)
    
    def create_tables(self):
        """创建所有表"""
        Base.metadata.create_all(bind=self.engine)
    
    def get_session(self):
        """获取数据库会话"""
        return self.SessionLocal()
    
    def close_session(self, session):
        """关闭数据库会话"""
        if session:
            session.close()
