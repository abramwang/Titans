const redis = require("./redis_m");
const {config} = require("./config_m");
//const quote = require("./quote_m");
//const identity = require("./identity_m");

export default {
  install(Vue)  {
    Vue.prototype.redis_con = null,
    Vue.prototype.getRedisModel = function() {
      if(!this.redis_con){
        this.redis_con = new redis.default;
        let redis_conf = config.redis;
        this.redis_con.connect(redis_conf.ip, redis_conf.port, redis_conf.auth)
      }
      return this.redis_con;
    },
    Vue.prototype.getIniConfig = function() {
      return config;
    }
  }
}