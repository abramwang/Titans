import pandas as pd
from sqlalchemy import create_engine
import rqdatac

# 创建MySQL数据库连接引擎
# 请替换以下参数为您的数据库信息
username = 'root'
password = 'fed10b43-ff8d-44ef-ab5a-9c2443790f5d'
host = '172.17.0.1'
port = '20000'  # MySQL默认端口
database = 'ti_data'

# 创建数据库引擎
engine = create_engine(f'mysql+mysqldb://{username}:{password}@{host}:{port}/{database}')


rqdatac.init()
#rqdatac.init("license","g4po_ijeLmLocHb42KlSJlyoJfhttsva4DLTHhiPCj7CJ68XOtuoOWO8Lpf9rNuOehA30mZa5Gx3cbaGeFn9MEI2iw2NxyOfOkUpMY9RleBDONF0OQMnHKEKNDG9qGKQwKzTIXOeZqAiaH2uxa9Rw72iZk-7CgyjCBuoXpOAxc0=QE4nBAaytdktLQt3S4Bi5-lHaGcnfZSJcmL0z8m8jdJI77kh722PPK5usbY-5EkZzMNio1g0w0dpMaE7MxuN7II5nw3aAhMRjZ5kYX7lR0q6rcgD2lCke879PhITb5hZ8cnSQaygNZ0jDKnrkKzOls698b1vQ-yjW5WWAh4Y8q4=")

exchange_dict = {
    "XSHG": "SH",
    "XSHE": "SZ",
    "DCE": "DCE",
    "SHFE": "SHFE",
    "CZCE": "CZCE",
    "INE": "INE",
    "CFFEX": "CF",
    "GFEX": "GFEX",
}

def download_etf_instruments():
    instruments_df = rqdatac.all_instruments(type="ETF", market='cn', date=None)
    instruments_df["symbol"] = instruments_df["order_book_id"].str.split(".", expand=True)[0]
    instruments_df["exchange"] = instruments_df["order_book_id"].str.split(".", expand=True)[1].map(exchange_dict)
    instruments_df["underlying_symbol"] = instruments_df["underlying_order_book_id"].str.split(".", expand=True)[0]
    instruments_df["underlying_exchange"] = instruments_df["underlying_order_book_id"].str.split(".", expand=True)[1].map(exchange_dict)

    instruments_df.to_sql("rq_base_etf_instruments", con=engine, if_exists='replace', index=False)

    print("ETF 基础信息下载完成")

def download_stock_instruments():
    instruments_df = rqdatac.all_instruments(type="CS", market='cn', date=None)
    instruments_df["symbol"] = instruments_df["order_book_id"].str.split(".", expand=True)[0]
    instruments_df["exchange"] = instruments_df["order_book_id"].str.split(".", expand=True)[1].map(exchange_dict)

    instruments_df.to_sql("rq_base_stock_instruments", con=engine, if_exists='replace', index=False)

    print("stock 基础信息下载完成")

def download_index_instruments():
    instruments_df = rqdatac.all_instruments(type="INDX", market='cn', date=None)
    instruments_df["symbol"] = instruments_df["order_book_id"].str.split(".", expand=True)[0]
    instruments_df["exchange"] = instruments_df["order_book_id"].str.split(".", expand=True)[1].map(exchange_dict)

    instruments_df.to_sql("rq_base_index_instruments", con=engine, if_exists='replace', index=False)

    print("index 基础信息下载完成")

def download_future_instruments():
    instruments_df = rqdatac.all_instruments(type="Future", market='cn', date=None)
    
    instruments_df["name"] = instruments_df["symbol"]
    instruments_df["symbol"] = instruments_df["order_book_id"]
    instruments_df["exchange"] = instruments_df["exchange"].map(exchange_dict)

    instruments_df.to_sql("rq_base_future_instruments", con=engine, if_exists='replace', index=False)

    print("future 基础信息下载完成")


if __name__ == "__main__":
    download_etf_instruments()
    download_stock_instruments()
    download_index_instruments()
    download_future_instruments()