#include "ti_zt_quote_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"

TiZtQuoteClient::TiZtQuoteClient(std::string configPath, TiQuoteCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_api = QuoteApi::CreateQuoteApi(1, "./", XTP_LOG_LEVEL_DEBUG);
	m_api->SetHeartBeatInterval(15);
	m_api->RegisterSpi(this);

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 0;
};


TiZtQuoteClient::~TiZtQuoteClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiZtQuoteClient::OnDisconnected(int reason) 
{
    printf("[OnDisconnected] %d\n", reason);
};
void TiZtQuoteClient::OnError(XTPRI *error_info) {};
void TiZtQuoteClient::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) 
{
    //printf("[OnSubMarketData] %d, %s, %d, %s\n", ticker->exchange_id, ticker->ticker, error_info->error_id, error_info->error_msg);
};
void TiZtQuoteClient::OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) {};
void TiZtQuoteClient::OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) 
{
    TiQuoteSnapshotStockField   _snapStockCash;
    memset(&_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));
    strcpy(_snapStockCash.symbol, market_data->ticker);
    _snapStockCash.date    = market_data->data_time / 1000000000;
    _snapStockCash.time    = market_data->data_time % 1000000000;
    if(market_data->exchange_id == XTP_EXCHANGE_SH){
        strcpy(_snapStockCash.exchange, "SH");
    }
    if(market_data->exchange_id  == XTP_EXCHANGE_SZ){
        strcpy(_snapStockCash.exchange, "SZ");
    }
    _snapStockCash.timestamp   = datetime::get_timestamp_ms(market_data->data_time);
    datetime::get_format_time_ms(_snapStockCash.date, _snapStockCash.time, _snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    _snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    _snapStockCash.last             = market_data->last_price;
    _snapStockCash.pre_close        = market_data->pre_close_price;
    _snapStockCash.open             = market_data->open_price;
    _snapStockCash.high             = market_data->high_price;
    _snapStockCash.low              = market_data->low_price;
    _snapStockCash.high_limit       = market_data->upper_limit_price;
    _snapStockCash.low_limit        = market_data->lower_limit_price;
    _snapStockCash.volume           = 0;
    _snapStockCash.turnover         = 0;
    _snapStockCash.acc_volume       = market_data->qty;
    _snapStockCash.acc_turnover     = market_data->turnover;
    _snapStockCash.match_items      = market_data->trades_count;
    _snapStockCash.total_ask_qty    = market_data->stk.total_ask_qty;
    _snapStockCash.total_bid_qty    = market_data->stk.total_bid_qty;
    _snapStockCash.wavg_ask         = market_data->stk.ma_ask_price;
    _snapStockCash.wavg_bid         = market_data->stk.ma_bid_price;
    _snapStockCash.interest         = 0;
    
    for (size_t i = 0; i < 10; i++)
    {
        // ask
        _snapStockCash.ask_price[i]    = market_data->ask[i];
        _snapStockCash.ask_volume[i]   = market_data->ask_qty[i];
        // bid
        _snapStockCash.bid_price[i]    = market_data->bid[i];
        _snapStockCash.bid_volume[i]   = market_data->bid_qty[i];
    }
    
    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&_snapStockCash);
    }
};
void TiZtQuoteClient::OnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last) 
{
    //printf("[OnSubOrderBook] %d, %s, %d, %s\n", ticker->exchange_id, ticker->ticker, error_info->error_id, error_info->error_msg);
};
void TiZtQuoteClient::OnUnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last) {};
void TiZtQuoteClient::OnOrderBook(XTPOB *order_book) 
{
    long long _timestamp = datetime::get_timestamp_ms(order_book->data_time);
    _timestamp += 892;
    char _timestr[48];
    datetime::get_format_timestamp_ms(_timestamp, _timestr, 48);
    printf("[OnOrderBook] %s, %f, %ld, %lld, %s\n", order_book->ticker, order_book->last_price, order_book->data_time, _timestamp, _timestr);
};
void TiZtQuoteClient::OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last)
{
    //printf("[OnSubTickByTick] %d, %s, %d, %s\n", ticker->exchange_id, ticker->ticker, error_info->error_id, error_info->error_msg);
};
void TiZtQuoteClient::OnUnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last) {};
void TiZtQuoteClient::OnTickByTick(XTPTBT *tbt_data) 
{
    //逐笔成交
    if (tbt_data->type == XTP_TBT_TRADE)
    {
        TiQuoteMatchesField         _matchCash;
        memset(&_matchCash, 0, sizeof(TiQuoteMatchesField));
        strcpy(_matchCash.symbol, tbt_data->ticker);
        _matchCash.date    = tbt_data->data_time / 1000000000;
        _matchCash.time    = tbt_data->data_time % 1000000000;
        if(tbt_data->exchange_id == XTP_EXCHANGE_SH){
            strcpy(_matchCash.exchange, "SH");
        }
        if(tbt_data->exchange_id  == XTP_EXCHANGE_SZ){
            strcpy(_matchCash.exchange, "SZ");
        }
        _matchCash.timestamp   = datetime::get_timestamp_ms(tbt_data->data_time);
        datetime::get_format_time_ms(_matchCash.date, _matchCash.time, _matchCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(_matchCash.recv_time_str, TI_TIME_STR_LEN);
        _matchCash.recv_timestamp  = datetime::get_now_timestamp_ms();
        

        _matchCash.channel         = tbt_data->trade.channel_no;
        _matchCash.seq             = tbt_data->trade.seq;
        _matchCash.price           = tbt_data->trade.price;
        _matchCash.volume          = tbt_data->trade.qty;
        _matchCash.ask_order_seq   = tbt_data->trade.ask_no;
        _matchCash.bid_order_seq   = tbt_data->trade.bid_no;
        
        if(_matchCash.bid_order_seq > _matchCash.ask_order_seq){
            _matchCash.bs_flag     = 'B';
        }else{
            _matchCash.bs_flag     = 'S';
        }
        
        if(tbt_data->trade.trade_flag == '4'){
            _matchCash.function_code   = 'C';
        }else{
            _matchCash.function_code   = '0';
        }

        if(m_cb){
            m_cb->OnL2StockMatchesRtn(&_matchCash);
        }
        return;
    }
    
    //逐笔委托
    if (tbt_data->type == XTP_TBT_ENTRUST)
    {
        TiQuoteOrderField           _orderCash;
        memset(&_orderCash, 0, sizeof(TiQuoteOrderField));
        strcpy(_orderCash.symbol, tbt_data->ticker);
        _orderCash.date    = tbt_data->data_time / 1000000000;
        _orderCash.time    = tbt_data->data_time % 1000000000;
        if(tbt_data->exchange_id == XTP_EXCHANGE_SH){
            strcpy(_orderCash.exchange, "SH");
        }
        if(tbt_data->exchange_id  == XTP_EXCHANGE_SZ){
            strcpy(_orderCash.exchange, "SZ");
        }
        _orderCash.timestamp   = datetime::get_timestamp_ms(tbt_data->data_time);
        datetime::get_format_time_ms(_orderCash.date, _orderCash.time, _orderCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(_orderCash.recv_time_str, TI_TIME_STR_LEN);
        _orderCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        _orderCash.channel         = tbt_data->entrust.channel_no;
        _orderCash.seq             = tbt_data->entrust.seq;
        _orderCash.price           = tbt_data->entrust.price;
        _orderCash.volume          = tbt_data->entrust.qty;
        _orderCash.function_code   = tbt_data->entrust.side;
        _orderCash.order_type      = tbt_data->entrust.ord_type;
        _orderCash.order_orino     = tbt_data->entrust.order_no;
        
        if(_orderCash.function_code == '1'){
           _orderCash.function_code = 'B'; 
        }
        if(_orderCash.function_code == '2'){
           _orderCash.function_code = 'S'; 
        }

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&_orderCash);
        }
        return;
    }
};


