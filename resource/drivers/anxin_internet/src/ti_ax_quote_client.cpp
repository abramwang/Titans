#include "ti_ax_quote_client.h"
#include <glog/logging.h>
#include <cstring>
#include "iniFile.h"
#include "datetime.h"

TiAxQuoteClient::TiAxQuoteClient(std::string configPath, TiQuoteCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;

    m_cb = userCb;

    loadConfig(configPath);
};


TiAxQuoteClient::~TiAxQuoteClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiAxQuoteClient::OnLog(const int32_t& level, const char* log, uint32_t len)
{
    std::cout << "AMA log: " << "    level: " << level << "    log:   " << log << std::endl;
}

// 定义监控回调处理方法
void TiAxQuoteClient::OnIndicator(const char* indicator, uint32_t len)
{
    std::cout << "AMA indicator: " << indicator << std::endl;
}

// 定义事件回调处理方法  level 对照 EventLevel 数据类型 code 对照 EventCode 数据类型
void TiAxQuoteClient::OnEvent(uint32_t level, uint32_t code, const char* event_msg, uint32_t len)
{

    std::cout << "AMA event: " << event_msg << std::endl;

    switch (code)
    {
    case amd::ama::EventCode::kChannelTCPLogonSuccess:
        std::cout << "AMA login success" << std::endl;
        onLogin();
        break;
    default:
        break;
    }
}

// 定义快照数据回调处理方法
void TiAxQuoteClient::OnMDSnapshot(amd::ama::MDSnapshot* snapshot, uint32_t cnt)
{
    TiQuoteSnapshotStockField   _snapStockCash;

    for (uint32_t i = 0; i < cnt; ++i)
    {

        memset(&_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));
        strcpy(_snapStockCash.symbol, snapshot[i].security_code);
        _snapStockCash.date    = snapshot[i].orig_time / 1000000000;
        _snapStockCash.time    = snapshot[i].orig_time % 1000000000;
        if(snapshot[i].market_type  == amd::ama::MarketType::kSSE){
            strcpy(_snapStockCash.exchange, "SH");
        }
        if(snapshot[i].market_type  == amd::ama::MarketType::kSZSE){
            strcpy(_snapStockCash.exchange, "SZ");
        }
        _snapStockCash.timestamp   = datetime::get_timestamp_ms(snapshot[i].orig_time);
        datetime::get_format_time_ms(_snapStockCash.date, _snapStockCash.time, _snapStockCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
        _snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        _snapStockCash.last             = ((double)snapshot[i].last_price)/1000000;
        _snapStockCash.pre_close        = ((double)snapshot[i].pre_close_price)/1000000;
        _snapStockCash.open             = ((double)snapshot[i].open_price)/1000000;
        _snapStockCash.high             = ((double)snapshot[i].high_price)/1000000;
        _snapStockCash.low              = ((double)snapshot[i].low_price)/1000000;
        _snapStockCash.high_limit       = ((double)snapshot[i].high_limited)/1000000;
        _snapStockCash.low_limit        = ((double)snapshot[i].low_limited)/1000000;
        _snapStockCash.volume           = 0;
        _snapStockCash.turnover         = 0;
        _snapStockCash.acc_volume       = snapshot[i].total_volume_trade / 100;
        _snapStockCash.acc_turnover     = snapshot[i].total_value_trade / 100000;
        _snapStockCash.match_items      = snapshot[i].num_trades;
        _snapStockCash.total_ask_qty    = snapshot[i].total_offer_volume / 100;
        _snapStockCash.total_bid_qty    = snapshot[i].total_bid_volume / 100;
        _snapStockCash.wavg_ask         = ((double)snapshot[i].weighted_avg_offer_price)/1000000;
        _snapStockCash.wavg_bid         = ((double)snapshot[i].weighted_avg_bid_price)/1000000;
        _snapStockCash.interest         = 0;
        
        for (size_t j = 0; j < 10; j++)
        {
            // ask
            _snapStockCash.ask_price[j]    = ((double)snapshot[i].offer_price[j])/1000000;
            _snapStockCash.ask_volume[j]   = snapshot[i].offer_volume[j] / 100;
            // bid
            _snapStockCash.bid_price[j]    = ((double)snapshot[i].bid_price[j])/1000000;
            _snapStockCash.bid_volume[j]   = snapshot[i].bid_volume[j] / 100;
        }
        
        if(m_cb){
            m_cb->OnL2StockSnapshotRtn(&_snapStockCash);
        }

        //std::cout << amd::ama::Tools::Serialize(snapshot[i]) << std::endl;
    }
    
    /* 手动释放数据内存, 否则会造成内存泄露 */
    amd::ama::IAMDApi::FreeMemory(snapshot);
}

