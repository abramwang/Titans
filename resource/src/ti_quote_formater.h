#ifndef __TI_QUOTE_FORMATER_H__
#define __TI_QUOTE_FORMATER_H__

#include "ti_quote_struct.h"
#include <nlohmann/json.hpp>
using namespace nlohmann;

namespace TiQuoteFormater{
    void FormatSnapshot(const TiQuoteSnapshotIndexField* pData, json& j);
    void FormatSnapshot(const TiQuoteSnapshotFutureField* pData, json& j);
    void FormatSnapshot(const TiQuoteSnapshotStockField* pData, json& j);
    void FormatMatch(const TiQuoteMatchesField* pData, json& j);
    void FormatOrder(const TiQuoteOrderField* pData, json& j);
}

#endif