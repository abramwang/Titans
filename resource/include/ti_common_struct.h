#ifndef TI_COMMON_STRUCT_H
#define TI_COMMON_STRUCT_H

#if defined(_MSC_VER) && _MSC_VER<1600
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
#else
#include <stdint.h>
#endif
#include <cstddef>

const int TI_EXCHANGE_STR_LEN       = 8;
const int TI_SYMBOL_STR_LEN         = 32;
const int TI_SYMBOL_NAME_STR_LEN    = 32;
const int TI_TIME_STR_LEN     = 48;
const int TI_ORDER_ID_STR_LEN = 64;
const int TI_TEXT_STR_LEN     = 128;

typedef char TI_ExchangeType[TI_EXCHANGE_STR_LEN];
typedef char TI_SymbolType[TI_SYMBOL_STR_LEN];
typedef char TI_SymbolNameType[TI_SYMBOL_NAME_STR_LEN];
typedef char TI_CodeName[32];
typedef char TI_AccountType[32];
typedef char TI_ISODateTimeType[TI_TIME_STR_LEN];    //日期和时间类型(格式 yyyy-MM-dd hh:mm:ss.mmmmmm)
typedef char TI_OrderIdType[TI_ORDER_ID_STR_LEN];
typedef char TI_Text[TI_TEXT_STR_LEN];
typedef char TI_ClientIdType[TI_TEXT_STR_LEN];


#pragma pack(1)


#pragma pack()
#endif