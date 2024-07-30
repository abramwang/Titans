#ifndef __OC_QUOTE_CACHE_H__
#define __OC_QUOTE_CACHE_H__

#include "ti_quote_struct.h"
#include "oc_quote_info_mysql.h"
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>

using namespace nlohmann;

class OcQuoteCache
{
private:
    typedef std::shared_ptr<TiQuoteSnapshotStockField> StockPtr;
    typedef std::shared_ptr<TiQuoteSnapshotIndexField> IndexPtr;

    std::unordered_map<int64_t, StockPtr> m_stock_cache;
    std::unordered_map<int64_t, IndexPtr> m_index_cache;
public:
    OcQuoteCache(/* args */);
    ~OcQuoteCache();

private:
    //添加北交所涨跌停
    void add_bj_stock_cache(std::string symbol, std::string exchange, double pre_close);
public:
    void init_instrument(std::vector<OCInstrumentInfo> m_instrument_info_list);
    bool update_xt_snapshot_cache(std::string code, const json &pData, TiQuoteSnapshotStockField* &stock, TiQuoteSnapshotIndexField* &index);
};

#endif // __OC_QUOTE_CACHE_H__
