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



client = LiveMarketDataClient()
# 订阅一支tick标的
client.subscribe(['tick_000001.XSHE', 'tick_000002.XSHE'])

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

def convert_timestamp_to_datetime(timestamp: int) -> datetime:
    """
    Convert a timestamp integer in the format YYYYMMDDHHMMSSmmm to a datetime object.
    This version uses integer arithmetic for efficiency, avoiding string conversion.
    
    Args:
        timestamp (int): The timestamp, e.g., 20251022094221000
    
    Returns:
        datetime: The corresponding datetime object
    """
    # Extract components using integer division and modulo
    year = timestamp // 10000000000000
    month = (timestamp // 100000000000) % 100
    day = (timestamp // 1000000000) % 100
    hour = (timestamp // 10000000) % 100
    minute = (timestamp // 100000) % 100
    second = (timestamp // 1000) % 100
    millisecond = timestamp % 1000
    microsecond = millisecond * 1000  # Convert milliseconds to microseconds
    
    return datetime(year, month, day, hour, minute, second, microsecond)


def convert_to_unix_timestamp(timestamp: int) -> float:
    """
    Convert a timestamp integer in the format YYYYMMDDHHMMSSmmm to a Unix timestamp (seconds since 1970-01-01 00:00:00 UTC).
    
    Args:
        timestamp (int): The timestamp, e.g., 20251022094221000
    
    Returns:
        float: The Unix timestamp
    """
    dt = convert_timestamp_to_datetime(timestamp)
    return dt.timestamp()


for market in client.listen():
    print(datetime.now(), market)
    data = market
    data["market"] = exchange_dict[market["order_book_id"].split(".")[1]] 
    data["symbol"] = market["order_book_id"].split(".")[0]
    del data["channel"]
    del data["action"]
    
    redis_conn.xadd("oc_rq_quote_tick",
        {
            "snapshot": json.dumps(market)
        }
        , maxlen= 2000
    )
