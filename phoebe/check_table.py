#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from sqlalchemy import create_engine, text
from config import DATABASE_CONFIG
import pandas as pd

try:
    connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
    engine = create_engine(connection_string)
    
    print("Checking daily_trading_details table structure:")
    result = pd.read_sql("DESCRIBE daily_trading_details", engine)
    print(result)
    print()
    
    print("Sample data from daily_trading_details:")
    sample = pd.read_sql("SELECT * FROM daily_trading_details LIMIT 3", engine)
    print(sample.columns.tolist())
    print(sample)
    
except Exception as e:
    print(f"Error: {e}")
