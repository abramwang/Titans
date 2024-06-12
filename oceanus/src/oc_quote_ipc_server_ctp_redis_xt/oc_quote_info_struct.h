#ifndef __OC_QUOTE_INFO_STRUCT_H__
#define __OC_QUOTE_INFO_STRUCT_H__

#include <string>



const char OC_INSTRUMENT_TYPE_DEFAULT     = 0;
const char OC_INSTRUMENT_TYPE_STOCK       = 1;
const char OC_INSTRUMENT_TYPE_FUTURE      = 2;
const char OC_INSTRUMENT_TYPE_INDEX       = 3;
const char OC_INSTRUMENT_TYPE_ETF         = 4;
typedef char OC_INSTRUMENT_TYPE;

/*
CREATE TABLE `ti_instrument_info` (
  `symbol` text,
  `exchange` text,
  `name` text,
  `type` text,
  `limit_up` double DEFAULT NULL,
  `limit_down` double DEFAULT NULL,
  `pre_close` double DEFAULT NULL,
  `update_date` date DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
*/

struct OCInstrumentInfo {
    std::string symbol;
    std::string exchange;
    std::string name;
    OC_INSTRUMENT_TYPE type;
    double limit_up;
    double limit_down;
    double pre_close;
    std::string update_date;
};

namespace OcQuoteInfoStruct {
    inline OC_INSTRUMENT_TYPE pares_instrument_type(std::string type_string){
        if (type_string == "stock") {
            return OC_INSTRUMENT_TYPE_STOCK;
        } else if (type_string == "future") {
            return OC_INSTRUMENT_TYPE_FUTURE;
        } else if (type_string == "index") {
            return OC_INSTRUMENT_TYPE_INDEX;
        } else if (type_string == "etf") {
            return OC_INSTRUMENT_TYPE_ETF;
        } else {
            return OC_INSTRUMENT_TYPE_DEFAULT;
        }
    };
}

#endif //__OC_QUOTE_INFO_STRUCT_H__