//定义指数快照数据回调处理方法
void TiAxQuoteClient::OnMDIndexSnapshot(amd::ama::MDIndexSnapshot* snapshot, uint32_t cnt)
{
    TiQuoteSnapshotIndexField _snapIndexCash;
    for (uint32_t i = 0; i < cnt; ++i)
    {

        memset(&_snapIndexCash, 0, sizeof(TiQuoteSnapshotIndexField));
        strcpy(_snapIndexCash.symbol, snapshot[i].security_code);
        _snapIndexCash.date    = snapshot[i].orig_time / 1000000000;
        _snapIndexCash.time    = snapshot[i].orig_time % 1000000000;
        if(snapshot[i].market_type  == amd::ama::MarketType::kSSE){
            strcpy(_snapIndexCash.exchange, "SH");
        }
        if(snapshot[i].market_type  == amd::ama::MarketType::kSZSE){
            strcpy(_snapIndexCash.exchange, "SZ");
        }
        _snapIndexCash.timestamp   = datetime::get_timestamp_ms(snapshot[i].orig_time);
        datetime::get_format_time_ms(_snapIndexCash.date, _snapIndexCash.time, _snapIndexCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(_snapIndexCash.recv_time_str, TI_TIME_STR_LEN);
        _snapIndexCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        _snapIndexCash.last             = ((double)snapshot[i].last_index)/1000000;
        _snapIndexCash.pre_close        = ((double)snapshot[i].pre_close_index)/1000000;
        _snapIndexCash.open             = ((double)snapshot[i].open_index)/1000000;
        _snapIndexCash.high             = ((double)snapshot[i].high_index)/1000000;
        _snapIndexCash.low              = ((double)snapshot[i].low_index)/1000000;
        _snapIndexCash.close            = ((double)snapshot[i].close_index)/1000000;
        _snapIndexCash.volume           = 0;
        _snapIndexCash.turnover         = 0;
        _snapIndexCash.volume           = snapshot[i].total_volume_trade / 100;
        _snapIndexCash.turnover         = snapshot[i].total_value_trade / 1000000;
        
        if(m_cb){
            m_cb->OnL2IndexSnapshotRtn(&_snapIndexCash);
        }
        
        //std::cout << amd::ama::Tools::Serialize(snapshots[i]) << std::endl;
    }
    
    /* 手动释放数据内存, 否则会造成内存泄露 */
    amd::ama::IAMDApi::FreeMemory(snapshot);
}

//定义逐笔委托数据回调处理方法
void TiAxQuoteClient::OnMDTickOrder(amd::ama::MDTickOrder* ticks, uint32_t cnt)
{
    TiQuoteOrderField           _orderCash;
    for (uint32_t i = 0; i < cnt; ++i)
    {
        memset(&_orderCash, 0, sizeof(TiQuoteOrderField));
        strcpy(_orderCash.symbol, ticks[i].security_code);
        _orderCash.date    = ticks[i].order_time / 1000000000;
        _orderCash.time    = ticks[i].order_time % 1000000000;
        if(ticks[i].market_type == amd::ama::MarketType::kSSE){
            strcpy(_orderCash.exchange, "SH");
        }
        if(ticks[i].market_type == amd::ama::MarketType::kSZSE){
            strcpy(_orderCash.exchange, "SZ");
        }
        _orderCash.timestamp   = datetime::get_timestamp_ms(ticks[i].order_time);
        datetime::get_format_time_ms(_orderCash.date, _orderCash.time, _orderCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(_orderCash.recv_time_str, TI_TIME_STR_LEN);
        _orderCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        _orderCash.channel         = ticks[i].channel_no;
        _orderCash.seq             = ticks[i].biz_index;
        _orderCash.price           = ((double)ticks[i].order_price) / 1000000;
        _orderCash.volume          = ticks[i].order_volume / 100;
        _orderCash.function_code   = ticks[i].side;
        _orderCash.order_type      = ticks[i].order_type;
        _orderCash.order_orino     = ticks[i].orig_order_no;
        
        if(_orderCash.function_code == '1'){
           _orderCash.function_code = 'B'; 
        }
        if(_orderCash.function_code == '2'){
           _orderCash.function_code = 'S'; 
        }

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&_orderCash);
        }


        //std::cout << amd::ama::Tools::Serialize(ticks[i]) << std::endl;
    }
    /* 手动释放数据内存, 否则会造成内存泄露 */
    amd::ama::IAMDApi::FreeMemory(ticks);
}

