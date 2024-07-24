#ifndef __TI_FEE_H__
#define __TI_FEE_H__

namespace TiFee{

    // 定义Stock费率常量
    const double STOCK_COMMISSION_RATE          = 0.00009;      // 假设佣金费率为0.009%
    const double FUND_STOCK_COMMISSION_RATE     = 0.0002;       // 假设公募代买代卖佣金费率为0.02%
    const double STAMP_DUTY_RATE                = 0.0005;       // 印花税率为0.05%

    // 最低佣金
    const double MIN_COMMISSION                 = 0.0;          // 最低佣金 5元(通常都免5)

    // 定义Stock费率常量
    const double ETF_COMMISSION_RATE            = 0.000045;     // 假设佣金费率为0.0045%


    // 计算股票交易费用    
    double calculate_stock_fees(double transactionAmount, bool isBuy, bool isFund);

    // 计算ETF交易费用
    double calculate_etf_fees(double transactionAmount);

}

#endif