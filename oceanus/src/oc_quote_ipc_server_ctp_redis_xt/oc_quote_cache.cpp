#include "oc_quote_cache.h"
#include "ti_quote_struct.h"
#include "ti_quote_tools.h"
#include "datetime.h"

OcQuoteCache::OcQuoteCache(/* args */)
{
}

OcQuoteCache::~OcQuoteCache()
{
}


void OcQuoteCache::init_instrument(std::vector<OCInstrumentInfo> m_instrument_info_list)
{
    for(auto in : m_instrument_info_list)
    {
        if(in.type == OC_INSTRUMENT_TYPE_STOCK || in.type == OC_INSTRUMENT_TYPE_ETF)
        {
            StockPtr stock = std::make_shared<TiQuoteSnapshotStockField>();
            memset(stock.get(), 0, sizeof(TiQuoteSnapshotStockField));
            strcpy(stock->symbol, in.symbol.c_str());
            strcpy(stock->exchange, in.exchange.c_str());
            stock->high_limit = in.limit_up;
            stock->low_limit = in.limit_down;
            stock->pre_close = in.pre_close;
            m_stock_cache[TiQuoteTools::GetSymbolID(stock->exchange, stock->symbol)] = stock;
        }
        else if(in.type == OC_INSTRUMENT_TYPE_INDEX)
        {
            IndexPtr index = std::make_shared<TiQuoteSnapshotIndexField>();
            memset(index.get(), 0, sizeof(TiQuoteSnapshotIndexField));
            strcpy(index->symbol, in.symbol.c_str());
            strcpy(index->exchange, in.exchange.c_str());
            index->pre_close = in.pre_close;
            m_index_cache[TiQuoteTools::GetSymbolID(index->exchange, index->symbol)] = index;
        }
    }
};


bool OcQuoteCache::update_xt_snapshot_cache(std::string code, const json &pData, TiQuoteSnapshotStockField* &out_stock, TiQuoteSnapshotIndexField* &out_index)
{
    TI_ExchangeType exchange = {0};
    TI_SymbolType symbol = {0};
    memcpy(exchange, code.substr(7, 2).c_str(), 2);
    memcpy(symbol, code.substr(0, 6).c_str(), 6);

    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    if(m_stock_cache.find(id) != m_stock_cache.end())
    {
        StockPtr stock = m_stock_cache[id];
        stock->last = pData["lastPrice"].get<double>();
        stock->acc_turnover = pData["amount"].get<double>();
        stock->acc_volume = pData["pvolume"].get<double>();
        stock->match_items = pData["transactionNum"].get<int>();
        stock->open = pData["open"].get<double>();
        stock->high = pData["high"].get<double>();
        stock->low = pData["low"].get<double>();
        stock->iopv = pData["pe"].get<double>();
        
        for(int i = 0; i< 5; i++){
            stock->bid_price[i] = pData["bidPrice"][i].get<double>();
            stock->bid_volume[i] = pData["bidVol"][i].get<double>();
            stock->ask_price[i] = pData["askPrice"][i].get<double>();
            stock->ask_volume[i] = pData["askVol"][i].get<double>();
        }

        stock->timestamp = pData["time"].get<int64_t>();
        datetime::get_date_time_num_from_timestamp_ms(stock->timestamp, stock->date, stock->time);
        stock->recv_timestamp = datetime::get_now_timestamp_ms();

        out_stock = stock.get();
    }
    else if(m_index_cache.find(id) != m_index_cache.end())
    {
        IndexPtr index = m_index_cache[id];
        index->last = pData["lastPrice"].get<double>();
        index->volume = pData["volume"].get<int>();
        index->turnover = pData["amount"].get<double>();
        index->open = pData["open"].get<double>();
        index->high = pData["high"].get<double>();
        index->low = pData["low"].get<double>();
        index->timestamp = pData["time"].get<int64_t>();
        datetime::get_date_time_num_from_timestamp_ms(index->timestamp, index->date, index->time);
        out_index = index.get();
    }
    else
    {
        return false;
    }
    return true;
};