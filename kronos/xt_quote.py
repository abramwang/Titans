import pandas as pd
from sqlalchemy import create_engine
import sqlalchemy
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
    # ip = '127.0.0.1'
    # port = 58609
    xtdata.connect(ip=ip, port=port)


connect()


instrument_info = {}
instrument_len = 0
instrument_full_time = 0

"""
{
    "ExchangeID": "SZ",
    "InstrumentID": "000001",
    "InstrumentName": "平安银行",
    "ProductID": "",
    "ProductName": "",
    "ExchangeCode": "000001",
    "UniCode": "000001",
    "CreateDate": "0",
    "OpenDate": "19910403",
    "ExpireDate": 99999999,
    "PreClose": 10.99,
    "SettlementPrice": 10.99,
    "UpStopPrice": 12.09,
    "DownStopPrice": 9.89,
    "FloatVolume": 19405546950.0,
    "TotalVolume": 19405918198.0,
    "LongMarginRatio": 1.7976931348623157e+308,
    "ShortMarginRatio": 1.7976931348623157e+308,
    "PriceTick": 0.01,
    "VolumeMultiple": 1,
    "MainContract": 2147483647,
    "LastVolume": 2147483647,
    "InstrumentStatus": 0,
    "IsTrading": False,
    "IsRecent": False,
    "ProductTradeQuota": 1515397181,
    "ContractTradeQuota": 0,
    "ProductOpenInterestQuota": 0,
    "ContractOpenInterestQuota": 0
}
"""
def update_instrument_info_sql(instrument_info):
    global engine
    table_name = "xt_instrument_info"
    df = pd.DataFrame(instrument_info).T
    print(df)
    df.replace(1.7976931348623157e+308, None, inplace=True)
    df.replace(2147483647, None, inplace=True)
    df.dropna(axis=0, how='all', inplace=True)
    df["UpdateDate"] = pd.Timestamp.now().date()
    #df = df.applymap(str)
    df.to_sql(table_name, con=engine, if_exists='replace', index=False,
                dtype={
                    'ExchangeID': sqlalchemy.types.VARCHAR(length=10),
                    'InstrumentID': sqlalchemy.types.VARCHAR(length=32),
                    'InstrumentName': sqlalchemy.types.VARCHAR(length=50),
                    'ProductID': sqlalchemy.types.Text,
                    'ProductName': sqlalchemy.types.Text,
                    'ExchangeCode': sqlalchemy.types.VARCHAR(length=32),
                    'UniCode': sqlalchemy.types.VARCHAR(length=32),
                    'CreateDate': sqlalchemy.types.Text,
                    'OpenDate': sqlalchemy.types.Text,
                    'ExpireDate': sqlalchemy.types.Text,
                    'PreClose': sqlalchemy.types.Float(precision=3),
                    'SettlementPrice': sqlalchemy.types.Float(precision=3),
                    'UpStopPrice': sqlalchemy.types.Float(precision=3),
                    'DownStopPrice': sqlalchemy.types.Float(precision=3),
                    'FloatVolume': sqlalchemy.types.Float(precision=3),
                    'TotalVolume': sqlalchemy.types.Float(precision=3),
                    'LongMarginRatio': sqlalchemy.types.Float(precision=3),
                    'ShortMarginRatio': sqlalchemy.types.Float(precision=3),
                    'PriceTick': sqlalchemy.types.Float(precision=3),
                    'VolumeMultiple': sqlalchemy.types.Integer,
                    'MainContract': sqlalchemy.types.BigInteger,
                    'LastVolume': sqlalchemy.types.BigInteger,
                    'InstrumentStatus': sqlalchemy.types.Integer,
                    'IsTrading': sqlalchemy.types.Boolean,
                    'IsRecent': sqlalchemy.types.Boolean,
                    'ProductTradeQuota': sqlalchemy.types.BigInteger,
                    'ContractTradeQuota': sqlalchemy.types.BigInteger,
                    'ProductOpenInterestQuota': sqlalchemy.types.BigInteger,
                    'ContractOpenInterestQuota': sqlalchemy.types.BigInteger,
            })
    print(f"Data has been written to {table_name} in {database} database.")

#订阅最新行情
def on_data(datas):
    global instrument_info
    global instrument_len
    global instrument_full_time

    is_printed = False

    redis_conn.xadd("oc_xt_quote",
        {
            "snapshot": json.dumps(datas)
        }
        , maxlen= 2000
    )
    if instrument_full_time > 3:
        return
    for symbol, data in datas.items():
        if symbol not in instrument_info:
            instrument_info[symbol] = xtdata.get_instrument_detail(symbol)
            #instrument_info[symbol]["Type"] = xtdata.get_instrument_type(symbol)
        if not is_printed:
            print('instrument', instrument_info[symbol])
            is_printed = True
        #print(symbol, data)
        #break
    new_len = len(instrument_info)
    if new_len > instrument_len:
        print('instrument_info new len', new_len)
        instrument_len = new_len
    else:
        print('instrument_info len', instrument_len)
        print('instrument_full', instrument_full_time)
        update_instrument_info_sql(instrument_info)
        instrument_full_time += 1

xtdata.subscribe_whole_quote(['SH', 'SZ'], callback=on_data)

#死循环 阻塞主线程退出
xtdata.run()