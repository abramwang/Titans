#ifndef _TI_CF_CONTRACT_TOOL_
#define _TI_CF_CONTRACT_TOOL_

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <sstream>

namespace TiCFContractTool {

    // 获取当前日期
    inline std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_c);
        int year = now_tm->tm_year + 1900;
        int month = now_tm->tm_mon + 1; // tm_mon的值范围是0-11，需要加1转换为实际月份
        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << year << std::setw(2) << std::setfill('0') << month;
        return oss.str();
    }

    // 计算合约代码
    inline std::string calculateContractCode(const std::string& prefix, int year, int month) {
        std::ostringstream oss;
        oss << prefix << year << std::setw(2) << std::setfill('0') << month;
        return oss.str();
    }

    inline void getContractCodes(const std::string& prefix) {
        std::string currentDate = getCurrentDate();
        int currentYear = std::stoi(currentDate.substr(2, 2));
        int currentMonth = std::stoi(currentDate.substr(4, 2));
        int currentQuarter = (currentMonth - 1) / 3 + 1;

        std::cout << "当前日期: " << currentDate << std::endl;
        std::cout << "当前年份: " << currentYear << std::endl;
        std::cout << "当前月份: " << currentMonth << std::endl;
        std::cout << "当前季度: " << currentQuarter << std::endl;

        // 当月合约
        std::string currentMonthContract = calculateContractCode(prefix, currentYear, currentMonth);
        std::cout << "当月合约 (当前月): " << currentMonthContract << std::endl;

        // 下月合约
        int nextMonth = (currentMonth == 12) ? 1 : (currentMonth + 1);
        std::string nextMonthContract = calculateContractCode(prefix, currentYear, nextMonth);
        std::cout << "下月合约 (下个月): " << nextMonthContract << std::endl;

        // 当季合约
        int quarterYear = (currentQuarter <= 3) ? currentYear : (currentYear + 1);
        int quarterMonth = (currentQuarter - 1) * 3 + 3;
        std::string quarterContract = calculateContractCode(prefix, quarterYear, quarterMonth);
        std::cout << "当季合约 (本季度): " << quarterContract << std::endl;

        // 下季合约
        quarterMonth = (currentQuarter - 1) * 3 + 6;
        if (quarterMonth > 12) {
            quarterYear = quarterYear + 1;
            quarterMonth = quarterMonth - 12;
        }
        std::string nextQuarterMonthContract = calculateContractCode("IF", quarterYear, quarterMonth);
        std::cout << "下季合约 (下季度): " << nextQuarterMonthContract << std::endl;
    }

#if 0
    // 获取最近的3个合约代码
    inline void getRecentThreeContracts(const std::string& prefix, std::vector<std::string>& contracts) {
        std::string currentYearMonth = getCurrentYearAndMonth();
        // 计算当前月份
        int currentMonth = std::stoi(currentYearMonth.substr(4, 2));
        std::cout << "当前年份和月份: " << currentYearMonth << std::endl;
        // 获取当前年份和月份对应的合约代码
        std::string currentContract = buildFuturesContractCode(prefix, currentYearMonth);
        // 获取下月和季月的合约代码
        std::string nextMonthContract1 = buildFuturesContractCode(prefix, (currentMonth + 1) % 12 < 10 ? "0" + std::to_string((currentMonth + 1) % 12) : std::to_string((currentMonth + 1) % 12));
        std::string nextMonthContract2 = buildFuturesContractCode(prefix, (currentMonth + 3) % 12 < 10 ? "0" + std::to_string((currentMonth + 3) % 12) : std::to_string((currentMonth + 3) % 12));
        
        std::cout << "当前合约: " << currentContract << std::endl;
        std::cout << "下月合约: " << nextMonthContract1 << std::endl;
        std::cout << "季月合约: " << nextMonthContract2 << std::endl;

        contracts.push_back(currentContract);
        contracts.push_back(nextMonthContract1);
        contracts.push_back(nextMonthContract2);
    }
#endif
};

#endif