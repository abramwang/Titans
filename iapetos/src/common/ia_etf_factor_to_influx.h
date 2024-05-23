#ifndef __IA_ETF_FACTOR_TO_INFLUX_H__
#define __IA_ETF_FACTOR_TO_INFLUX_H__

#include "ti_influxdb_client.h"
#include <string>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaEtfFactorToInflux
{
private:
    TiInfluxdbClient* m_influxdb_client;
    
public:
    IaEtfFactorToInflux(std::string url, std::string token);
    ~IaEtfFactorToInflux();

public:
    void add_point(std::string measurement, json &data);
    void write(std::string bucket, std::string org, std::string precision);
};

#endif // __IA_ETF_FACTOR_TO_INFLUX_H__

