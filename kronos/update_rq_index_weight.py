import pandas as pd
from sqlalchemy import create_engine, text
import rqdatac
from datetime import datetime, timedelta

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
    "INDX" : "INDX",
    "DCE": "DCE",
    "SHFE": "SHFE",
    "CZCE": "CZCE",
    "INE": "INE",
    "CFFEX": "CF",
    "GFEX": "GFEX",
}

def query_etf_instruments():
    df = pd.read_sql("select * from rq_base_etf_instruments Where status ='Active'", con=engine)

    index_list = df["underlying_order_book_id"].unique().tolist()
    del index_list[0]

    #print(index_list)
    print(len(index_list))
    return index_list

def query_index_instruments():
    df = pd.read_sql("select * from rq_base_index_instruments", con=engine)
    df = df[df["exchange"].isin(["SH", "SZ"])]
    print(df)

def query_index_weights(order_book_id):
    df = pd.read_sql(f"SELECT date FROM ti_index_weights WHERE order_book_id = '{order_book_id}' AND date = (SELECT MAX(date) FROM ti_index_weights)", con=engine)
    if df.empty:
        return datetime(2024, 1, 1).date()
    else:
        return df["date"].max()

def download_index_weights(order_book_ids):
    global engine
    sql = text("""
    CREATE TABLE IF NOT EXISTS ti_index_weights (
        date DATE,
        order_book_id VARCHAR(32),
        component_order_book_id VARCHAR(32),
        weight FLOAT,
        symbol VARCHAR(32),
        exchange VARCHAR(32),
        component_symbol VARCHAR(32),
        component_exchange VARCHAR(32),
        PRIMARY KEY (date, order_book_id, component_order_book_id)
    )
    """)
    with engine.connect() as connection:
        try:
            # 执行SQL语句
            result = connection.execute(sql)
            # 处理结果（如果有）
            for row in result:
                print(row)
        except Exception as e:
            print(f"An error occurred: {e}")
        finally:
            # 确保连接被关闭
            connection.close()

    for order_book_id in order_book_ids:
        begin_date = query_index_weights(order_book_id)
        print(f"download_index_weights {order_book_id} 开始下载", begin_date)
        if begin_date is None:
            continue
        if begin_date == datetime.now().date():
            print(f"download_index_weights {order_book_id} 今天已经下载")
            continue

        #print(index_weights_df)
        try:
            symbol = order_book_id.split(".")[0]
            exchange = exchange_dict[order_book_id.split(".")[1]]
            index_weights_df = rqdatac.index_weights(order_book_id, 
                                                start_date=(begin_date + timedelta(days=1)).strftime("%Y-%m-%d"), 
                                                end_date=datetime.now().date().strftime("%Y-%m-%d"), 
                                                market='cn')
            
            index_weights_df.reset_index(inplace=True)
            index_weights_df["component_symbol"] = index_weights_df["order_book_id"].str.split(".", expand=True)[0]
            index_weights_df["component_exchange"] = index_weights_df["order_book_id"].str.split(".", expand=True)[1].map(exchange_dict)
            index_weights_df.rename(columns={"order_book_id": "component_order_book_id"}, inplace=True)
            index_weights_df["order_book_id"] = order_book_id
            index_weights_df["symbol"] = symbol
            index_weights_df["exchange"] = exchange

            index_weights_df.to_sql("ti_index_weights", con=engine, if_exists='append', index=False)
        except:
            print(f"error: {order_book_id}")
            continue
        #break
    print("download_index_weights 基础信息下载完成")

def main():
    order_book_ids = query_etf_instruments()
    download_index_weights(order_book_ids)

if __name__ == "__main__":
    main()
