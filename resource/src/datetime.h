#ifndef DATETIME_H
#define DATETIME_H

#include <time.h>
#include <sys/timeb.h>

#include <stdio.h>
#include <sys/time.h>
#include <string>
#include <time.h>

namespace datetime {
    unsigned int get_today();

    long long get_time_num();

    long long get_timestamp();

    long long get_now_timestamp_ms(void);

    long long get_timestamp_ms(unsigned int day_num, unsigned int time_num);

    long long get_timestamp_ms(char* day_str, char* time_str);  //20240130 10:26:14

    long long get_timestamp_ms(long long datetime_num);

    void get_format_timestamp_ms(long long src_timestamp, char *dis_str_time, unsigned int dis_len);
    std::string get_format_timestamp_ms(long long src_timestamp);
    
    void get_format_time_ms(unsigned int day_num, unsigned int time_num,
        char *dis_str_time, unsigned int dis_len);
    std::string get_format_time_ms(unsigned int day_num, unsigned int time_num);

    void get_format_now_time_ms(char *dis_str_time, unsigned int dis_len);
    std::string get_format_now_time_ms();

    void get_format_now_time_us(char *dis_str_time, unsigned int dis_len);
    std::string get_format_now_time_us();

    void get_format_time_duration_ms(long long time_duration_ms, char *dis_str_time, unsigned int dis_len);
    std::string get_format_time_duration_ms(long long time_duration_ms);

};




#endif