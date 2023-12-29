#!/usr/bin/env python
# coding=utf-8

__version__ = '0.1.0'
__all__ = [
    'Init', 'Sub', 'SubStop', 'Pub', 'Destroy'
]
__author__ = 'bo wang <bo.wang@sci-inv.cn>'

from types import FunctionType

__user_data__ = None

def Init(host_:str, port_:int, user_:str, pwd_:str = "XrychhqCdkyhhlHhyqbffByqzkyy2020)%!&",
    host2_:str="", port2_:int=0, user2_:str="", pwd2_:str = ""):
    global __user_data__
    if not __user_data__:
        import redis
        rc = redis.Redis(
            host=host_, 
            port=port_, 
            password="XrychhqCdkyhhlHhyqbffByqzkyy2020)%!&",
            socket_keepalive_options=True,
            decode_responses=True)
        ps = rc.pubsub()
        rc2 = None
        if host2_ and port2_ and user2_ and pwd2_:
            rc2 = redis.Redis(
                host=host2_, 
                port=port2_, 
                password=pwd2_,
                socket_keepalive_options=True,
                decode_responses=True)
        else:
            rc2 = rc
            user2_ = user_
        __user_data__ = {
            'rc': rc,
            'ps': ps,
            'rc2': rc2,
            'user': user_,
            'user2': user2_
        }
        return 0
    return -1

def Sub(topic_:str, cb_:FunctionType):
    global __user_data__
    if not __user_data__:
        return -1
    if not cb_:
        return -1
    import json
    ps = __user_data__['ps']
    ps.psubscribe('{}.{}'.format(__user_data__['user'], topic_))
    for item in ps.listen():
        if item['type'] == 'pmessage':
            cb_(item['channel'].split(".")[1], json.loads(item['data']))

def Pub(topic_:str, data_:object):
    global __user_data__
    if not __user_data__:
        return -1
    import json
    rc = __user_data__['rc2']
    rc.publish('{}.{}'.format(__user_data__['user2'], topic_), json.dumps(data_))
