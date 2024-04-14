#ifndef _TI_CF_CONTRACT_TOOL_
#define _TI_CF_CONTRACT_TOOL_

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <sstream>

namespace TiCFContractTool {

    // 获取当前日期是这个月的第几周
    inline int getWeekOfMonth(const std::tm* timeInfo) {
        if (timeInfo == nullptr) {
            return -1; // 错误处理，timeInfo未正确设置
        }

        int week = 1; // 从第1周开始计数
        int weekday = timeInfo->tm_wday; // 获取当前日期是星期几，0代表星期天，1代表星期一，以此类推
        int day = timeInfo->tm_mday; // 获取当前日期的天数

        // 计算从月初到当前日期的天数
        int daysFromStart = day - 1 + (7 - weekday);

        // 计算当前日期是这个月的第几周
        week = (daysFromStart + 6) / 7; // 加6是因为星期天是一周的第0天，所以要对齐到星期一开始的周计算

        return week;
    };

    // 获取当前日期是周几
    int getDayOfWeek(const std::tm* timeInfo) {
        if (timeInfo == nullptr) {
            return -1; // 错误处理，timeInfo未正确设置
        }
        return timeInfo->tm_wday; // 获取当前日期是星期几，0代表星期天，1代表星期一，以此类推
    }

    // 获取当前日期
    inline std::string getCurrentDate(int &week, int &dayOfWeek) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_c);

        now_tm->tm_mday = 22; // 设置为当月第22天
        now_tm->tm_wday = 1;

        week = getWeekOfMonth(now_tm);
        dayOfWeek = getDayOfWeek(now_tm);
        int year = now_tm->tm_year + 1900;
        int month = now_tm->tm_mon + 1; // tm_mon的值范围是0-11，需要加1转换为实际月份
        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << year << std::setw(2) << std::setfill('0') << month << std::setw(2) << std::setfill('0') << now_tm->tm_mday;
        return oss.str();
    };

    // 计算合约代码
    inline std::string calculateContractCode(const std::string& prefix, int year, int month) {
        std::ostringstream oss;
        oss << prefix << year << std::setw(2) << std::setfill('0') << month;
        return oss.str();
    };

    inline void getContractCodes(const std::string& prefix, std::vector<std::string>& contracts) {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&currentTime);

        // 初始化年份和月份
        int year = now_tm->tm_year + 1900; // tm_year是从1900年开始计算的
        int month = now_tm->tm_mon + 1; // tm_mon是从0开始计算的月份


        // 输出往后6个月的月份列表
        for (int i = 0; i < 6; ++i) {
            // 增加月份，注意月份是从1开始的
            std::string contract = calculateContractCode(prefix, year%100, month);
            contracts.push_back(contract);
            std::cout << contract << std::endl;

            month++;
            if (month > 12) {
                month = 1; // 重置月份为1，年份增加
                year++;
            }
        }
    };

};

#endif