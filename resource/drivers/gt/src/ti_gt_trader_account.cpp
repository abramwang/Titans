#include "ti_gt_trader_account.h"

#include <iostream>

TiGtTraderAccount::TiGtTraderAccount(TI_BrokerType broker_type, TI_AccountType account, TiTraderCallback* userCb)
{
    m_cb = userCb;

};

TiGtTraderAccount::~TiGtTraderAccount()
{

};


void TiGtTraderAccount::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){
    std::cout << *rspData << std::endl;
}; 
    