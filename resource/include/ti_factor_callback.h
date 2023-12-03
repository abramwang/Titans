#ifndef TI_FACTOR_CALLBACK_H
#define TI_FACTOR_CALLBACK_H

#include <nlohmann/json.hpp>
using namespace nlohmann;

class TiFactorCallback
{
public:
    virtual void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data) = 0;
    /* data */
public:
    TiFactorCallback(){};
    virtual ~TiFactorCallback(){};
};

#endif 