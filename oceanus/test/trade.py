import redis
import json

redis_pool = redis.ConnectionPool(
    host='127.0.0.1', port= 20184, password= 'P7pO48Lw4AZTOLXKlR', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)


streamKey = "oc_trader_commander_huaxin.00043346"

def querySymbolList():
    redis_conn.xadd(streamKey,
        {
            "querySymbolList": json.dumps({})
        }
    )

def queryUserAccount():
    redis_conn.xadd(streamKey,
        {
            "queryUserAccount": json.dumps({})
        }
    )

def queryAccountPosition():
    redis_conn.xadd(streamKey,
        {
            "queryAccountPosition": json.dumps(
                {
                    "nReqId" : 1,
                    "accountId" : "07420"
                }
            )
            
        }
    )

def queryAccountBP():
    redis_conn.xadd(streamKey,
        {
            "queryAccountBP": json.dumps(
                {
                    "nReqId" : 1,
                    "accountId" : "07420"
                }
            )
        }
    )

def queryAccountTradeMargin():
    redis_conn.xadd(streamKey,
        {
            "queryAccountTradeMargin": json.dumps(
                {
                    "accountId" : "07420"
                }
            )
        }
    )

def queryAccountTradeFee():
    redis_conn.xadd(streamKey,
        {
            "queryAccountTradeFee": json.dumps(
                {
                    "accountId" : "07420"
                }
            )
        }
    )

def queryAccountOrder():
    redis_conn.xadd(streamKey,
        {
            "queryAccountOrder": json.dumps(
                {
                    "accountId" : "07420"
                }
            )
        }
    )

def queryAccountOrderExecution():
    redis_conn.xadd(streamKey,
        {
            "queryAccountOrderExecution": json.dumps(
                {
                    "accountId" : "07420"
                }
            )
        }
    )

def queryMarketMBLData():
    redis_conn.xadd(streamKey,
        {
            "queryMarketMBLData": json.dumps(
                {
                    "nReqId" : 2,
                    "exchangeID" : 103,
                    "nSide" : 1,
                }
            )
        }
    )

def cancelOrder(m_MarketOrderToken):
    redis_conn.xadd(streamKey,
        {
            "cancelOrder": json.dumps(
                {
                    "m_Account" : "07420",
                    "m_MarketOrderToken" : m_MarketOrderToken
                }
            )
        }
    )

def QueryAsset():
    redis_conn.xadd(streamKey,
        {
            "QueryAsset": json.dumps({})
        }
    )

def QueryPositions():
    redis_conn.xadd(streamKey,
        {
            "QueryPositions": json.dumps({})
        }
    )

def enterOrder():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "600000",
                    "szExchange" : "SH",
                    "nTradeSideType" : 'B',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 6.86,
                    "nOrderVol" : 100000
                }
            )
        }
    )

#QueryAsset()
QueryPositions()
#enterOrder()