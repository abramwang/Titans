import redis
import json

redis_pool = redis.ConnectionPool(
    host='47.103.74.35', port= 20184, password= 'W_P!ViW+d7xAFept6', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)


streamKey = "ia_etf_xtp_trader"


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

def enterOrder():
    redis_conn.xadd(streamKey,
        {
            "enterOrder": json.dumps(
                {
                    "m_Account" : "07420",
                    "m_Side" : 1,
                    "m_Exchange" : 103,
                    "m_Symbol" : "rb2304",
                    "m_SecType" : 3,
                    "m_Price" : 69690,
                    "m_Qty" : 3,
                    #"m_ClientOrderToken" : 5,
                    "m_Tif" : 99998,
                    "m_MinQty" : 0,
                    "m_HedgeFlag" : 2,
                    "m_OrderType" : 1
                }
            )
        }
    )

#querySymbolList()
#queryAccountOrder()
enterOrder()
#cancelOrder(4194312)
#queryAccountPosition()
#queryAccountBP()