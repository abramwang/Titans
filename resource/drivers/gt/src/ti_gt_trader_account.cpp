#include "ti_gt_trader_account.h"

TiGtTraderAccount::TiGtTraderAccount(TI_BrokerType broker_type, TI_AccountType account, TiTraderCallback* userCb)
{
    m_cb = userCb;

};


TiGtTraderAccount::~TiGtTraderAccount()
{

};