// 定义逐笔成交数据回调处理方法
void TiAxQuoteClient::OnMDTickExecution(amd::ama::MDTickExecution* tick, uint32_t cnt)
{
    TiQuoteMatchesField         _matchCash;
    for (uint32_t i = 0; i < cnt; ++i)
    {
        memset(&_matchCash, 0, sizeof(TiQuoteMatchesField));
        strcpy(_matchCash.symbol, tick[i].security_code);
        _matchCash.date    = tick[i].exec_time / 1000000000;
        _matchCash.time    = tick[i].exec_time % 1000000000;
        if(tick[i].market_type == amd::ama::MarketType::kSSE){
            strcpy(_matchCash.exchange, "SH");
        }
        if(tick[i].market_type == amd::ama::MarketType::kSZSE){
            strcpy(_matchCash.exchange, "SZ");
        }
        _matchCash.timestamp   = datetime::get_timestamp_ms(tick[i].exec_time);
        datetime::get_format_time_ms(_matchCash.date, _matchCash.time, _matchCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(_matchCash.recv_time_str, TI_TIME_STR_LEN);
        _matchCash.recv_timestamp  = datetime::get_now_timestamp_ms();
        

        _matchCash.channel         = tick[i].channel_no;
        _matchCash.seq             = tick[i].biz_index;
        _matchCash.price           = ((double)tick[i].exec_price) / 1000000;
        _matchCash.volume          = tick[i].exec_volume;
        _matchCash.ask_order_seq   = tick[i].offer_appl_seq_num;
        _matchCash.bid_order_seq   = tick[i].bid_appl_seq_num;
        
        if(_matchCash.bid_order_seq > _matchCash.ask_order_seq){
            _matchCash.bs_flag     = 'B';
        }else{
            _matchCash.bs_flag     = 'S';
        }
        
        if(tick[i].exec_type == '4'){
            _matchCash.function_code   = 'C';
        }else{
            _matchCash.function_code   = '0';
        }

        if(m_cb){
            m_cb->OnL2StockMatchesRtn(&_matchCash);
        }

        //std::cout << amd::ama::Tools::Serialize(tick[i]) << std::endl;
    }

    /* 手动释放数据内存, 否则会造成内存泄露 */
    amd::ama::IAMDApi::FreeMemory(tick);
}

//定义委托簿数据回调处理方法(本地构建模式下非单线程递交, cfg.thread_num为递交委托簿数据线程数, 服务端推送模式下为单线程递交)
void TiAxQuoteClient::OnMDOrderBook(std::vector<amd::ama::MDOrderBook>& order_book)
{
    for(uint32_t i = 0; i< order_book.size(); i++)
    {
        std::cout << amd::ama::Tools::Serialize(order_book[i]) << std::endl;
    }
    //不需要手动释放内存
}

    
////////////////////////////////////////////////////////////////////////
// 私有回调方法
////////////////////////////////////////////////////////////////////////
void TiAxQuoteClient::onLogin()
{
    if (m_cb)
    {
        m_cb->OnEventRtn('L');
        /* code */
    }
    char* shCodeList[3] = {"600000", "600004", "600008"};
    subData(NULL, shCodeList, 0);
};

////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiAxQuoteClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szIp                  = string(_iniFile["ti_ax_quote_client"]["ip"]);
    m_config->nPort                 = int(_iniFile["ti_ax_quote_client"]["port"]);
    m_config->szLocalIp             = string(_iniFile["ti_ax_quote_client"]["local_ip"]);
    m_config->szModel               = string(_iniFile["ti_ax_quote_client"]["model"]);
    
    LOG(INFO)<< "m_config->szModel:" << m_config->szModel;

    m_config->szUser                = string(_iniFile["ti_ax_quote_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_ax_quote_client"]["pass"]);

    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szLocalIp.empty() |
        m_config->szModel.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void TiAxQuoteClient::init_ama_config()
{
    /*
        通道模式设置及各个通道说明:
        cfg.channel_mode = amd::ama::ChannelMode::kTCP;   ///< TCP 方式计入上游行情系统
        cfg.channel_mode = amd::ama::ChannelMode::kAMI;   ///< AMI 组播方式接入上游行情系统
        cfg.channel_mode = amd::ama::ChannelMode::kRDMA;  ///< 开启硬件加速RDMA通道,抓取网卡数据包数据
        cfg.channel_mode = amd::ama::ChannelMode::kEXA;   ///< 开启硬件加速EXA通道,抓取网卡数据包数据
        cfg.channel_mode = amd::ama::ChannelMode::kPCAP;  ///< 开启硬件加速PCAP通道,抓取网卡数据包数据
        cfg.channel_mode = amd::ama::ChannelMode::kMDDP;  ///< 直接接入交易所网关组播数据，现在只有深圳交易所开通了此服务
        cfg.channel_mode = amd::ama::ChannelMode::kFPGA;  ///< FPGA接入数据
        cfg.channel_mode = amd::ama::ChannelMode::kTCP|amd::ama::ChannelMode::kAMI;  ///< 同时通过TCP方式和AMI组播方式接入上游，通过cfg.ha_mode 设置对应的高可用设置模式
    */
    m_cfg.channel_mode = amd::ama::ChannelMode::kTCP;

    if (m_config->szModel == "AMI")
    {
        m_cfg.channel_mode = amd::ama::ChannelMode::kAMI;
    }

    m_cfg.tcp_compress_mode = 1;  //TCP传输数据方式: 0 不压缩 1 华锐自定义压缩 2 zstd压缩(仅TCP模式有效)
    
    
    /*
        通道高可用模式设置
        1. cfg.channel_mode 为单通道时,建议设置值为kMasterSlaveA / kMasterSlaveB
        2. cfg.channel_mode 混合开启多个通道时,根据需求设置不同的值
            1) 如果需要多个通道为多活模式接入,请设置kRegularDataFilter值
            2) 如果需要多个通道互为主备接入，请设置值为kMasterSlaveA / kMasterSlaveB,kMasterSlaveA / kMasterSlaveB 差别请参看注释说明
               通道优先级从高到低依次为 kRDMA/kEXA/kMDDP/kAMI/kTCP/kPCAP
    */
    m_cfg.ha_mode = amd::ama::HighAvailableMode::kMasterSlaveA;


    
    m_cfg.min_log_level = amd::ama::LogLevel::kInfo; // 设置日志最小级别：Info级（高于等于此级别的日志就可输出了）, AMA内部日志通过 OnLog 回调函数返回
    
    /*
        设置是否输出监控数据: true(是), false(否), 监控数据通过OnIndicator 回调函数返回
        监控数据格式为json, 主要监控数据包括订阅信息，数据接收数量统计
        数据量统计:包括接收数量和成功下发的数量统计,两者差值为过滤的数据量统计
        eg: "RecvSnapshot": "5926", "SuccessSnapshot": "5925",表示接收了快照数据5926个,成功下发5925个，过滤数据为 5926 - 5925 = 1 个
            过滤的数据有可能为重复数据或者非订阅数据
    */
    m_cfg.is_output_mon_data = false;   
    
    /*
        设置逐笔保序开关: true(开启保序功能) , false(关闭保序功能)
        主要校验逐笔成交数据和逐笔委托数据是否有丢失,如果丢失会有告警日志,缓存逐笔数据并等待keep_order_timeout_ms(单位ms)时间等待上游数据重传,
        如果超过此时间,直接下发缓存数据,默认数据已经丢失,如果之后丢失数据过来也会丢弃。
        同时由于深圳和上海交易所都是通道内序号连续,如果打开了保序开关,必须订阅全部代码的逐笔数据,否则一部分序号会被订阅过滤,导致数据超时等待以及丢失告警。
    */
    m_cfg.keep_order = false;
    m_cfg.keep_order_timeout_ms = 3000;


    m_cfg.is_subscribe_full = false; //设置默认订阅: true 代表默认全部订阅, false 代表默认全部不订阅

    /*
        配置UMS信息:
        username/password 账户名/密码, 一个账户只能保持一个连接接入（注意: 如果需要使用委托簿功能，注意账号需要有委托簿功能权限）
        ums地址配置:
            1) ums地址可以配置1-8个 建议值为2 互为主备, ums_server_cnt 为此次配置UMS地址的个数
            2) ums_servers 为UMS地址信息数据结构:
                local_ip 为本地地址,填0.0.0.0 或者本机ip
                server_ip 为ums服务端地址
                server_port 为ums服务端端口
    */
    strcpy(m_cfg.username, m_config->szUser.c_str());
    strcpy(m_cfg.password, m_config->szPass.c_str());

    m_cfg.ums_server_cnt = 2;
    strcpy(m_cfg.ums_servers[0].local_ip, m_config->szLocalIp.c_str());
    strcpy(m_cfg.ums_servers[0].server_ip, m_config->szIp.c_str());
    m_cfg.ums_servers[0].server_port = m_config->nPort;
    /*
    strcpy(cfg.ums_servers[1].local_ip, "0.0.0.0");
    strcpy(cfg.ums_servers[1].server_ip, "59.36.5.49");
    cfg.ums_servers[1].server_port = 5555;
    */
    
    /*
        业务数据回调接口(不包括 OnIndicator/OnLog等功能数据回调)的线程安全模式设置:
        true: 所有的业务数据接口为接口集线程安全
        false: 业务接口单接口为线程安全,接口集非线程安全 
    */
    m_cfg.is_thread_safe = false;

    /*
        委托簿前档行情参数设置(仅委托簿版本API有效，若非委托簿版本参数设置无效):
            1) 行情输出设置，包括档位数量以及每档对应的委托队列数量
            2）委托簿计算输出参数设置，包括线程数量以及递交间隔设置
    */
   m_cfg.enable_order_book = amd::ama::OrderBookType::kNone; //是否开启委托簿计算功能
   m_cfg.entry_size = 10;                                    //委托簿输出最大档位数量(递交的委托簿数据档位小于等于entry_size)
   m_cfg.order_queue_size = 10;                              //每个档位输出的委托队列揭示(最大设置50)
   m_cfg.thread_num = 3;                                     //构建委托簿的线程数量(递交和构建每一个市场分别开启thread_num个线程)
   m_cfg.order_book_deliver_interval_microsecond = 0;        /*递交的最小时间间隔(单位:微妙, 委托簿递交间隔大于等于order_book_deliver_interval_microsecond)
                                                             设置为0时, 委托簿实时下发, 不会等待间隔.
                                                            */
};



void TiAxQuoteClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    init_ama_config();

    if (amd::ama::IAMDApi::Init(this, m_cfg)
        != amd::ama::ErrorCode::kSuccess)
    {
        std::cout << "Init AMA failed" << std::endl;
        amd::ama::IAMDApi::Release();
    }
};


void TiAxQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len){
    /*
    按品种类型订阅信息设置:
    1. 订阅信息分三个维度 market:市场, data_type:证券数据类型, category_type:品种类型, security_code:证券代码
    2. 订阅操作有三种:
        kSet 设置订阅, 覆盖所有订阅信息
        kAdd 增加订阅, 在前一个基础上增加订阅信息
        kDel 删除订阅, 在前一个基础上删除订阅信息
        kCancelAll 取消所有订阅信息
    */
    amd::ama::SubscribeCategoryItem sub1[2];
    memset(sub1, 0, sizeof(sub1));

    // 订阅深交所全部证券代码的股票逐笔委托和逐笔成交数据 
    sub1[0].market = amd::ama::MarketType::kSZSE;
    sub1[0].data_type = amd::ama::SubscribeSecuDataType::kSnapshot
                | amd::ama::SubscribeSecuDataType::kTickExecution
                | amd::ama::SubscribeSecuDataType::kTickOrder;
    sub1[0].category_type = amd::ama::SubscribeCategoryType::kNone;
    sub1[0].security_code[0] = '\0';

    
    
    // 订阅上交所全部证券代码的基金逐笔委托和逐笔成交数据
    sub1[1].market = amd::ama::MarketType::kSSE;
    sub1[1].data_type = amd::ama::SubscribeSecuDataType::kSnapshot
                | amd::ama::SubscribeSecuDataType::kTickExecution
                | amd::ama::SubscribeSecuDataType::kTickOrder;
    sub1[1].category_type = amd::ama::SubscribeCategoryType::kNone;
    sub1[1].security_code[0] = '\0';
    

    /* 发起订阅 */
    amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kSet, sub1, 2);
    return;
};

