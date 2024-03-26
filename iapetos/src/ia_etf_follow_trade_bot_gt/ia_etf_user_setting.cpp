#include "ia_etf_user_setting.h"

IaEtfUserSetting::IaEtfUserSetting(RedisSyncHandle* redis_client)
{
    m_redis_client = redis_client;
}

IaEtfUserSetting::~IaEtfUserSetting()
{
}

