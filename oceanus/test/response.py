import redis
import json

redis_pool = redis.ConnectionPool(
    host='127.0.0.1', port= 20184, password= 'kymq20180', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)

streamKey = "trade_response.huatai_8080.07420"
groupName = "g1"
#redis_conn.xgroup_create(streamKey, groupName)
while True:
    msg = redis_conn.xreadgroup(groupName, "c1", {streamKey: '>'}, 1, 0, False)
    print(msg)