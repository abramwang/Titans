import pandas as pd
from sqlalchemy import create_engine
import redis
import json

redis_pool = redis.ConnectionPool(host='172.17.0.1', port= 20184, password= 'P7pO48Lw4AZTOLXKlR', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)

# 创建MySQL数据库连接引擎
# 请替换以下参数为您的数据库信息
username = 'root'
password = 'fed10b43-ff8d-44ef-ab5a-9c2443790f5d'
host = '172.17.0.1'
port = '20000'  # MySQL默认端口
database = 'ti_data'

# 创建数据库引擎
engine = create_engine(f'mysql+mysqldb://{username}:{password}@{host}:{port}/{database}')


from xtquant import xtdata

def connect():
    ip = '172.17.0.1'
    port = 58610
    xtdata.connect(ip=ip, port=port)

connect()

def get_instrument_info():
    index_df = pd.read_sql_query("SELECT symbol, exchange FROM rq_base_index_instruments WHERE status = 'Active';", engine)
    index_df["type"] = "index"

    stock_df = pd.read_sql_query("SELECT symbol, exchange FROM rq_base_stock_instruments WHERE status = 'Active';", engine)
    stock_df["type"] = "stock"

    etf_df = pd.read_sql_query("SELECT symbol, exchange FROM rq_base_etf_instruments WHERE status = 'Active';", engine)
    etf_df["type"] = "etf"
    
    df = pd.concat([index_df, stock_df, etf_df])
    df["code"] = df["symbol"] + "." + df["exchange"]

    code_list = df["code"].to_list()
    instrument_info = {}
    for code in code_list:
        try:
            instrument_info[code] = xtdata.get_instrument_detail(code)
        except:
            print(f"获取{code}失败")

    instrument_info_df = pd.DataFrame(instrument_info).T
    instrument_info_df["exchange"] = instrument_info_df["ExchangeID"]
    instrument_info_df["symbol"] = instrument_info_df["InstrumentID"]
    instrument_info_df["name"] = instrument_info_df["InstrumentName"]

    df = pd.merge(df, instrument_info_df, on=["symbol", "exchange"], how="inner")
    df = df[["symbol", "exchange", "name", "type", "UpStopPrice", "DownStopPrice", "PreClose"]]
    df.rename(columns={"UpStopPrice": "limit_up", "DownStopPrice": "limit_down", "PreClose": "pre_close"}, inplace=True)
    df["update_date"] = pd.Timestamp.now().date()
    df.to_sql("ti_instrument_info", con=engine, if_exists='replace', index=False)

#订阅最新行情
def on_data(datas):
    global instrument_info
    global instrument_len
    global instrument_full_time

    redis_conn.xadd("oc_xt_quote",
        {
            "snapshot": json.dumps(datas)
        }
        , maxlen= 2000
    )

if __name__ == "__main__":
    get_instrument_info()
    xtdata.subscribe_whole_quote(['SH', 'SZ'], callback=on_data)
    #死循环 阻塞主线程退出
    xtdata.run()