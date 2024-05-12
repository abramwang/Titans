#ifndef TI_INFLUXDB_CLIENT_H
#define TI_INFLUXDB_CLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

class TiInfluxdbClient
{
public:
    class Point{
        std::string m_content;
        std::string m_measurement;
        std::string m_tag;
        std::string m_field;
        int64_t m_time;
    public:
        void meas(std::string measurement){
            m_measurement = measurement;
        };
        //snapshot,exchange=SH,symbol=600000 close=12.13 1715502420
        void tag(std::string tag, std::string value){
            if (!m_tag.empty())
            {
                m_tag += ",";
            }
            m_tag += tag + "=" + value;
        };

        template <typename T>
        void field(std::string field, T &value){
            if (!m_field.empty())
            {
                m_field += ",";
            }
            m_field += field + "=" + std::to_string(value);
        };

        void timestamp(int64_t time){
            m_time = time;
            m_content = m_measurement + "," + m_tag + " " + m_field + " " + std::to_string(m_time);
        };

        std::string get_content(){
            return m_content;
        };
        
        Point(){};
        ~Point(){};
    };

private:
    std::string m_url;      //http://192.168.124.6:8086
    std::string m_token;    //mFdhDhbwTEAmaXHyYDDEyIJHhM8vjZt0w2ZQePhoAv5cMOZriLiMLpXLKH8_c5tooZ5LPYN9-eB5CC2Owd4u8A==
    /* data */

    std::vector<Point> m_records_vec;
public:
    TiInfluxdbClient(std::string url, std::string token);
    ~TiInfluxdbClient();

    void add_point(Point &Point);
    Point* add_point();
    void write(std::string bucket, std::string org, std::string precision);
};

#endif