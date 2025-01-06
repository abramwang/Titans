#include "ti_df_quote_l1_client.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "ti_quote_formater.h"
#include "ti_encoding_tool.h"
#include "datetime.h"

TiDfQuoteL1Client::TiDfQuoteL1Client() : m_quote_api(nullptr)
{ 
    m_cb = NULL;
    m_trading_day   = datetime::get_today();
    Init(); 
}

TiDfQuoteL1Client::~TiDfQuoteL1Client() {
    if (m_quote_api) {
        m_quote_api->Logout();
        m_quote_api = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////
// 私有方法
////////////////////////////////////////////////////////////////////////

void TiDfQuoteL1Client::Init() {
    // 初始化行情类Api
    m_quote_api = EMQ::API::QuoteApi::CreateQuoteApi("./logs", EMQ_LOG_LEVEL::EMQ_LOG_LEVEL_DEBUG, EMQ_LOG_LEVEL::EMQ_LOG_LEVEL_DEBUG);
    if (!m_quote_api) {
        std::cout << "CreatQuoteApi Failed!" << std::endl;
        return;
    }

    // 设置接收回报类SPI
    m_quote_api->RegisterSpi(this);

    // 行情登录，请将您的行情账号密码填入
    auto error_code = m_quote_api->Login("61.152.230.216", 8093, "510100025168", "OW4273");
    if (error_code < 0) {
        std::cout << "Quote Login Failed! error code: " << error_code << std::endl << std::endl;
        return;
    }
    std::cout << "Quote Login Success!" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    m_quote_api->QueryAllTickersFullInfo(EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SH);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    //subData("SH", nullptr, 0);
}

void TiDfQuoteL1Client::formatQuoteUpdatetime(unsigned long long quote_update_time, int32_t &date, int32_t &time, int64_t &timestamp)
{
    date = quote_update_time / 1000000000;
    time = quote_update_time % 1000000000;
    timestamp = datetime::get_timestamp_ms(date, time);
};


////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiDfQuoteL1Client::OnQueryAllTickersFullInfo(EMTQuoteFullInfo* qfi, EMTRspInfoStruct* error_info, bool is_last)
{
    if (error_info->error_id || is_last)
    {
        std::cout << "----------OnQueryAllTickersFullInfo----------" << std::endl;
        std::cout << "error_id: " << error_info->error_id << std::endl
                << "error_msg: " << error_info->error_msg << std::endl
                << "is_last: " << is_last << std::endl;
    }

    if (qfi && !is_last) {
        TiQuoteContractInfoField contract = {0};
        if (qfi->exchange_id == EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SH) {
            strcpy(contract.exchange, "SH");
        } else if (qfi->exchange_id == EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZ)
        {
            strcpy(contract.exchange, "SH");
        }
        strcpy(contract.symbol, qfi->ticker);
        strcpy(contract.symbol_name, TiEncodingTool::GbkToUtf8(qfi->ticker_name).c_str());
        contract.date = m_trading_day;
        contract.is_registration = qfi->is_registration;
        contract.is_VIE = qfi->is_VIE;
        contract.is_noprofit = qfi->is_noprofit;
        contract.is_weighted_voting_rights = qfi->is_weighted_voting_rights;
        contract.is_have_price_limit = qfi->is_have_price_limit;
        contract.high_limit = qfi->upper_limit_price;
        contract.low_limit = qfi->lower_limit_price;
        contract.pre_close = qfi->pre_close_price;
        contract.price_tick = qfi->price_tick;
        contract.bid_qty_upper_limit = qfi->bid_qty_upper_limit;
        contract.bid_qty_lower_limit = qfi->bid_qty_lower_limit;
        contract.bid_qty_unit = qfi->bid_qty_unit;
        contract.ask_qty_upper_limit = qfi->ask_qty_upper_limit;
        contract.ask_qty_lower_limit = qfi->ask_qty_lower_limit;
        contract.ask_qty_unit = qfi->ask_qty_unit;

        TiQuoteContractInfoField* pContract = GetContractInfo(contract.symbol, contract.exchange);
        if(pContract){
            memcpy(pContract, &contract, sizeof(TiQuoteContractInfoField));
        }else{
            m_contract_map[TiQuoteTools::GetSymbolID(contract.exchange, contract.symbol)] = std::make_unique<TiQuoteContractInfoField>(contract);
        }
    }

    if (is_last)
    {
        std::cout << "QueryAllTickersFullInfo Done! " << m_contract_map.size() << std::endl;
    }
    
};

// 订阅快照行情响应
void TiDfQuoteL1Client::OnSubMarketData(EMTSpecificTickerStruct *ticker, EMTRspInfoStruct *error_info, bool is_last) {
    std::cout << "----------OnSubMarketData----------" << std::endl;
    std::cout << "securityCode: " << ticker->ticker << std::endl
              << "error_id: " << error_info->error_id << std::endl
              << "error_msg: " << error_info->error_msg << std::endl
              << "is_last: " << is_last << std::endl; // 如果订阅多个标的，通过is_last判断订阅响应是否结束
}

// 订阅全市场指数快照行情响应
void TiDfQuoteL1Client::OnSubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct *error_info) {
    std::cout << "--------------OnSubscribeAllIndexData------------------" << std::endl;
    std::cout << "exchangeType:" << static_cast<int32_t>(exchange_id) << std::endl
              << "error_id: " << error_info->error_id << std::endl
              << "error_msg: " << error_info->error_msg << std::endl;
}

// 现货集中竞价快照行情
void TiDfQuoteL1Client::OnDepthMarketData(EMTMarketDataStruct *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[],
                                  int32_t ask1_count, int32_t max_ask1_count)
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));
    if (market_data->exchange_id == EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SH) {
        strcpy(m_snapStockCash.symbol, "SH");
    } else if (market_data->exchange_id == EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZ) {
        strcpy(m_snapStockCash.symbol, "SZ");
    }

    formatQuoteUpdatetime(market_data->data_time, 
        m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.timestamp);
    datetime::get_format_time_ms(m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp = datetime::get_now_timestamp_ms();

    strcpy(m_snapStockCash.symbol, market_data->ticker);
    m_snapStockCash.last = market_data->last_price;
    m_snapStockCash.pre_close = market_data->pre_close_price;
    m_snapStockCash.open = market_data->open_price;
    m_snapStockCash.high = market_data->high_price;
    m_snapStockCash.low = market_data->low_price;
    m_snapStockCash.high_limit = market_data->upper_limit_price;
    m_snapStockCash.low_limit = market_data->lower_limit_price;
    m_snapStockCash.acc_volume = market_data->qty;
    m_snapStockCash.acc_turnover = market_data->turnover;
    m_snapStockCash.match_items = market_data->trades_count;

    for (int i = 0; i < 5; i++) {
        m_snapStockCash.ask_price[i] = market_data->ask[i];
        m_snapStockCash.ask_volume[i] = market_data->ask_qty[i];
        m_snapStockCash.bid_price[i] = market_data->bid[i];
        m_snapStockCash.bid_volume[i] = market_data->bid_qty[i];
    }

    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapStockCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;

    TiQuoteSnapshotStockField* pSnap = GetStockSnapshot(m_snapStockCash.symbol, m_snapStockCash.exchange);
    if(pSnap){
        memcpy(pSnap, &m_snapStockCash, sizeof(TiQuoteSnapshotStockField));
    }else{
        m_snapshot_map[TiQuoteTools::GetSymbolID(m_snapStockCash.exchange, m_snapStockCash.symbol)] = std::make_unique<TiQuoteSnapshotStockField>(m_snapStockCash);
    }

    
    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
}

// 指数快照行情
void TiDfQuoteL1Client::OnIndexData(EMTIndexDataStruct *index_data) {
    return;
    std::cout << "----------OnIndexData-----------------" << std::endl;
    std::cout << index_data->ticker << std::endl
              << index_data->last_price << std::endl;
};

void TiDfQuoteL1Client::subData(const char* exchangeName, char* codeList[], size_t len)
{
    if (strcmp(exchangeName, "SZ") == 0) {
        if (len)
        {
            m_quote_api->SubscribeMarketData(codeList, len, EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZ);
        }else{
            m_quote_api->SubscribeAllMarketData(EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZ);
        }
    }
    if (strcmp(exchangeName, "SH") == 0) {
        if (len)
        {
            m_quote_api->SubscribeMarketData(codeList, len, EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SH);
        }else{
            m_quote_api->SubscribeAllMarketData(EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SH);
        }
    }
};

TiQuoteSnapshotStockField* TiDfQuoteL1Client::GetStockSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_snapshot_map.find(id);
    if (it != m_snapshot_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};

TiQuoteContractInfoField* TiDfQuoteL1Client::GetContractInfo(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_contract_map.find(id);
    if (it != m_contract_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};