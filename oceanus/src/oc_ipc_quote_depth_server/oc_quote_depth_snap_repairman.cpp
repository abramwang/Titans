#include "oc_quote_depth_snap_repairman.h"


OcQuoteDepthSnapRepairman::OcQuoteDepthSnapRepairman(TI_ExchangeType exchange, TI_SymbolType symbol)
{
    memcpy(&m_exchange, &exchange, sizeof(TI_ExchangeType));
    memcpy(&m_symbol, &symbol, sizeof(TI_SymbolType));

    m_original_snapshot_stock   = NULL;
    m_closest_fitted_snapshot   = NULL;
    m_lasted_fitted_snapshot    = NULL;

    m_acc_volume_diff = 0;
    m_acc_turnover_diff = 0;
}

OcQuoteDepthSnapRepairman::~OcQuoteDepthSnapRepairman()
{
    if (m_original_snapshot_stock != NULL)
    {
        delete m_original_snapshot_stock;
        m_original_snapshot_stock = NULL;
    }

    if (m_closest_fitted_snapshot != NULL)
    {
        delete m_closest_fitted_snapshot;
        m_closest_fitted_snapshot = NULL;
    }

    if (m_lasted_fitted_snapshot != NULL)
    {
        delete m_lasted_fitted_snapshot;
        m_lasted_fitted_snapshot = NULL;
    }
}


void OcQuoteDepthSnapRepairman::OnL2OriginalStockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    if (!m_original_snapshot_stock)
    {
        m_original_snapshot_stock = new TiQuoteSnapshotStockField;
        memset(m_original_snapshot_stock, 0, sizeof(TiQuoteSnapshotStockField));
    }
    memcpy(&m_original_snapshot_stock, pData, sizeof(TiQuoteSnapshotStockField));
}

void OcQuoteDepthSnapRepairman::OnL2FittedStockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    if (!m_closest_fitted_snapshot)
    {
        m_closest_fitted_snapshot = new TiQuoteSnapshotStockField;
        memset(m_closest_fitted_snapshot, 0, sizeof(TiQuoteSnapshotStockField));
    }

    if (!m_lasted_fitted_snapshot)
    {
        m_lasted_fitted_snapshot = new TiQuoteSnapshotStockField;
        memset(m_lasted_fitted_snapshot, 0, sizeof(TiQuoteSnapshotStockField));
    }
    memcpy(&m_lasted_fitted_snapshot, pData, sizeof(TiQuoteSnapshotStockField));


    if (m_original_snapshot_stock == NULL)
    {
        return;
    } 

    int64_t time_diff_lasted = std::abs(m_original_snapshot_stock->timestamp - m_lasted_fitted_snapshot->timestamp);
    int64_t time_diff_closest = std::abs(m_original_snapshot_stock->timestamp - m_closest_fitted_snapshot->timestamp);

    if (time_diff_lasted <= time_diff_closest)
    {
        memcpy(&m_closest_fitted_snapshot, m_lasted_fitted_snapshot, sizeof(TiQuoteSnapshotStockField));

        m_acc_volume_diff = m_original_snapshot_stock->acc_volume - m_closest_fitted_snapshot->acc_volume;
        m_acc_turnover_diff = m_original_snapshot_stock->acc_turnover - m_closest_fitted_snapshot->acc_turnover;
    }
}