#include "ti_influxdb_client.h"

TiInfluxdbClient::TiInfluxdbClient(std::string url, std::string token)
{
    m_url = url;
    m_token = token;
}

TiInfluxdbClient::~TiInfluxdbClient()
{
}


void TiInfluxdbClient::add_point(Point &Point)
{
    m_records_vec.push_back(Point);
}

TiInfluxdbClient::Point* TiInfluxdbClient::add_point()
{
    Point point;
    m_records_vec.push_back(point);
    return &m_records_vec.back();
};

/*
curl -X POST "http://192.168.124.6:8086/api/v2/write?org=timely&bucket=ti_test&precision=s" \
    --header "Authorization: Token mFdhDhbwTEAmaXHyYDDEyIJHhM8vjZt0w2ZQePhoAv5cMOZriLiMLpXLKH8_c5tooZ5LPYN9-eB5CC2Owd4u8A==" \
    --data-raw "snapshot,exchange=SH,symbol=600000 close=12.13 1715502420
    snapshot,exchange=SH,symbol=600000 close=12.15 1715502440"
*/
void TiInfluxdbClient::write(std::string bucket, std::string org, std::string precision)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::string url = m_url + "/api/v2/write?org=" + org + "&bucket=" + bucket + "&precision=" + precision;
    std::string data;
    for (auto &point : m_records_vec)
    {
        data += point.get_content() + "\n";
    }
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
        headers = curl_slist_append(headers, ("Authorization: Token " + m_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: text/plain");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        //std::cout << data << std::endl;
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    m_records_vec.clear();
}