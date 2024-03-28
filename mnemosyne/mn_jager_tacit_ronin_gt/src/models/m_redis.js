const Redis = require("ioredis");

class Client {
    constructor() {
        this.m_con = null;
        this.m_xreadgroup_tasks = {};
        this.m_xread_tasks = {};
        this.m_xread_padding_tasks = [];

        this.m_xread_cb_fun = (err, data) => {
            console.log(err, data);
            if(!data){
                let _task_info = this.m_xread_padding_tasks.shift();
                this.xread(
                    _task_info.streamKey,
                    _task_info.id,
                    _task_info.block,
                    _task_info.cbFun
                );
                return;
            }
            
            let _data_array = [];
            this.parseArrayDataFun(data, _data_array);
            //移除等待队列中的任务
            for (let index = 0; index < this.m_xread_padding_tasks.length; index++) {
                const read_task = this.m_xread_padding_tasks[index];
                if (read_task.streamKey == _data_array[0]){
                    this.m_xread_padding_tasks.splice(index, 1);
                    break;
                }
            }
    
            let _task_info = this.m_xread_tasks[_data_array[0]]
            if(_task_info.cbFun instanceof Function){
                _task_info.id = _data_array[1]
                _task_info.cbFun(_data_array[0], _data_array[1], _data_array[2], JSON.parse(_data_array[3]));
            }
    
            this.xread(
                _task_info.streamKey,
                _task_info.id,
                _task_info.block,
                _task_info.cbFun
            );
        };

        this.m_xreadgroup_cb_fun = (err, data) => {
            if(!(data instanceof Array)){
                return;
            }
            let _data_array = [];
            this.parseArrayDataFun(data, _data_array);
            console.log(err);
            console.log(data);
            console.log(_data_array);
            
            if (_data_array.length < 4){
                console.log("xreadgroupCbFun: err", _data_array.length);
                return;
            }
    
            let _task_info = this.m_xreadgroup_tasks[_data_array[0]]
            if(_task_info.cbFun instanceof Function){
                _task_info.cbFun(_data_array[0], _data_array[1], _data_array[2], JSON.parse(_data_array[3]));
            }
    
            this.xack(_task_info.group,_task_info.streamKey, _data_array[1],
                (err, data) => {
                    if(!err){
                        this.m_con.xreadgroup(
                            'GROUP', _task_info.group, _task_info.consumer, 
                            'COUNT', 1, 'BLOCK', _task_info.block, 'STREAMS', 
                            _task_info.streamKey, _task_info.id,
                            this.m_xreadgroup_cb_fun);
                    }
            });
        }
    }

    //////////////////////////////////////////////////////////////////
    // 工具方法
    //////////////////////////////////////////////////////////////////
    parseResultArrayToDict(srcArray){
        let result = {}
        for (let index = 0; index < srcArray.length/2; index++) {
            const element = srcArray[index];
            result[srcArray[index*2].replaceAll('-','_')] = srcArray[index*2+1]
        }
        return result;
    }

    parseArrayDataFun(srcArray, disArray){
        for (const iter of srcArray) {
            if(iter instanceof Array){
                this.parseArrayDataFun(iter, disArray)
            }else{
                disArray.push(iter)
            }
        }
    }

    //////////////////////////////////////////////////////////////////
    // 导出方法
    //////////////////////////////////////////////////////////////////
    connect(ip, port, auth = null){
        this.m_con =  new Redis({
            host: ip,           // Redis host
            port: port,          // Redis port
            password: auth,
        })
    }

    xack(group, streamKey, id, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.xack(streamKey, group, id, cbFun);
    }

    xinfoGroups(streamKey, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.xinfo('GROUPS', streamKey, (err, data) =>{
            let _resultArray = []
            if(data instanceof Array){
                for (const iterator of data) {
                    _resultArray.push(this.parseResultArrayToDict(iterator));
                }
            }
            cbFun(err, _resultArray);
        })
    }

    xcreatgroup(group, streamKey){
        if(!this.m_con){
            return 0;
        }
        this.m_con.xgroup("CREATE", 
            streamKey,
            group,
            "$",
            "MKSTREAM", (message) => {
                console.log("xgroup CREATE", message);
        });
    }

    xreadgroup(group, consumer, streamKey, block, id, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_xreadgroup_tasks[streamKey] = {
            group       : group,
            consumer    : consumer,
            streamKey   : streamKey,
            block       : block,
            id          : id,
            cbFun       : cbFun
        }
        this.m_con.xreadgroup('GROUP', group, consumer, 
            'COUNT', 1, 'BLOCK', block, 'STREAMS', 
            streamKey, id,
            this.m_xreadgroup_cb_fun);
        return 1;
    }

    xrevrange(streamKey, start, end, count, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.xrevrange(streamKey, start, end, 
            'COUNT', count,
            (err, data) => {
            if(!data){
                return;
            }
            let _data_array = [];
            this.parseArrayDataFun(data, _data_array);
            if(_data_array.length == 3){
                cbFun(_data_array[0], _data_array[1], JSON.parse(_data_array[2]));
            }else{
                cbFun(null, null, null);
            }
        })
        return 1;
    }

    xrevrange_all(streamKey, start, end, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.xrevrange(streamKey, start, end,
            (err, data) => {
            if(!data){
                return;
            }
            for (const iterator of data) {
                let _data_array = [];
                this.parseArrayDataFun(iterator, _data_array);
                if(_data_array.length == 3){
                    cbFun(_data_array[0], _data_array[1], JSON.parse(_data_array[2]));
                }else{
                    cbFun(null, null, null);
                }
            }
        })
        return 1;
    }

    xread(streamKey, block, id, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.xread(
            "BLOCK", block, 
            'COUNT', 1,
            "STREAMS", streamKey, 
            id, 
            (err, data) => {
                if(!data){
                    return;
                }
                let _data_array = [];
                this.parseArrayDataFun(data, _data_array);
                cbFun(_data_array[0], _data_array[1], _data_array[2], JSON.parse(_data_array[3]));
            }
        )
    }

    hgetall(key, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.hgetall(key, cbFun);
    }

    get(key, cbFun){
        if(!this.m_con){
            return 0;
        }
        this.m_con.get(key, cbFun)
    }
}

export default Client;
