const fs = require('fs')

var config = {
    "redis": {
        "ip"              : "121.36.210.85",
        "port"            : 20184,
        "auth"            : "P7pO48Lw4AZTOLXKlR"
    },
    "user_setting": {
        "factor_stream_key": "etf_factor.GT",
        "signal_cost_stream_key": "etf_factor.GT.cost",
        "trader_account": {
            "order_key": "trading_orders.xtp.253191001815",
            "match_key": "trading_matches.xtp.253191001815",
            "trading_stream_key": "trading_matches.xtp.253191001815"
        }
    }
}

// 检查文件是否存在于当前目录中、以及是否可写。
const file = 'config.json';
fs.access(file, fs.constants.F_OK, (err) => {
    console.log(`${file} ${err ? '不存在' : '存在'}`);
    if (err) {
        fs.writeFileSync(file, JSON.stringify(config.user_setting));
    }else{
        var config_file = fs.readFileSync("config.json", 'utf8').toString();
        if(config_file){
            config.user_setting = JSON.parse(config_file)
        }
    }
});

exports.config = config;