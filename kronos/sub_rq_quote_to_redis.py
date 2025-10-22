import rqdatac
from datetime import datetime
from rqdatac import LiveMarketDataClient
import redis
import json

rqdatac.init("license","g4po_ijeLmLocHb42KlSJlyoJfhttsva4DLTHhiPCj7CJ68XOtuoOWO8Lpf9rNuOehA30mZa5Gx3cbaGeFn9MEI2iw2NxyOfOkUpMY9RleBDONF0OQMnHKEKNDG9qGKQwKzTIXOeZqAiaH2uxa9Rw72iZk-7CgyjCBuoXpOAxc0=QE4nBAaytdktLQt3S4Bi5-lHaGcnfZSJcmL0z8m8jdJI77kh722PPK5usbY-5EkZzMNio1g0w0dpMaE7MxuN7II5nw3aAhMRjZ5kYX7lR0q6rcgD2lCke879PhITb5hZ8cnSQaygNZ0jDKnrkKzOls698b1vQ-yjW5WWAh4Y8q4=")

redis_pool = redis.ConnectionPool(host='192.168.3.100', 
                                  port= 6379, 
                                  password= 'SEZmVCbnskkB7nebDT6cs3hZW5dnhyup', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)


df = rqdatac.all_instruments(type="ETF", market='cn', date=None)
order_book_id_list = df['order_book_id'].tolist()
order_book_id_list = ['tick_' + oid for oid in order_book_id_list]

client = LiveMarketDataClient()
# 订阅一支tick标的
client.subscribe(order_book_id_list)

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


print(order_book_id_list)

for market in client.listen():
    #print(datetime.now(), market)
    data = market
    data["exchange"] = exchange_dict[market["order_book_id"].split(".")[1]] 
    data["symbol"] = market["order_book_id"].split(".")[0]
    del data["channel"]
    del data["action"]
    
    redis_conn.xadd("oc_rq_quote",
        {
            "snapshot": json.dumps(market)
        }
        , maxlen= 200000
    )
