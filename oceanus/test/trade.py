import redis
import json

redis_pool = redis.ConnectionPool(
    host='127.0.0.1', port= 20184, password= 'P7pO48Lw4AZTOLXKlR', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)


streamKey = "oc_trader_commander_huaxin.00043346"

def enterOrder():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "000001",
                    "szExchange" : "SZ",
                    "nTradeSideType" : 'B',
                    "nOffsetType" : 'C',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 11.26,
                    "nOrderVol" : 100000
                }
            )
        }
    )
    
enterOrder()