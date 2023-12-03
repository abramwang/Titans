import redis
import json

redis_pool = redis.ConnectionPool(
    host='47.103.74.35', port= 20184, password= 'W_P!ViW+d7xAFept6', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)


streamKey = "etf_semi_trader_xtp.253191001815"


def purchasing():
    redis_conn.xadd(streamKey,
        {
            "purchasing": json.dumps(
                {
                    #"etf_symbol": "512160",
                    "etf_symbol": "159601",
                }
            )
        }
    )


def redeeming():
    redis_conn.xadd(streamKey,
        {
            "redeeming": json.dumps(
                {
                    #"etf_symbol": "512160",
                    "etf_symbol": "159601",
                }
            )
        }
    )


redeeming()