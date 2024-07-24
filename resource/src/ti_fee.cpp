#include "ti_fee.h"

double TiFee::calculate_stock_fees(double transactionAmount, bool isBuy, bool isPublicFund) {
    double commissionRate = isPublicFund ? FUND_STOCK_COMMISSION_RATE : STOCK_COMMISSION_RATE;
    double commission = transactionAmount * commissionRate;
    if (commission < MIN_COMMISSION) {
        commission = MIN_COMMISSION;
    }

    double stampDuty = 0.0;

    if (!isBuy) {
        stampDuty = transactionAmount * STAMP_DUTY_RATE;
    }
    
    return commission + stampDuty;
}


double TiFee::calculate_etf_fees(double transactionAmount) {
    double commission = transactionAmount * ETF_COMMISSION_RATE;
    if (commission < MIN_COMMISSION) {
        commission = MIN_COMMISSION;
    }

    return commission;
};
