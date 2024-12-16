#!/usr/bin/env python
# coding=utf-8

__version__ = '0.1.0'
__all__ = [
    'Init', 'SubQuote', 'ReadLoop'
]
__author__ = 'bo wang <bo.wang@sci-inv.cn>'

from .callback import CallBack

__user_data__ = None

"""
__user_data__ = {
    "redis_conn": obj,
    "user": "lw",
    "response_key": "oc_quote_commander",
    "sub_info" : {
        "SH.snapshot": [],
        "SH.orders": [],
        "SH.matches": [],
        "SZ.snapshot": [],
        "SZ.orders": [],
        "SZ.matches": [],
        "BJ.snapshot": [],
        "BJ.orders": [],
        "BJ.matches": [],
    }
}
"""

def Init(host_:str, port_:int, user_:str, pwd_:str):
    global __user_data__
    if not __user_data__:
        import redis
        redis_pool = redis.ConnectionPool(
            host = host_, 
            port = port_, 
            password= pwd_, db= 0)
        
        redis_conn = redis.Redis(connection_pool= redis_pool)

        __user_data__ = {
            'redis_conn': redis_conn,
            'user': user_,
            'response_key': f"oc_ipc_quote_json_server.response",
            "response_group": "g1",
            "sub_info" : {
                "snapshot": {
                    "SH": set(),
                    "SZ": set(),
                    "BJ": set(),
                    "CF": set(),
                },
            }
        }
        try:
            redis_conn.xgroup_create(__user_data__["response_key"], __user_data__["response_group"])
        except Exception as e:
            print(e)
            pass
        return 0
    return -1

def SubQuote(dataType:str, exchange:str, symbol_list:list):
    global __user_data__
    if not __user_data__:
        return -1
    import json
    if dataType == "snapshot":
        for symbol in symbol_list:
            __user_data__["sub_info"]["snapshot"][exchange].add(symbol)
    return 0

def ReadLoop(cb_:CallBack):
    global __user_data__
    if not __user_data__:
        return -1
    if not cb_:
        return -1
    import json
    while True:
        msg = __user_data__["redis_conn"].xreadgroup(
            __user_data__["response_group"], 
            "c1", 
            {__user_data__["response_key"]: '>'}, 
            1, 0, False)
        for stream_key, entries in msg:
            for entry_id, fields in entries:
                data_array = json.loads(fields[b'msg'].decode())
                for data in data_array:
                    if data["type"] == "snapshot":
                        if data["symbol"] in __user_data__["sub_info"]["snapshot"][data["exchange"]] \
                            or "*" in __user_data__["sub_info"]["snapshot"][data["exchange"]]:
                            cb_.OnSnapshotRtn(data)
                    #cb_(data["type"], data)
                __user_data__["redis_conn"].xack(
                    __user_data__["response_key"], 
                    __user_data__["response_group"], 
                    entry_id)

