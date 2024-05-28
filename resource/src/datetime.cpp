#include <string.h>
#include "datetime.h"

unsigned int datetime::get_today(){
    struct tm *tm_t;
    struct timeval time;
    
    gettimeofday(&time,NULL);
    tm_t = localtime(&time.tv_sec);
    
    return (tm_t->tm_year + 1900)*10000+ (tm_t->tm_mon + 1)*100 + tm_t->tm_mday;
};

long long datetime::get_time_num()
{
    struct tm *tm_t;
    struct timeval time;
    
    gettimeofday(&time,NULL);
    tm_t = localtime(&time.tv_sec);
    
    return tm_t->tm_hour*10000000 + tm_t->tm_min*100000 + tm_t->tm_sec*1000 + time.tv_usec/1000;
};

long long datetime::get_time_sec_num(){
    struct tm *tm_t;
    struct timeval time;
    
    gettimeofday(&time,NULL);
    tm_t = localtime(&time.tv_sec);
    
    return tm_t->tm_hour*10000 + tm_t->tm_min*100 + tm_t->tm_sec;
};

long long datetime::get_timestamp(){
    time_t t;
    time(&t);
    return t;
};

long long datetime::get_now_timestamp_ms(void)//获取时间戳函数
{
    long long tmp;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tmp = tv.tv_sec;
    tmp = tmp * 1000;
    tmp = tmp + (tv.tv_usec / 1000);

    return tmp;
};

long long datetime::get_timestamp_ms(unsigned int day_num, unsigned int time_num){
    struct tm tm_t = {0};
    tm_t.tm_year    = day_num / 10000 - 1900;
    tm_t.tm_mon     = day_num / 100 - (day_num / 10000)*100 - 1; 
    tm_t.tm_mday    = day_num - (day_num / 100)* 100;

    tm_t.tm_hour    = time_num / 10000000;
    tm_t.tm_min     = (time_num / 100000) - (time_num / 10000000)*100;
    tm_t.tm_sec     = time_num/1000 - (time_num / 100000)* 100;
    unsigned int _msec = time_num%1000;

    time_t time1 = mktime(&tm_t);
    return time1 * 1000 + _msec;
};

long long datetime::get_timestamp_ms(char* day_str, char* time_str){
    struct tm tm_t = {0};
    unsigned int day_num = atoi(day_str);
    tm_t.tm_year    = day_num / 10000 - 1900;
    tm_t.tm_mon     = day_num / 100 - (day_num / 10000)*100 - 1; 
    tm_t.tm_mday    = day_num - (day_num / 100)* 100;


    char *token;

    token = strtok(time_str, ":");
    tm_t.tm_hour = atoi(token);

    token = strtok(NULL, ":");
    tm_t.tm_min = atoi(token);

    token = strtok(NULL, ":");
    tm_t.tm_sec = atoi(token);

    time_t time1 = mktime(&tm_t);
    return time1 * 1000;
};

long long datetime::get_timestamp_ms(long long datetime_num){
    unsigned int _day, _time = 0;
    _time   = datetime_num % 1000000000;
    _day    = datetime_num / 1000000000;

    return get_timestamp_ms(_day, _time);
};

void datetime::get_format_timestamp_ms(long long src_timestamp, char *dis_str_time, unsigned int dis_len) { 
    if (dis_len < 32) {
        printf("input buff len less than 32");	
        return;
    }
    int _msec = src_timestamp % 1000;
    time_t _time = src_timestamp / 1000;

    struct tm *tm_t;

    tm_t = localtime(&_time);
    
    if(NULL != tm_t) {
        sprintf(dis_str_time,"%04d-%02d-%02d %02d:%02d:%02d.%06d",
            tm_t->tm_year + 1900,
            tm_t->tm_mon + 1, 
            tm_t->tm_mday,
            tm_t->tm_hour, 
            tm_t->tm_min, 
            tm_t->tm_sec,
            _msec*1000);
    }
};

