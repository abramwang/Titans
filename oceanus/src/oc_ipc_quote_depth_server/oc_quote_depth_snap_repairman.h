#ifndef OC_QUOTE_DEPTH_SNAP_REPAIRMAN_H
#define OC_QUOTE_DEPTH_SNAP_REPAIRMAN_H

#include "ti_quote_callback.h"
#include <string.h>
#include <cmath>

class OcQuoteDepthSnapRepairman
{
public:
    OcQuoteDepthSnapRepairman(TI_ExchangeType exchange, TI_SymbolType symbol);
    virtual ~OcQuoteDepthSnapRepairman();

private:
    TI_ExchangeType m_exchange;
    TI_SymbolType m_symbol;

    TiQuoteSnapshotStockField* m_original_snapshot_stock;    //最新的原始快照
    TiQuoteSnapshotStockField* m_closest_fitted_snapshot;    //最接近的拟合快照
    TiQuoteSnapshotStockField* m_lasted_fitted_snapshot;     //最新的拟合快照


    int64_t m_acc_volume_diff;
    double  m_acc_turnover_diff;

public:
    void OnL2OriginalStockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2FittedStockSnapshotRtn(const TiQuoteSnapshotStockField* pData, TiQuoteSnapshotStockField* pOut);

};



#endif // OC_QUOTE_DEPTH_SNAP_REPAIRMAN_H