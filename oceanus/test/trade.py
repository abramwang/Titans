import redis
import json

redis_pool = redis.ConnectionPool(
    host='121.36.210.85', port= 20184, password= 'P7pO48Lw4AZTOLXKlR', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)


streamKey = "oc_trader_commander_guangfa.00043346"

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

def QueryOrders():
    redis_conn.xadd(streamKey,
        {
            "QueryOrders": json.dumps({})
        }
    )

def QueryMatches():
    redis_conn.xadd(streamKey,
        {
            "QueryMatches": json.dumps({})
        }
    )

def enterOrder():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "600000",
                    "szExchange" : "SH",
                    "szAccount": "2001324",
                    "nTradeSideType" : 'B',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 7.19,
                    "nOrderVol" : 400
                }
            )
        }
    )

def enterOrder_SZ():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "000001",
                    "szExchange" : "SZ",
                    "szAccount": "2001324",
                    "nTradeSideType" : 'B',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 10.48,
                    "nOrderVol" : 300
                }
            )
        }
    )
def enterOrderGroup_SZ():
    orders = []
    for i in range(100):
        orders.append(
            {
                "szSymbol" : "000001",
                "szExchange" : "SZ",
                "szAccount": "2001324",
                "nTradeSideType" : 'B',
                "nOffsetType" : 'O',
                "nBusinessType" : 'S',
                "nOrderPrice" : 10.48,
                "nOrderVol" : 300
            }
        )
    redis_conn.xadd(streamKey,
        {
            "enterOrders": json.dumps(orders)
        }
    )

def enterOrder_sell():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "600000",
                    "szExchange" : "SH",
                    "szAccount": "2001324",
                    "nTradeSideType" : 'S',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 6.78,
                    "nOrderVol" : 300
                }
            )
        }
    )

def enterOrder_sell_sz():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "000001",
                    "szExchange" : "SZ",
                    "szAccount": "2001324",
                    "nTradeSideType" : 'S',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 9.48,
                    "nOrderVol" : 300
                }
            )
        }
    )

def cancelOrder(order_id):
    redis_conn.xadd(streamKey,
        {
            "cancelOrder": json.dumps(
                {
                    "nOrderId" : order_id
                }
            )
        }
    )


def enterOrder_ETF():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "512820",
                    "szExchange" : "SH",
                    "nTradeSideType" : 'B',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'S',
                    "nOrderPrice" : 1.144,
                    "nOrderVol" : 300000
                }
            )
        }
    )

def enterOrder_ETF_Redemption():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "szSymbol" : "512820",
                    "szExchange" : "SH",
                    "nTradeSideType" : 'R',
                    "nOffsetType" : 'O',
                    "nBusinessType" : 'E',
                    "nOrderPrice" : 1.134,
                    "nOrderVol" : 300000
                }
            )
        }
    )

#enterOrder_ETF()
#enterOrder_ETF_Redemption()
#enterOrder_sell()
#enterOrder_sell_sz()
#QueryOrders()
enterOrder()
#enterOrder_SZ()
#for i in range(100):
#    enterOrder_SZ()
#cancelOrder(1680011)

#enterOrderGroup_SZ()

#QueryPositions()
#exit(0)
#QueryAsset()
#QueryPositions()
#QueryOrders()
#QueryMatches()