std::string datetime::get_format_timestamp_ms(long long src_timestamp) {
    char dis_str_time[32] = {0};
    get_format_timestamp_ms(src_timestamp, dis_str_time, 32);
    return std::string(dis_str_time);
};

void datetime::get_format_time_ms(unsigned int day_num, unsigned int time_num,
    char *dis_str_time, unsigned int dis_len){

    if (dis_len < 32) {
        printf("input buff len less than 32");	
        return;
    }

    struct tm tm_t = {0};
    tm_t.tm_year    = day_num / 10000 - 1900;
    tm_t.tm_mon     = day_num / 100 - (day_num / 10000)*100 - 1; 
    tm_t.tm_mday    = day_num - (day_num / 100)* 100;

    tm_t.tm_hour    = time_num / 10000000;
    tm_t.tm_min     = (time_num / 100000) - (time_num / 10000000)*100;
    tm_t.tm_sec     = time_num/1000 - (time_num / 100000)* 100;

    unsigned int _msec = time_num%1000;

    sprintf(dis_str_time,"%04d-%02d-%02d %02d:%02d:%02d.%06d",
        tm_t.tm_year + 1900,
        tm_t.tm_mon + 1, 
        tm_t.tm_mday,
        tm_t.tm_hour, 
        tm_t.tm_min, 
        tm_t.tm_sec,
        _msec*1000);
};

std::string datetime::get_format_time_ms(unsigned int day_num, unsigned int time_num){
    char dis_str_time[32] = {0};
    get_format_time_ms(day_num, time_num, dis_str_time, 32);
    return std::string(dis_str_time);
};

void datetime::get_format_now_time_ms(char *dis_str_time, unsigned int dis_len) { 
    if (dis_len < 32) {
        printf("input buff len less than 32");	
        return;
    }

    struct tm *tm_t;
    struct timeval time;
    
    gettimeofday(&time,NULL);
    tm_t = localtime(&time.tv_sec);
    if(NULL != tm_t) {
        sprintf(dis_str_time,"%04d-%02d-%02d %02d:%02d:%02d.%06ld",
            tm_t->tm_year + 1900,
            tm_t->tm_mon + 1, 
            tm_t->tm_mday,
            tm_t->tm_hour, 
            tm_t->tm_min, 
            tm_t->tm_sec,
            time.tv_usec/1000);
    }
};

std::string datetime::get_format_now_time_ms() {
    char dis_str_time[32] = {0};
    get_format_now_time_ms(dis_str_time, 32);
    return std::string(dis_str_time);
};

void datetime::get_format_now_time_us(char *dis_str_time, unsigned int dis_len) {
    if (dis_len < 38) {
        printf("input buff len less than 38");	
        return;
    }

    struct tm *tm_t;
    struct timeval time;
    
    gettimeofday(&time,NULL);
    tm_t = localtime(&time.tv_sec);
    if(NULL != tm_t) {
        sprintf(dis_str_time,"%04d-%02d-%02d %02d:%02d:%02d.%06ld",
            tm_t->tm_year + 1900,
            tm_t->tm_mon + 1, 
            tm_t->tm_mday,
            tm_t->tm_hour, 
            tm_t->tm_min, 
            tm_t->tm_sec,
            time.tv_usec);
    }
};

std::string datetime::get_format_now_time_us(){
    char dis_str_time[38] = {0};
    get_format_now_time_us(dis_str_time, 38);
    return std::string(dis_str_time);
};

void datetime::get_format_time_duration_ms(long long time_duration_ms, char *dis_str_time, unsigned int dis_len){
    if (dis_len < 32) {
        printf("input buff len less than 38");	
        return;
    }
    int _msec = time_duration_ms % 1000;
    int _sec = time_duration_ms / 1000;
    int _min = _sec / 60;
    _sec = _sec % 60;
    sprintf(dis_str_time,"%d:%02d.%03d", _min, _sec, _msec);
};

std::string datetime::get_format_time_duration_ms(long long time_duration_ms){
    char dis_str_time[32] = {0};
    get_format_time_duration_ms(time_duration_ms, dis_str_time, 32);
    return std::string(dis_str_time);
};