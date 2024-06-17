import pymoneta.quote as quote
import pymoneta.data as data
import pandas as pd
import numpy as np
from datetime import datetime
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
today = datetime.now().strftime('%Y%m%d')

def get_etf_code_list():
    etf_info_df = pd.read_sql(f"SELECT * FROM xt_etf_info WHERE m_tradeDate = '{today}'", pcf_engine)
    etf_info_df.rename(columns={'m_fundId': 'symbol', 'm_exchange':'exchange'}, inplace=True)
   

    etf_instruments_df = pd.read_sql("SELECT * FROM rq_base_etf_instruments WHERE status ='Active'", ti_data_engine)
    etf_instruments_df = etf_instruments_df[["symbol","exchange", "order_book_id", "underlying_order_book_id", "underlying_symbol", "underlying_exchange"]]
    
    etf_info_merge_df = pd.merge(etf_info_df, etf_instruments_df, on=["symbol", "exchange"], how="left")
    
    etf_code_list = []
    for index, row in etf_info_merge_df.iterrows():
        etf_code_list.append({
            "symbol": row["symbol"],
            "exchange": row["exchange"],
            "order_book_id": row["order_book_id"]
        })
    print(len(etf_code_list), etf_code_list[0], etf_code_list[-1])
    return etf_code_list

def main():
    get_etf_code_list()
    pass

if __name__ == "__main__":
    main()