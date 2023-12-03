#ifndef _AMA_HKEX_SPI_H_
#define _AMA_HKEX_SPI_H_

#include "ama_hkex_struct.h"

namespace amd { namespace ama {
/**
 * @brief AMA中的接收商业港股数据的回调基类，IAMDSpi已继承该类。
 */
class AMA_EXPORT HKExSpi
{
public:
    virtual ~HKExSpi() {}

    /**
     * @brief                       接收商业港股股票快照数据回调
     *
     * @param snaps                 商业港股股票快照数据
     * @param cnt                   商业港股股票快照数据条数
     */
    virtual void OnMDHKExSnapshot(MDHKExSnapshot* snaps, uint32_t cnt);


    /**
     * @brief                       接收商业港股委托挂单数据回调
     *
     * @param snaps                 商业港股委托挂单数据
     * @param cnt                   商业港股委托挂单数据条数
     */
    virtual void OnMDHKExOrderSnapshot(MDHKExOrderSnapshot* snaps, uint32_t cnt);


    /**
     * @brief                       接收商业港股经纪席位数据回调
     *
     * @param snaps                 商业港股经纪席位数据
     * @param cnt                   商业港股经纪席位条数
     */
    virtual void OnMDHKExOrderBrokerSnapshot(MDHKExOrderBrokerSnapshot* snaps, uint32_t cnt);


    /**
     * @brief                       接收商业港股逐笔成交数据回调
     *
     * @param ticks                 商业港股逐笔成交数据
     * @param cnt                   商业港股逐笔成交数据条数
     */
    virtual void OnMDHKExTickExecution(MDHKExTickExecution* ticks, uint32_t cnt);


    /**
     * @brief                       接收商业港股指数行情快照数据回调
     *
     * @param snaps                 商业港股指数行情快照数据
     * @param cnt                   商业港股指数行情快照数据条数
     */
    virtual void OnMDHKExIndexSnapshot(MDHKExIndexSnapshot* snaps, uint32_t cnt);
}; // end of HKExSpi


}; // end of ama
}; // end of amd

#endif // end of 
