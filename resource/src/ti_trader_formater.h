#ifndef __TI_TRADER_FORMATER_H__
#define __TI_TRADER_FORMATER_H__

#include "ti_trader_struct.h"
#include <nlohmann/json.hpp>
using namespace nlohmann;

namespace TiTraderFormater{
    void FormatOrderDelete(const TiRspOrderDelete* pData, json& j);
    void FormatPosition(const TiRspQryPosition* pData, json& j);
    void FormatOrderStatus(const TiRtnOrderStatus* pData, json& j);
    void FormatOrderMatchEvent(const TiRtnOrderMatch* pData, json& j);

    void FormatAccountInfo(const TiRspAccountInfo* pData, json& j);
}

#endif