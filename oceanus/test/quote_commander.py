import redis
import json

redis_pool = redis.ConnectionPool(
    host='127.0.0.1', port= 20184, password= 'kymq20180', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)

streamKey = "oc_quote_commander.lw"


def querySymbolList():
    redis_conn.xadd(streamKey,
        {
            "querySymbolList": json.dumps({})
        }
    )

def registerSymbol():
    redis_conn.xadd(streamKey,
        {
            "registerSymbol": json.dumps({
               "type"   :  54,
               "symbol" : "cu2304"
            })
        }
    )

def registerSymbol2():
    redis_conn.xadd(streamKey,
        {
            "registerSymbol": json.dumps({
               "type"   :  54,
               "symbol" : "rb2304"
            })
        }
    )

#querySymbolList()
registerSymbol()
registerSymbol2()