#ifndef __EES_QUOTE_RESPONSE_MSG_H__
#define __EES_QUOTE_RESPONSE_MSG_H__

#include "EESQuoteDefine.h"
#include "cJSON.h"

namespace EES_QUOTE_RESPONSE_MSG {
    //OnQueryUserAccount
    void to_json(cJSON *j, EESMarketDepthQuoteData* o);
};

#endif