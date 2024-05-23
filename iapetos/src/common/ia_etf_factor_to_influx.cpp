#include "ia_etf_factor_to_influx.h"


IaEtfFactorToInflux::IaEtfFactorToInflux(std::string url, std::string token)
{
    m_influxdb_client = new TiInfluxdbClient(url, token);
}

IaEtfFactorToInflux::~IaEtfFactorToInflux()
{
    delete m_influxdb_client;
}


void IaEtfFactorToInflux::add_point(std::string measurement, json &data)
{
    if (m_influxdb_client == nullptr)
    {
        return;
    }

    if (!data.is_object())
    {
        return;
    }

    if (!data.contains("tags"))
    {
        return;
    }

    if (!data.contains("fields"))
    {
        return;
    }

    if (!data.contains("timestamp"))
    {
        return;
    }

    TiInfluxdbClient::Point* point = m_influxdb_client->add_point();
    point->meas(measurement);

    for (json::iterator it = data["tags"].begin(); it != data["tags"].end(); ++it)
    {
        point->tag(it.key(), it.value().get<std::string>());
    }

    for (json::iterator it = data["fields"].begin(); it != data["fields"].end(); ++it)
    {
        double value = it.value().get<double>();
        point->field(it.key(), value);
    }

    int64_t timestamp = data["timestamp"].get<int64_t>();
    point->timestamp(timestamp);
}

void IaEtfFactorToInflux::write(std::string bucket, std::string org, std::string precision)
{
    if (m_influxdb_client == nullptr)
    {
        return;
    }

    m_influxdb_client->write(bucket, org, precision);
}