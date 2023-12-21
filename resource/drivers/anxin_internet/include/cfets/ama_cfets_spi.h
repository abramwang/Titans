#ifndef _AMA_CFETS_SPI_H_
#define _AMA_CFETS_SPI_H_

#include "ama_cfets_struct.h"

namespace amd { namespace ama {
/**
 * @brief AMA中的接收银行间数据的回调基类，IAMDSpi已继承该类。
 */
class AMA_EXPORT CFETSSpi
{
public:
    virtual ~CFETSSpi() {}

    /**
     * @brief                       接收现券买卖逐笔成交数据回调
     *  
     * @param ticks                 现券买卖逐笔成交数据
     * @param cnt                   现券买卖逐笔成交数据条数
     */
    virtual void OnMDSpotTickExecution(MDSpotTickExecution* ticks, uint32_t cnt) ;

    /**
     * @brief                       接收现券买卖汇总成交行情数据回调
     *  
     * @param snaps                 现券买卖汇总成交行情数据
     * @param cnt                   现券买卖汇总成交行情数据条数
     */
    virtual void OnMDSpotSummarySnapshot(MDSpotSummarySnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收现券买卖匿名点击成交行情(X-Bond)数据回调
     *  
     * @param snaps                 现券买卖匿名点击成交行情(X-Bond)数据
     * @param cnt                   现券买卖匿名点击成交行情(X-Bond)数据条数
     */
    virtual void OnMDSpotXBondSnapshot(MDSpotXBondSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购逐笔成交数据回调
     *  
     * @param snaps                 质押式回购逐笔成交数据
     * @param cnt                   质押式回购逐笔成交数据条数
     */
    virtual void OnMDPledgeRepoTickExecution(MDPledgeRepoTickExecution* ticks, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购汇总成交行情数据回调
     *  
     * @param snaps                 质押式回购汇总成交行情数据
     * @param cnt                   质押式回购汇总成交行情数据条数
     */
    virtual void OnMDPledgeRepoSummarySnapshot(MDPledgeRepoSummarySnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购询价成交行情数据回调
     *  
     * @param snaps                 质押式回购询价成交行情数据
     * @param cnt                   质押式回购询价成交行情数据条数
     */
    virtual void OnMDPledgeRepoInquirySnapshot(MDPledgeRepoInquirySnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购存款类机构间成交行情数据回调
     *  
     * @param snaps                 质押式回购存款类机构间成交行情数据
     * @param cnt                   质押式回购存款类机构间成交行情数据条数
     */
    virtual void OnMDPledgeRepoDepositSnapshot(MDPledgeRepoDepositSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购双边匿名点击(X-Repo)成交行情数据回调
     *  
     * @param snaps                 质押式回购双边匿名点击(X-Repo)成交行情数据
     * @param cnt                   质押式回购双边匿名点击(X-Repo)成交行情数据条数
     */
    virtual void OnMDPledgeRepoXRepoSnapshot(MDPledgeRepoXRepoSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收同业拆借逐笔成交数据回调
     *  
     * @param snaps                 同业拆借逐笔成交数据
     * @param cnt                   同业拆借逐笔成交数据条数
     */
    virtual void OnMDCeditLoanTickExecution(MDCeditLoanTickExecution* ticks, uint32_t cnt) ;

    /**
     * @brief                       接收同业拆借成交行情数据回调
     *  
     * @param snaps                 同业拆借成交行情数据
     * @param cnt                   同业拆借成交行情数据条数
     */
    virtual void OnMDCeditLoanSnapshot(MDCeditLoanSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收同业拆借存款类机构间成交行情数据回调
     *  
     * @param snaps                 同业拆借存款类机构间成交行情数据
     * @param cnt                   同业拆借存款类机构间成交行情数据条数
     */
    virtual void OnMDCeditLoanDepositSnapshot(MDCeditLoanDepositSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收利率互换固定-浮动逐笔成交数据回调
     *  
     * @param ticks                 利率互换固定-浮动逐笔成交数据
     * @param cnt                   利率互换固定-浮动逐笔成交数据条数
     */
    virtual void OnMDSwapFixedFloatTickExecution(MDSwapFixedFloatTickExecution* ticks, uint32_t cnt) ;

    /**
     * @brief                       接收利率互换固定-浮动成交行情数据回调
     *  
     * @param snaps                 利率互换固定-浮动成交行情数据
     * @param cnt                   利率互换固定-浮动成交行情数据条数
     */
    virtual void OnMDSwapFixedFloatSnapshot(MDSwapFixedFloatSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收利率互换匿名点击(X-Swap)成交行情数据回调
     *  
     * @param snaps                 利率互换匿名点击(X-Swap)成交行情数据
     * @param cnt                   利率互换匿名点击(X-Swap)成交行情数据条数
     */
    virtual void OnMDSwapXSwapSnapshot(MDSwapXSwapSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收利率互换匿名点击（X-Swap）最优报价行情数据回调
     *  
     * @param snaps                 利率互换匿名点击（X-Swap）最优报价行情数据
     * @param cnt                   利率互换匿名点击（X-Swap）最优报价行情数据条数
     */
    virtual void OnMDSwapXSwapBestOfferSnapshot(MDSwapXSwapBestOfferSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收债券借贷逐笔成交数据回调
     *  
     * @param ticks                 债券借贷逐笔成交数据
     * @param cnt                   债券借贷逐笔成交数据条数
     */
    virtual void OnMDBondLoanTickExecution(MDBondLoanTickExecution* ticks, uint32_t cnt) ;

    /**
     * @brief                       接收债券借贷标的债券借贷费率行情数据回调
     *  
     * @param snaps                 债券借贷标的债券借贷费率行情数据
     * @param cnt                   债券借贷标的债券借贷费率行情数据条数
     */
    virtual void OnMDBondLoanUnderlyingSecurityRateSnapshot(MDBondLoanUnderlyingSecurityRateSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收债券借贷交易品种费率行情数据回调
     *  
     * @param snaps                 债券借贷交易品种费率行情数据
     * @param cnt                   债券借贷交易品种费率行情数据条数
     */
    virtual void OnMDBondLoanTradingProductRateSnapshot(MDBondLoanTradingProductRateSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收现券买卖做市报价深度行情数据回调
     *  
     * @param snaps                 现券买卖做市报价深度行情数据
     * @param cnt                   现券买卖做市报价深度行情数据条数
     */
    virtual void OnMDSpotMarketMakersQuoteSnapshot(MDSpotMarketMakersQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收现券买卖指示性报价深度行情数据回调
     *  
     * @param snaps                 现券买卖指示性报价深度行情数据
     * @param cnt                   现券买卖指示性报价深度行情数据条数
     */
    virtual void OnMDSpotDirectiveQuoteSnapshot(MDSpotDirectiveQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收现券买卖匿名点击(X-Bond)报价深度行情数据回调
     *  
     * @param snaps                 现券买卖匿名点击(X-Bond)报价深度行情数据
     * @param cnt                   现券买卖匿名点击(X-Bond)报价深度行情数据条数
     */
    virtual void OnMDSpotXBondDepthQuoteSnapshot(MDSpotXBondDepthQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收现券买卖匿名点击(X-Bond)报价集中匹配行情数据回调
     *  
     * @param snaps                 现券买卖匿名点击(X-Bond)报价集中匹配行情数据
     * @param cnt                   现券买卖匿名点击(X-Bond)报价集中匹配行情数据条数
     */
    virtual void OnMDSpotXBondMatchQuoteSnapshot(MDSpotXBondMatchQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购匿名点击(X-Repo)报价深度行情数据回调
     *  
     * @param snaps                 质押式回购匿名点击(X-Repo)报价深度行情数据
     * @param cnt                   质押式回购匿名点击(X-Repo)报价深度行情数据条数
     */
    virtual void OnMDPledgeRepoXRepoDepthQuoteSnapshot(MDPledgeRepoXRepoDepthQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收质押式回购匿名点击(X-Repo)报价最优行情数据回调
     *  
     * @param snaps                 质押式回购匿名点击(X-Repo)报价最优行情数据
     * @param cnt                   质押式回购匿名点击(X-Repo)报价最优行情数据条数
     */
    virtual void OnMDPledgeRepoXRepoBestOfferQuoteSnapshot(MDPledgeRepoXRepoBestOfferQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收利率互换匿名点击(X-Swap)报价深度行情数据回调
     *  
     * @param snaps                 利率互换匿名点击(X-Swap)报价深度行情数据
     * @param cnt                   利率互换匿名点击(X-Swap)报价深度行情数据条数
     */
    virtual void OnMDSwapXSwapDepthQuoteSnapshot(MDSwapXSwapDepthQuoteSnapshot* snaps, uint32_t cnt) ;

    /**
     * @brief                       接收利率互换匿名点击(X-Swap)逐笔成交数据回调
     *  
     * @param snaps                 利率互换匿名点击(X-Swap)逐笔成交数据
     * @param cnt                   利率互换匿名点击(X-Swap)逐笔成交数据条数
     */
    virtual void OnMDSwapXSwapTickExecution(MDSwapXSwapTickExecution* ticks, uint32_t cnt) ;
}; // end of CFETSSpi


}; // end of ama
}; // end of amd

#endif // end of 
