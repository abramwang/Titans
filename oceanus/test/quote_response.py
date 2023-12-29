import redis
import json

redis_pool = redis.ConnectionPool(
    host='127.0.0.1', port= 20184, password= 'P7pO48Lw4AZTOLXKlR', db= 0)
redis_conn = redis.Redis(connection_pool= redis_pool)

streamKey = "oc_quote_commander.response.lw"
groupName = "g1"
#redis_conn.xgroup_create(streamKey, groupName)

while True:
    msg = redis_conn.xreadgroup(groupName, "c1", {streamKey: '>'}, 1, 0, False)
    for stream_key, entries in msg:
        for entry_id, fields in entries:
            print("Stream Key:", stream_key)
            print("Entry ID:", entry_id)
            print("Fields:", fields)
            redis_conn.xack(streamKey, groupName, entry_id)
    #print(msg)

exit()
# 读取Stream流数据
response = redis_conn.xread({streamKey: '0-0'}, count=10, block=0)

# 处理返回的数据
for stream_key, entries in response:
    for entry_id, fields in entries:
        print("Stream Key:", stream_key)
        print("Entry ID:", entry_id)
        print("Fields:", fields)

exit()