////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiZtQuoteClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szIp                  = string(_iniFile["ti_zt_quote_client"]["ip"]);
    m_config->nPort                 = int(_iniFile["ti_zt_quote_client"]["port"]);
    m_config->szLocalIp             = string(_iniFile["ti_zt_quote_client"]["local_ip"]);
    m_config->bEnableUDP            = bool(_iniFile["ti_zt_quote_client"]["enable_udp"]);
    LOG(INFO)<< "m_config->bEnableUDP:" << m_config->bEnableUDP;

    m_config->szUser                = string(_iniFile["ti_zt_quote_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_zt_quote_client"]["pass"]);

    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szLocalIp.empty() |
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

void TiZtQuoteClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    m_api->SetHeartBeatInterval(15);

    int ret = 0;
    if(m_config->bEnableUDP){
        m_api->SetUDPBufferSize(512);
        m_api->SetUDPSeqLogOutPutFlag(true);
        ret = m_api->Login(m_config->szIp.c_str(), m_config->nPort, 
            m_config->szUser.c_str(), m_config->szPass.c_str(), 
            XTP_PROTOCOL_UDP, m_config->szLocalIp.c_str());
    }else{
        ret = m_api->Login(m_config->szIp.c_str(), m_config->nPort, 
            m_config->szUser.c_str(), m_config->szPass.c_str(), 
            XTP_PROTOCOL_TCP, m_config->szLocalIp.c_str());
    }
    
	if (0 != ret)
	{
		// 登录失败，获取错误信息
		XTPRI* error_info = m_api->GetApiLastError();
		std::cout << "login to server error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
	}
    
    std::cout << "login successed" << std::endl;

};


void TiZtQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len){
    XTP_EXCHANGE_TYPE _ex = XTP_EXCHANGE_UNKNOWN;
    if (exchangeName)
    {
        if(!strcmp(exchangeName, "SH")){
            _ex = XTP_EXCHANGE_SH;
        }
        if(!strcmp(exchangeName, "SZ")){
            _ex = XTP_EXCHANGE_SZ;
        }
    }
    int ret = 0;
    if (len)
    {
        ret = m_api->SubscribeMarketData(codeList, len, _ex);
        ret = m_api->SubscribeTickByTick(codeList, len, _ex);
    }else{
        if (!exchangeName)
        {
            ret = m_api->SubscribeAllMarketData();
            ret = m_api->SubscribeAllTickByTick();
        }else{
            ret = m_api->SubscribeAllMarketData(_ex);
            ret = m_api->SubscribeAllTickByTick(_ex);
        }
    }
    /*
    if(m_config->bEnableUDP){

        ret = m_api->SubscribeAllMarketData(_ex);
        ret = m_api->SubscribeAllTickByTick(_ex);
    }else{
        ret = m_api->SubscribeMarketData(codeList, len, _ex);
        ret = m_api->SubscribeTickByTick(codeList, len, _ex);
    }
    */
    if (ret != 0)
    {
        printf("SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
    }
};