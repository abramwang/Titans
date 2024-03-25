#ifndef _AMA_USA_SPI_H_
#define _AMA_USA_SPI_H_

#include "ama_usa_struct.h"

namespace amd { namespace ama {
/**
 * @brief AMA中的接收美股数据的回调基类，IAMDSpi已继承该类。
 */
class AMA_EXPORT UsaSpi
{
public:
    virtual ~UsaSpi() {}

    /**
     * @brief                       接收美股股票快照数据回调
     *
     * @param snaps                 美股股票快照数据
     * @param cnt                   美股股票快照数据条数
     */
    virtual void OnMDUsaStockSnapshot(MDUsaStockSnapshot* snaps, uint32_t cnt);


    /**
     * @brief                       接收美股挂单快照数据回调
     *
     * @param snaps                 美股挂单快照数据
     * @param cnt                   美股挂单快照数据条数
     */
    virtual void OnMDUsaOrderSnapshot(MDUsaOrderSnapshot* snaps, uint32_t cnt);


    /**
     * @brief                       接收美股EOD收盘快照数据回调
     *
     * @param snaps                 美股EOD收盘快照数据
     * @param cnt                   美股EOD收盘快照条数
     */
    virtual void OnMDUsaEodSnapshot(MDUsaEodSnapshot* snaps, uint32_t cnt);


}; // end of UsaSpi


}; // end of ama
}; // end of amd

#endif // end of 
