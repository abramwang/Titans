import pymoneta.quote as quote
import pymoneta.data as data
import pandas as pd
import numpy as np
from sklearn.linear_model import LinearRegression
from sqlalchemy import create_engine

# 创建MySQL数据库连接引擎
# 请替换以下参数为您的数据库信息
username = 'root'
password = 'fed10b43-ff8d-44ef-ab5a-9c2443790f5d'
host = '192.168.71.215'
port = '20000'  # MySQL默认端口

# 创建数据库引擎
ti_data_engine = create_engine(f'mysql+mysqlconnector://{username}:{password}@{host}:{port}/ti_data')
pcf_engine = create_engine(f'mysql+mysqlconnector://{username}:{password}@{host}:{port}/pcf')

def get_etf_list():
    pass

def main():
    get_etf_list()
    pass

if __name__ == "__main__":
    main()