#!/usr/bin/env python
# coding=utf-8

__version__ = '0.1.0'
__all__ = [
    'Init', 'SubQuote', 'ReadLoop'
]
__author__ = 'bo wang <bo.wang@sci-inv.cn>'

from types import FunctionType

__user_data__ = None
__history_root__ = "/home/ti_data/"
"""
__user_data__ = {
    "redis_conn": obj,
    "user": "lw",
    "commander_key": "oc_quote_commander.lw",
    "response_key": "oc_quote_commander",
    "response_group": "g1",
    "sub_info" : {
        "SH.snapshot": [],
        "SH.orders": [],
        "SH.matches": [],
        "SZ.snapshot": [],
        "SZ.orders": [],
        "SZ.matches": []
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
            'commander_key': f"oc_quote_commander.{user_}",
            'response_key': f"oc_quote_commander.response.{user_}",
            "response_group": "g1",
            "sub_info" : {
                "SH.snapshot": [],
                "SH.orders": [],
                "SH.matches": [],
                "SZ.snapshot": [],
                "SZ.orders": [],
                "SZ.matches": []
            }
        }
        try:
            redis_conn.xgroup_create(__user_data__["response_key"], __user_data__["response_group"])
        except Exception as e:
            print(e)
            pass
        return 0
    return -1

def SubQuote(dateType:str, exchange:str, symbol_list:list):
    print(dateType, exchange, symbol_list)
    global __user_data__
    if not __user_data__:
        return -1
    import json
    __user_data__["sub_info"][f"{exchange}.{dateType}"] = symbol_list
    __user_data__["redis_conn"].xadd(__user_data__["commander_key"], {
            "update_sub_info": json.dumps(__user_data__["sub_info"])
        })
    return 0

def ReadLoop(cb_:FunctionType):
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
                    cb_(data["type"], data)
                __user_data__["redis_conn"].xack(
                    __user_data__["response_key"], 
                    __user_data__["response_group"], 
                    entry_id)

