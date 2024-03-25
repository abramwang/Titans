#ifndef _AMA_CFETS_API_H_
#define _AMA_CFETS_API_H_

#include "../ama_export.h"
#include "ama_cfets_struct.h"

namespace amd { namespace ama {
/**
 * 
 */
class AMA_EXPORT CFETSApi
{
public:

    /**
     * @brief                       查询债券信息(银行间)数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetBondInfoInterbank(BondInfoInterbankList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询资产支持证券信息(ABS)数据操作
     *
     * @param security_code         查询的代码信息
     * @param data                  查询返回的数据
     * @param cnt                   查询返回的数据项个数
     * @return
     */
    static bool GetABSInfo(ABSInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询资产支持证券历史信息(ABS历史)数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetABSHistoryInfo(ABSHistoryInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询资产支持证券信用评级数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetABSCreditRatings(ABSCreditRatingsList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询预发行债券信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetPreIssuedBondInfo(PreIssuedBondInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询上市前债券信息(银行间)数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetPreIPOBondInfo(PreIPOBondInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询可交易债券信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetXBondTradeBondInfo(XBondTradeBondInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询质押券折算率(匿名点击)信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetPledgedConvertRateACInfo(PledgedConvertRateACInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询 X-Repo 分层报价群组信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @return
     */
    static bool GetXRepoHierQuoteGroupInfo(XRepoHierQuoteGroupInfoList& list);

    /**
     * @brief                       查询X-Repo 合约信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetXRepoContractInfo(XRepoContractInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询利率互换(固浮)信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetSwapFixedFloatInfo(SwapFixedFloatInfoList& list, RDIQueryItem* items, uint32_t cnt);

    /**
     * @brief                       查询利率互换(固浮)期差基差合约信息数据操作
     *
     * @param list                  查询结果数据,使用完后需要调用FreeMemory函数释放内部data指针数据(out)
     * @param item                  查询数据项头指针，具体参数请参考 RDIQueryItem (in)
     * @param cnt                   查询的数据项个数(in)
     * @return
     */
    static bool GetSwapFixedFloatBasisContractInfo(SwapFixedFloatBasisContractInfoList& list, RDIQueryItem* items, uint32_t cnt);
}; // end of CFETSApi


}; // end of ama
}; // end of amd

#endif // end of 