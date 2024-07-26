import pandas as pd
import rqdatac
from datetime import datetime, timedelta
import redis
import time
import json


rqdatac.init("license", "DvJCo50puUe8uL6BV5p1HGc7ncC5ENYrEYmrFing2-DjNq7sDPnYn8TOe8J3sKQ-ciCDF2F2kRarnrMvpJVH0EXS7PoYKpEkkKS_24DP8qjmQhxTNL5dH5Lxjtx7B2aSx0HcgdbJhU1FrNM9YVZA2htQZzgMYL2oaOSbOiOngE4=CzahxBeSgoGQ50Ycm_kuaVSBX9yeFiryUYFKp8Ui5F4UgGtZQseDfmKiyPeCm2fDaHTlBX7v0OV9n4I3vYT-YuzkJxBoHm8-BJvZ6VqhPSOMmvclnzD0fBgdeCZwzoDWtLwljNhtLvCzMMUlaepoo5o1Km0cm37LiXa8JtZcg2I=")

redis_pool = redis.ConnectionPool(host='192.168.71.215', 
                                  port= 20184, 
                                  password= 'P7pO48Lw4AZTOLXKlR', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)



today = datetime.now().date().strftime("%Y-%m-%d")


df = rqdatac.all_instruments(type="CS", market='cn', date=today)

order_book_ids = df["order_book_id"].to_list()


is_suspended_df = rqdatac.is_suspended(order_book_ids, 
                                        start_date = today, 
                                        end_date = today, 
                                        market='cn')
#print(is_suspended_df.T)
df = is_suspended_df.T
df.columns = ["is_suspended"]

df.reset_index(inplace=True)

#print(df)
df = df[df["is_suspended"] == True]
#print(df)

order_book_ids = df["index"].to_list()

df = rqdatac.current_minute(order_book_ids, skip_suspended=False)


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


df.reset_index(inplace=True)
df["exchange"] = df["order_book_id"].apply(lambda x: exchange_dict[x.split(".")[1]])
df["symbol"] = df["order_book_id"].apply(lambda x: x.split(".")[0])
df["wind_code"] = df["symbol"] + "." + df["exchange"]
#print(df)

now = datetime.now().timestamp()
now_timestamp = int(now * 1000)

records = df.to_dict(orient="records")


def on_data():
    now = datetime.now().timestamp()
    now_timestamp = int(now * 1000)
    datas = {

    }
    for record in records:
        datas[record["wind_code"]] = {
            "time": now_timestamp,
            "lastPrice": record["close"],
            "open": record["open"],
            "high": record["high"],
            "low": record["low"],
            "lastClose": record["close"],
            "amount": 0,
            "volume": 0,
            "pvolume": 0,
            "stockStatus": 3,
            "openInt": 13,
            "transactionNum": 0,
            "lastSettlementPrice": 0.0,
            "settlementPrice": 0.0,
            "pe": 0.0,
            "askPrice": [0.0, 0.0, 0.0, 0.0, 0.0],
            "bidPrice": [0.0, 0.0, 0.0, 0.0, 0.0],
            "askVol": [0,0,0,0,0],
            "bidVol": [0,0,0,0,0],
            "volRatio": 0.0,
            "speed1Min": 0.0,
            "speed5Min": 0.0
        }

    print(datas["000836.SZ"])

    redis_conn.xadd("oc_xt_quote",
        {
            "snapshot": json.dumps(datas)
        }
        , maxlen= 2000
    )

if __name__ == "__main__":
    while True:
        on_data()
        time.sleep(3)
    pass