bool TiAxQuoteClient::getETFCodeList(json &out)
{
    out = json::array();
    amd::ama::ETFCodeTableRecordList list;

    amd::ama::ETFItem etf[2] ;
    memset(etf, 0, sizeof(etf));
    etf[0].market = amd::ama::MarketType::kSZSE;                   //查询深交所ETF代码表的数据
    strcpy(etf[0].security_code, "\0");                            //查询全部代码ETF代码表的数据

    etf[1].market = amd::ama::MarketType::kSSE;                    //查询上交所ETF代码表的数据
    strcpy(etf[1].security_code, "\0");                            //查询全部代码ETF代码表的数据
    
    bool ret = amd::ama::IAMDApi::GetETFCodeTableList(list, etf, 2);
    if(ret)
    {
        for(uint32_t i=0; i< list.etf_list_nums; i++)
        {
            TI_ExchangeType exchange;
            if(list.etf_records[i].market_type == amd::ama::MarketType::kSSE){
                strcpy(exchange, "SH");
            }
            if(list.etf_records[i].market_type ==  amd::ama::MarketType::kSZSE){
                strcpy(exchange, "SZ");
            }
            json info = {
                { "exchange", exchange },                   ///<交易市场
                { "symbol", list.etf_records[i].security_code },   ///<etf代码,买卖,申赎统一使用该代码
                { "name", list.etf_records[i].symbol },   ///<etf代码,买卖,申赎统一使用该代码
                { "underlying_symbol", list.etf_records[i].underlying_security_id },   ///拟合指数代码(仅深圳有效)
                { "fund_management_company", list.etf_records[i].fund_management_company },     ///基金公司名称
                { "unit", list.etf_records[i].creation_redemption_unit },       ///<最小申购赎回单位对应的ETF份数,例如上证"50ETF"就是900000
                { "subscribe_status", list.etf_records[i].creation },           ///<是否允许申购,1-允许,0-禁止
                { "redemption_status", list.etf_records[i].redemption },        ///<是否允许赎回,1-允许,0-禁止
                { "max_cash_ratio", list.etf_records[i].max_cash_ratio },       ///<最大现金替代比例,小于1的数值   TODO 是否采用double
                { "estimate_amount", list.etf_records[i].estimate_cash_component },     ///<T日预估金额差额
                { "cash_component", list.etf_records[i].cash_component },               ///<T-X日现金差额
                { "net_value", list.etf_records[i].nav_per_cu },                        ///<基金单位净值
                { "nav", list.etf_records[i].nav },                                     ///前一日基金份额净值(类型:价格Price)
                { "total_amount", list.etf_records[i].nav_per_cu * list.etf_records[i].creation_redemption_unit },  ///<最小申赎单位净值总金额=net_value*unit
                { "creation_limit", list.etf_records[i].creation_limit },       //累计申购总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "redemption_limit", list.etf_records[i].redemption_limit },   //累计赎回总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "creation_limit_per_user", list.etf_records[i].creation_limit_per_user },         //单个账户累计申购总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "redemption_limit_per_user", list.etf_records[i].redemption_limit_per_user },     //单个账户累计赎回总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "net_creation_limit", list.etf_records[i].net_creation_limit },                   //净申购总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "net_redemption_limit", list.etf_records[i].net_redemption_limit },               //净赎回总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "net_creation_limit_per_user", list.etf_records[i].net_creation_limit_per_user }, //单个账户净申购总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "net_redemption_limit_per_user", list.etf_records[i].net_redemption_limit_per_user }, //单个账户净赎回总额限制,为 0 表示没有限制(仅深圳有效,类型:数量Qty)
                { "component_list", json::array()}
            };
            for (size_t j = 0; j < list.etf_records[i].constituent_stock_infos.size(); j++)
            {
                TI_ExchangeType c_exchange;
                if(list.etf_records[i].constituent_stock_infos[j].market_type ==  amd::ama::MarketType::kSZSE){
                    strcpy(c_exchange, "SZ");
                }else{
                    strcpy(c_exchange, "SH");
                }
                json c = {
                    {"etf_symbol", list.etf_records[i].security_code },
                    {"exchange", c_exchange },
                    {"symbol", list.etf_records[i].constituent_stock_infos[j].security_code },
                    {"name", list.etf_records[i].constituent_stock_infos[j].underlying_symbol },
                    {"disclosure_vol", list.etf_records[i].constituent_stock_infos[j].component_share },
                    {"replace_type", list.etf_records[i].constituent_stock_infos[j].substitute_flag },
                    //************************************深圳现金替代标志***************************************************************
                    //0=禁止现金替代(必须有证券),1=可以进行现金替代(先用证券,证券不足时差额部分用现金替代),2=必须用现金替代
                    //************************************上海现金替代标志***************************************************************
                    //ETF 公告文件 1.0 版格式
                    //0 –沪市不可被替代, 1 – 沪市可以被替代, 2 – 沪市必须被替代, 3 – 深市退补现金替代, 4 – 深市必须现金替代
                    //5 – 非沪深市场成份证券退补现金替代(不适用于跨沪深港 ETF 产品), 6 – 非沪深市场成份证券必须现金替代(不适用于跨沪深港 ETF 产品)
                    //ETF 公告文件 2.1 版格式
                    //0 –沪市不可被替代, 1 – 沪市可以被替代, 2 – 沪市必须被替代, 3 – 深市退补现金替代, 4 – 深市必须现金替代
                    //5 – 非沪深市场成份证券退补现金替代(不适用于跨沪深港 ETF 产品), 6 – 非沪深市场成份证券必须现金替代(不适用于跨沪深港 ETF 产品)
                    //7 – 港市退补现金替代(仅适用于跨沪深港ETF产品), 8 – 港市必须现金替代(仅适用于跨沪深港ETF产品)
                    {"amount", list.etf_records[i].constituent_stock_infos[j].substitution_cash_amount },               //替代总金额(仅上海有效,类型:金额Amt)
                    {"creation_amount", list.etf_records[i].constituent_stock_infos[j].creation_cash_substitute},       //申购替代金额(仅深圳有效,类型:金额Amt)
                    {"creation_premium_ratio", list.etf_records[i].constituent_stock_infos[j].premium_ratio},           //溢价比例(类型:比例Rate)
                    {"premium_ratio", list.etf_records[i].constituent_stock_infos[j].premium_ratio },                   //溢价比例(类型:比例Rate)
                    {"redemption_amount", list.etf_records[i].constituent_stock_infos[j].redemption_cash_substitute },  //赎回替代金额(仅深圳有效,类型:金额Amt)
                    {"redemption_discount_ratio", list.etf_records[i].constituent_stock_infos[j].discount_ratio },      //折价比例(类型:比例Rate)
                };
                info["component_list"].push_back(c);
            }
            out.push_back(info);
        }
        if(list.etf_list_nums > 0)
            amd::ama::IAMDApi::FreeMemory(list.etf_records);  //释放ETF代码表内存池数据
    }
    return ret;
};

bool TiAxQuoteClient::getCodeList()
{
    amd::ama::CodeTableRecordList list;

    amd::ama::SubCodeTableItem sub[2] ;
    memset(sub, 0, sizeof(sub));
    sub[0].market = amd::ama::MarketType::kSZSE;                   //查询深交所代码表的数据
    strcpy(sub[0].security_code, "\0");                            //查询全部代码代码表的数据

    sub[1].market = amd::ama::MarketType::kSSE;                    //查询上交所代码表的数据
    strcpy(sub[1].security_code, "\0");                            //查询全部代码代码表的数据

    bool ret = amd::ama::IAMDApi::GetCodeTableList(list, sub, 2);
    if(ret)
    {
        for(uint32_t i=0; i< list.list_nums; i++)
        {
            /*
                handle list.records
                records 是代码表数据头指针
            */
            std::cout << amd::ama::Tools::Serialize(list.records[i]) << std::endl;
        }
        if(list.list_nums > 0)
            amd::ama::IAMDApi::FreeMemory(list.records);  //释放代码表内存池数据
    }
    return ret;
};