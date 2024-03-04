/********************************************************************
    company:    北京睿智融科控股有限公司
    fileName:   XtTraderApi.h
    author:     xujun    
    created:    8:11:2013   17:01    
    purpose:    
*********************************************************************/
#ifndef XtTraderApi_2013_11_8_H
#define XtTraderApi_2013_11_8_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XtDef.h"
#include "XtError.h"
#include "XtStructs.h"

namespace xti
{
    class XtTraderApiCallback;

    /**
    * @brief 交易api
    * nRequestId：数据请求ID，是数据请求和请求结果对应的标识
    * @note 请求成功后的回调函数会传回请求函数的 nRequestId
    * @note 只用资金帐号登录成功后才可以请求该帐号的相应数据
    */
    class XT_API_EXPORT XtTraderApi
    {
    public:
        virtual ~XtTraderApi(){};

        /**
        * @brief 获取XtTraderApi实例
        * @param [in] address XtApiService监听端口
        */
        static XtTraderApi* createXtTraderApi(const char* address);

        /**
        * @brief 设置数据回调对象
        * @param [in] pCallback XtTraderApiCallback类实例
        */
        virtual void setCallback(XtTraderApiCallback* pCallback) = 0;

        /**
        * @brief 创建api实例，并进行初始化
        * @param [in] configFilePath 配置文件夹目录，默认是"../config"，运行目录上一层的config下
        */
        virtual bool init(const char* configFilePath = "../config") = 0;

        /**
        * @brief 同步创建api实例，并进行初始化
        * @param [in] configFilePath 配置文件夹目录，默认是"../config"，运行目录上一层的config下
        */
        virtual XtError initSync(const char* configFilePath = "../config") = 0;

        /**
        * @brief 启动XtTraderApi单实例线程, 多实例情况必须调用joinAll()函数或者joinAll_async()函数
        * @note 需要调用该函数阻塞线程，否则没有请求XtTraderApi实例会退出
        */
        virtual void join() = 0;

        /**
        * @brief 异步启动XtTraderApi单实例线程, 多实例情况必须调用joinAll()或者joinAll_async()函数
        * @note 需要调用该函数阻塞线程，否则没有请求XtTraderApi实例会退出
        */
        virtual void join_async() = 0;

        /**
        * @brief 启动XtTraderApi多实例线程, 多实例情况下必须使用该函数, 同时单实例也可调用该函数
        * @note 需要调用该函数阻塞线程，否则没有请求XtTraderApi实例会退出
        */
        static void joinAll();

        /**
        * @brief 异步启动XtTraderApi多实例线程, 多实例情况下必须使用该函数, 同时单实例也可调用该函数
        * @note 需要调用该函数阻塞线程，否则没有请求XtTraderApi实例会退出
        */
        virtual void joinAll_async() = 0;

        /**
        * @brief 析构XtTraderApi实例
        */
        virtual void destroy() = 0;

        /**
        * @brief 销毁XtTraderApi多实例线程, 所有实例停止工作, joinAll()函数解除等待状态
        */
        static void destroyAll();

        /**
        * 获取迅投交易用户名
        */
        virtual const char* getUserName() = 0;

        /**
        * 获取Api版本
        */
        virtual const char* getVersion() = 0;

        /**
        * @deprecated 已废弃，使用reqAccountKeys()代替
        * @brief 获取对应账号key
        * @param [in] accountID 账号ID
        * @param [in] accountKeys accountID对应的账号key，用于区分accountID相同时不同类型的账号
        * @param [in] index accountKeys数组的大小
        */
        virtual XtError getKey(const char* accountID, CAccountKey* accountKeys, int index) = 0;

        /**
        * @brief 获取账号key对应产品Id
        * @param [in] accountKey 账号key
        */
        virtual int reqProductIdByAccountKey(const char* accountKey) = 0;

        /**
        * @brief 是否启用指令级成交统计,默认不启用。启用后,会主动对运行中的指令发起成交明细查询，统计完成交后通过onRtnOrderStat回调函数推送统计数据。本函数需在登录前调用。
        * @param [in] flag 是否启用
        */
        virtual void enableOrderStat(bool flag){};

        /**
        * @brief 是否启用撤指令后补撤委托，默认不启用。启用后,会主动对撤销过的指令发起委托明细查询，针对发起撤指令但对应委托未了结的委托进行撤委托操作，直到委托撤销或没撤掉成交，下游只用关心指令状态即可。本函数需在登录前调用。
        */
        virtual void enableCmdCancelOrder() = 0;

        /**
        * @brief 用户登陆 同步接口
        * @note 调用此函数后，返回 XtError结构, 判断 isSuccess
        * @param [in] userName 用户名
        * @param [in] password 用户密码
        * @param [in] machineInfo 如果需要下单设置站点信息，则传入，否则可以传空
        * @param [in] appid 如果是期货中继模式，需传入监管处申请的appid，则传入，否则可以传空
        * @param [in] authcode 如果是期货中继模式，需传入监管处申请的authcode，则传入，否则可以传空
        */
        virtual XtError userLoginSync(const char* userName, const char* password, const char* machineInfo = NULL, const char* appid = NULL, const char* authcode = NULL) = 0;


        /**
        * @brief 用户登陆
        * @note 调用此函数后，回调 onUserLogin
        * @param [in] userName 用户名
        * @param [in] password 用户密码
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] machineInfo 如果需要下单设置站点信息，则传入，否则可以传空
        * @param [in] appid 如果是期货中继模式，需传入监管处申请的appid，则传入，否则可以传空
        * @param [in] authcode 如果是期货中继模式，需传入监管处申请的authcode，则传入，否则可以传空
        */
        virtual void userLogin(const char* userName, const char* password, int nRequestId, const char* machineInfo = NULL, const char* appid = NULL, const char* authcode = NULL) = 0;

        /**
        * @brief 用户登出
        * @note 调用此函数后，回调 onUserLogout
        * @param [in] userName 用户名
        * @param [in] password 用户密码
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void userLogout(const char* userName, const char* password, int nRequestId) = 0;

        /**
        * @brief 请求账号资金信息
        * @note 调用此函数后，返回vector<xti::CAccountDetail>
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CAccountDetail> reqAccountDetailSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号资金信息
        * @note 调用此函数后，回调 onReqAccountDetail，两融账号还会回调onReqCreditAccountDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqAccountDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求产品下所有账号资金信息
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品id
        */
        virtual std::vector<xti::CAccountDetail> reqAccountDetailSyncWithProductId(XtError& error, int productId) = 0 ;

        /**
        * @brief 请求账号委托明细信息
        * @note 调用此函数后，回调 onReqOrderDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqOrderDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号委托明细信息
        * @note 调用此函数后，回调 onReqOrderDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqOrderDetail_sync(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号委托明细信息
        * @note 调用此函数后，返回vector<xti::COrderDetail>
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::COrderDetail> reqOrderDetailSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 根据指令号请求账号委托明细信息
        * @note 调用此函数后，回调 onReqOrderDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] nOrderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqOrderDetail(const char* accountID, int nRequestId, int nOrderID, const char* accountKey = "") = 0;


        /**
        * @brief 同步根据指令号请求账号委托明细信息
        * @note 调用此函数后，返回vector<xti::COrderDetail>
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] nOrderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::COrderDetail> reqOrderDetailSyncByOrderID(const char* accountID, XtError& error, int nOrderID, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号成交明细信息
        * @note 调用此函数后，回调 std::vector<xti::CDealDetail>结构
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CDealDetail> reqDealDetailSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求产品下所有账号成交明细信息
        * @note 调用此函数后，回调 std::vector<xti::CDealDetail>结构
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品ID
        */
        virtual std::vector<xti::CDealDetail> reqDealDetailSyncWithProductId( XtError& error, int productId) = 0;

        /**
        * @brief 同步请求账号成交统计信息
        * @note 调用此函数后，回调 std::vector<xti::CDealDetail>结构
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CDealStatics> reqDealStaticsSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求产品下所有账号成交统计信息
        * @note 调用此函数后，回调 std::vector<xti::CDealDetail>结构
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品ID
        */
        virtual std::vector<xti::CDealStatics> reqDealStaticsSyncWithProductId(XtError& error, int productId) = 0 ;

        /**
        * @brief 同步根据指令号请求账号成交明细信息
        * @note 调用此函数后，回调 CDealDetail结构
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] nOrderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CDealDetail> reqDealDetailSyncByOrderID(const char* accountID, XtError& error, int nOrderID, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号成交明细信息
        * @note 调用此函数后，回调 onReqDealDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqDealDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号成交明细信息
        * @note 调用此函数后，回调 onReqDealDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqDealDetail_sync(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 根据指令号请求账号成交明细信息
        * @note 调用此函数后，回调 onReqDealDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] nOrderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqDealDetail(const char* accountID, int nRequestId, int nOrderID, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号持仓明细信息
        * @note 调用此函数后，回调 onReqPositionDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqPositionDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号持仓明细信息
        * @note 调用此函数后，回调 CPositionDetail
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CPositionDetail> reqPositionDetailSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号持仓明细信息
        * @note 调用此函数后，回调 onReqPositionDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqPositionDetail_sync(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号持仓统计信息
        * @note 调用此函数后，回调 onReqPositionStatics
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqPositionStatics(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号持仓统计信息
        * @note 调用此函数后，回调 CPositionStatics
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CPositionStatics> reqPositionStaticsSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号持仓统计信息
        * @note 调用此函数后，回调 onReqPositionStatics
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqPositionStatics_sync(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号日初持仓统计信息
        * @note 调用此函数后，回调 onReqInitialPositionStatics
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqInitialPositionStatics(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号日初持仓统计信息
        * @note 调用此函数后，回调 CPositionStatics
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CPositionStatics> reqInitialPositionStaticsSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求产品下所有账号持仓统计
        * @note 调用此函数后，回调 CPositionStatics结构
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品ID
        */
        virtual std::vector<xti::CPositionStatics> reqPositionStaticsSyncWithProductId( XtError& error, int productId) = 0;

        /**
        * @brief 请求信用账号负债合约信息，该接口已废弃
        * @note 调用此函数后，回调 onReqStkcompacts
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqStkcompacts(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求信用账号标的信息
        * @note 调用此函数后，回调 onReqStksubjects
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqStksubjects(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求期权账号备兑持仓信息
        * @note 调用此函数后，回调 onReqCoveredStockPosition
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCoveredStockPosition(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求期权账号组合持仓信息
        * @note 调用此函数后，回调 onReqStkOptCombPositionDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqStkOptCombPositionDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求行情数据信息
        * @note 调用此函数后，回调 onReqPriceData
        * @param [in] exchangeId 市场，取值参考MarketType.h
        * @param [in] instrumentId 合约代码
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPriceData(const char* exchangeId, const char* instrumentId, int nRequestId) = 0;

        /**
        * @brief 请求行情数据信息 同步接口
        * @param [in] exchangeId 市场，取值参考MarketType.h
        * @param [in] instrumentId 合约代码
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        */
        virtual std::vector<xti::CPriceData> reqPriceDataSync(const char* exchangeId, const char* instrumentId, XtError& error) = 0;

        /**
        * @brief 按市场请求行情数据信息
        * @note 调用此函数后，回调 onReqCInstrumentDetail
        * @param [in] exchangeId 市场，取值参考MarketType.h
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPriceDataByMarket(const char* exchangeId, int nRequestId) = 0;

        /**
        * @brief 请求账号期权行情信息
        * @note 调用此函数后，回调 onReqCInstrumentDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqInstrumentDetail(const char* accountID, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 订阅行情数据
        * @note code为"allCode"时，订阅整个市场，调用此函数后，回调 onSubscribQuote
        * @param [in] data 订阅行情参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void subscribQuote(const CSubscribData* data, int nRequestId) = 0;

        /**
        * @brief 批量订阅行情数据
        * @note 调用此函数后，回调 onSubscribQuote
        * @param[in] data 订阅行情参数
        * @param[in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void batchSubscribQuote(const std::vector<CSubscribData>* data, int nRequestId) = 0;

        /**
        * @brief 退订行情数据
        * @note code为"allCode"时，订阅整个市场，调用此函数后，回调 onUnSubscribQuote
        * @param [in] data 订阅行情参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void unSubscribQuote(const CSubscribData* data, int nRequestId) = 0;

        /**
        * @brief 批量退订行情数据
        * @note 调用此函数后，回调 onUnSubscribQuote
        * @param[in] data 订阅行情参数
        * @param[in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void batchUnSubscribQuote(const std::vector<CSubscribData>* data, int nRequestId) = 0;

        /**
        * @brief 请求账号汇率信息
        * @note 调用此函数后，回调 onReqReferenceRate
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqGGTReferenceRate(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求两融账号综合资金信息
        * @note 调用此函数后，回调 onReqCreditDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCreditDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求两融账号综合资金信息
        * @note 调用此函数后，返回 vector<xti::CCreditDetail>
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual xti::CCreditDetail reqCreditDetailSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步接口普通单下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @param [in] orderInfo 普通单请求参数
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */

        virtual int orderSync(const COrdinaryOrder* orderInfo, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 普通单下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 普通单，回调 onOrder
        * @param [in] orderInfo 普通单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const COrdinaryOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 直接下单，支持股票，期货，个股期权，期货期权，沪港通，深港通，只支持指定价普通下单
        * @note 期货六键普通委托，OPT_OPEN_LONG - OPT_CLOSE_SHORT_TODAY
        * @note 股票港股普通委托，OPT_BUY - OPT_SELL
        * @note 两融和专项委托，OPT_FIN_BUY - OPT_DIRECT_CASH_REPAY和OPT_SLO_SELL_SPECIAL - OPT_DIRECT_CASH_REPAY_SPECIAL
        * @note 期权普通委托，OPT_OPTION_BUY_OPEN - OPT_OPTION_COVERED_CLOSE和OPT_OPTION_SECU_LOCK - OPT_OPTION_SECU_UNLOCK
        * @note 普通单，回调 onDirectOrder
        * @param [in] orderInfo 普通单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void directOrder(const COrdinaryOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 组合算法单下单，只支持股票
        * @note 组合算法单，回调 onOrder
        * @param [in] orderInfo 组合算法单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CGroupOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 组合智能算法单下单同步接口，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @param[in] orderInfo 组合智能算法单请求参数
        * @param[in] error 错误信息，用于接收同步请求时的错误信息
        * @param[in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual int orderSync(const CAlgGroupOrder* orderInfo, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 组合智能算法单下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 组合智能算法单，回调 onOrder
        * @param [in] orderInfo 组合智能算法单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CAlgGroupOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 组合外部算法单下单同步接口，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @param [in] orderInfo 组合外部算法单请求参数
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号        */
        virtual int orderSync(const CExternAlgGroupOrder* orderInfo, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 组合外部算法单下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 组合外部算法单，回调 onOrder
        * @param [in] orderInfo 组合外部算法单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CExternAlgGroupOrder* orderInfo, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 算法单下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号        */
        virtual int orderSync(const CAlgorithmOrder* orderInfo, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 算法单下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 算法单，回调 onOrder
        * @param [in] orderInfo 算法单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CAlgorithmOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 随机量交易下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 随机量交易单，回调 onOrder
        * @param [in] orderInfo 随机量交易请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CRandomOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 智能算法下单同步接口，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @param[in] orderInfo 智能算法交易请求参数
        * @param[in] error 错误信息，用于接收同步请求时的错误信息
        * @param[in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual int orderSync(const CIntelligentAlgorithmOrder* orderInfo, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 智能算法下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 智能算法单，回调 onOrder
        * @param [in] orderInfo 智能算法交易请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CIntelligentAlgorithmOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 主动算法下单同步接口，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @param[in] orderInfo 主动算法交易请求参数
        * @param[in] error 错误信息，用于接收同步请求时的错误信息
        * @param[in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual int orderSync(const CExternAlgorithmOrder* orderInfo, XtError& error, const char* accountKey = "") = 0;
        
        /**
        * @brief 主动算法下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 主动算法单，回调 onOrder
        * @param [in] orderInfo 主动算法交易请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CExternAlgorithmOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 中信建投算法下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 主动算法单，回调 onOrder
        * @param [in] orderInfo 主动算法交易请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const CZxjtAlgorithmOrder* orderInfo, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 普通组合下单，支持股票，期货，个股期权，期货期权，沪港通，深港通
        * @note 普通组合单，回调 onOrder
        * @param [in] orderInfo 普通组合交易请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void order(const COrdinaryGroupOrder* orderInfo, int nRequestId,const char* accountKey = "") = 0;


        /**
        * @brief 同步撤单，按指令号撤单
        * @note 撤销指令，终止某个单子的运行
        * @param [in] orderID 指令号
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号        */
        virtual void cancelSync(int orderID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 按指令号撤单
        * @note 撤销指令，终止某个单子的运行，回调 onCancel
        * @param [in] orderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void cancel(int orderID, int nRequestId) = 0;

        /**
        * @brief 同步按委托号撤单
        * @note 撤销委托，终止某个单子的运行，回调 CCancelSync
        * @param [in] accountID 账号ID
        * @param [in] orderSyeId 委托号，可通过onReqOrderDetail或者onRtnOrderDetail返回参数COrderDetail.m_strOrderSysID获得
        * @param [in] exchangeId 委托对应的市场，可通过onReqOrderDetail或者onRtnOrderDetail返回参数COrderDetail.m_strExchangeID获得，有些柜台撤单需要送入市场和合约代码，没有特殊要求可以送空
        * @param [in] instrumentId 委托对应的合约代码，可通过onReqOrderDetail或者onRtnOrderDetail返回参数COrderDetail.m_strInstrumentID获得，有些柜台撤单需要送入市场和合约代码，没有特殊要求可以送空
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void cancelOrderSync(const char* accountID, const char* orderSyeId, const char* exchangeId, const char* instrumentId, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 按委托号撤单
        * @note 撤销委托，终止某个单子的运行，回调 onCancelOrder
        * @param [in] accountID 账号ID
        * @param [in] orderSyeId 委托号，可通过onReqOrderDetail或者onRtnOrderDetail返回参数COrderDetail.m_strOrderSysID获得
        * @param [in] exchangeId 委托对应的市场，可通过onReqOrderDetail或者onRtnOrderDetail返回参数COrderDetail.m_strExchangeID获得，有些柜台撤单需要送入市场和合约代码，没有特殊要求可以送空
        * @param [in] instrumentId 委托对应的合约代码，可通过onReqOrderDetail或者onRtnOrderDetail返回参数COrderDetail.m_strInstrumentID获得，有些柜台撤单需要送入市场和合约代码，没有特殊要求可以送空
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void cancelOrder(const char* accountID, const char* orderSyeId, const char* exchangeId, const char* instrumentId, int nRequestId,const char* accountKey = "") = 0;

        /**
        * @brief 普通单风险试算
        * @note 对某笔普通委托试算，回调 onCheck
        * @param [in] orderInfo 普通单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void check(const COrdinaryOrder* orderInfo ,int nRequestId) = 0;

        /**
        * @brief 算法单风险试算
        * @note 对某笔算法委托试算，回调 onCheck
        * @param [in] orderInfo 算法单请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void check(const CGroupOrder* orderInfo ,int nRequestId) = 0;

        /**
        * @brief 请求用户产品信息
        * @note 调用此函数后，回调 onReqProductData
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqProductData(int nRequestId) = 0;

        /**
        * @brief 请求用户产品信息同步接口
        * @note 调用此函数后，返回std::vector<xti::CProductData>
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        */
        virtual std::vector<xti::CProductData> reqProductDataSync(XtError& error) = 0;

        /**
        * @brief 请求投资组合可用持仓统计信息
        * @note 调用此函数后，回调 onReqRevolvePositions
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void queryRevolvePositions(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 上传终端ctp采集信息，用于传入ctp柜台需要留痕的终端信息
        * @param [in] accountID 账号ID
        * @param [in] IpPortAddr 上传终端信息机器的ip和端口信息(格式为ip:port，例如127.0.0.1:58000)
        * @param [in] len ctp采集信息内容的长度len，即CTP_GetSystemInfo入参nLen
        * @param [in] CTPSystemInfo ctp采集到的内容
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void  registerUserSystemInfo(const char* accountID, const char* IpPortAddr, const int len, const char* CTPSystemInfo, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @deprecated 已废弃，使用startNamedTimer()代替
        * @brief 设置用户自己的定时器
        * @note 调用此函数后，回调 onCustomTimer
        * @param [in] millsec 定时器间隔，单位ms
        */
        virtual void startTimer(int millsec) = 0;

        /**
        * @deprecated 已废弃，使用stopNamedTimer()代替
        * @brief 停止用户自己的定时器
        */
        virtual void stopTimer() = 0;

        /**
        * @brief 设置用户自己的定时器
        * @note 调用此函数后，回调 onNamedTimer
        * @param [in] timerName 定时器名称
        * @param [in] intervalInMilliSecond 定时器间隔，单位毫秒
        * @param [in] time 定时器第一次触发的时间，单位毫秒，如14点25分36秒789毫秒就是142536789，输入负数表示从当前时间开始计时
        * @param [in] nextDay 触发的时间是否在下一日
        */
        virtual void startNamedTimer(const char* timerName, int intervalInMilliSecond, int time = -1, bool nextDay = false) = 0;

        /**
        * @brief 停止用户自己的定时器
        * @param [in] timerName 定时器名称
        */
        virtual void stopNamedTimer(const char* timerName) = 0;

        /**
        * @brief 设置用户下单指令冻结选项
        * @param [in] nCmdFrzCheckOption 用户下单指令冻结选项，1 禁止 2 警告
        */
        virtual void setCmdFrzCheckOption(int nCmdFrzCheckOption) = 0;

        /**
        * @brief 指令暂停恢复
        * @param [in] op 指令暂停恢复请求
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void operateTask(const CTaskOpRecord* op, const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 智能算法指令改单
        * @param [in] orderInfo 智能算法指令改单参数，m_strAccountID，m_strMarket，m_strInstrument和m_eOperationType不能改，其他的都能改
        * @param [in] nOrderID 需要改参的指令号
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void modifyAlgoCommands(const CIntelligentAlgorithmOrder* orderInfo, int nOrderID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 普通算法指令改单
        * @param [in] orderInfo 普通算法指令改单参数，只能改m_dPrice，m_nVolume，m_ePriceType，m_strRemark
        * @param [in] nOrderID 需要改参的指令号
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void modifyAlgoCommands(const CAlgorithmOrder* orderInfo, int nOrderID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号新股额度信息
        * @note 调用此函数后，回调 onReqSubscribeInfo
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqSubscribeInfo(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号北交所询价额度信息
        * @note 调用此函数后，回调 onReqEnquirySubscribeInfo
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqEnquirySubscribeInfo(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求信用账号未了结负债信息
        * @note 调用此函数后，回调 onReqStkUnCloseCompact
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqStkUnCloseCompacts(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求信用账号未了结负债信息 同步接口
        * @note 调用此函数后，回调 std::vector<xti::CStkUnClosedCompacts>结构
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CStkUnClosedCompacts> reqStkUnCloseCompactsSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 请求信用账号已了结负债信息
        * @note 调用此函数后，回调 onReqStkClosedCompact
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqStkClosedCompacts(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求信用账号已了结负债信息 同步接口
        * @note 调用此函数后，回调 std::vector<xti::CStkClosedCompacts>结构
        * @param [in] accountID 账号ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CStkClosedCompacts> reqStkClosedCompactsSync(const char* accountID, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号委托明细信息
        * @note 调用此函数后，回调 onReqOrderDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqOrderDetailNew(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号成交明细信息
        * @note 调用此函数后，回调 onReqDealDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqDealDetailNew(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 获取用户下所有账号key
        * @note 调用此函数后，回调 onReqAccountKey
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        */
        virtual std::vector<xti::CAccountKey> reqAccountKeysSync(XtError& error) = 0;

        /**
        * @brief 获取用户下所有账号key
        * @note 调用此函数后，回调 onReqAccountKey
        * @param[in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqAccountKeys(int nRequestId) = 0;

        /**
        * @brief 根据委托号请求账号成交明细信息
        * @note 调用此函数后，回调 onReqDealDetailBySysID
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] orderSyeId 下单的委托号
        * @param [in] exchangeId 委托所属市场
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqDealDetailBySysID(const char* accountID, int nRequestId, const char* orderSyeId, const char* exchangeId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号结算单信息
        * @note 调用此函数后，回调 onReqDeliveryDetail
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqDeliveryDetail(const char* accountID, const char* startDate, const char* endDate, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号合约行情信息
        * @note 调用此函数后，回调 onReqSingleInstrumentInfo
        * @param [in] exchangeId 市场代码
        * @param [in] instrumentId 合约代码
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqSingleInstrumentInfo(const char* exchangeId, const char* instrumentId, int nRequestId) = 0;

        /**
        * @brief 请求账号可下单量
        * @note 调用此函数后，回调 onReqOpVolume
        * @param [in] opVolumeReq 可下单量请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqOpVolume(const COpVolumeReq* opVolumeReq, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号可下单量 (根据迅投系统当前持仓和资金计算，可能跟实际情况有误差)
        * @note 调用此函数后，回调 onReqCanOrderVolume
        * @param [in] opVolumeReq 可下单量请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCanOrderVolume(const COpVolumeReq* opVolumeReq, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号可下单量 (根据迅投系统当前持仓和资金计算，可能跟实际情况有误差)
        * @param [in] opVolumeReq 可下单量请求参数
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual int reqCanOrderVolumeSync(const COpVolumeReq* opVolumeReq, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 请求两融账号融券可融数量信息
        * @note 调用此函数后，回调 onReqCreditSloCode
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCreditSloCode(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求两融账号融资融券标的信息
        * @note 调用此函数后，回调 onReqCreditSubjects
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCreditSubjects(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求两融账号担保标的信息
        * @note 调用此函数后，回调 onReqCreditAssure
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCreditAssure(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号银证转账银行信息
        * @note 调用此函数后，回调 onReqTransferBank
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqTransferBank(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号银证转账银行流水
        * @note 调用此函数后，回调 onReqTransferSerial
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqTransferSerial(const char* accountID, const char* startDate, const char* endDate, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号银证转账银行余额
        * @note 调用此函数后，回调 onReqBankAmount
        * @param [in] bankInfo 银证转账银行余额请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqBankAmount(const CQueryBankInfo* bankInfo, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 银证转账
        * @note 调用此函数后，回调 onTransfer
        * @param [in] transferReq 银证转账请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void transfer(const CTransferReq* transferReq, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 按市场请求合约信息
        * @note 调用此函数后，回调 onReqInstrumentInfoByMarket
        * @param [in] exchangeId 市场，取值参考MarketType.h
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqInstrumentInfoByMarket(const char* exchangeId, int nRequestId) = 0;

        /**
        * @brief 请求账号可撤单委托明细信息
        * @note 调用此函数后，回调 onReqCanCancelOrderDetail
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqCanCancelOrderDetail(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账户所有下单信息
        * @note 调用此函数后，回调 onReqCommandsInfo
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqCommandsInfo(int nRequestId) = 0;

        /**
        * @brief 同步请求账户所有指令信息
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        */
        virtual std::vector<xti::COrderInfo> reqCommandsInfoSync( XtError& error) = 0;

        /**
        * @brief 资金划拨
        * @note 调用此函数后，回调 onFundTransfer
        * @param [in] fundTransferReq 资金划拨请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void fundTransfer(const CSecuFundTransferReq* fundTransferReq, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 股份划拨
        * @note 调用此函数后，回调 onSecuTransfer
        * @param [in] secuTransferReq 股份划拨请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void secuTransfer(const CSecuFundTransferReq* secuTransferReq, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号普通（集中）柜台资金
        * @note 调用此函数后，回调 onReqComFund
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqComFund(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号普通（集中）柜台持仓
        * @note 调用此函数后，回调 onReqComPosition
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqComPosition(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 获取当前交易日
        * @note 调用此函数后，回调 onReqTradeDay
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqTradeDay(int nRequestId) = 0;

        /**
        * @brief 请求账号历史委托明细
        * @note 调用此函数后，回调 onReqHistoryOrderDetail
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqHistoryOrderDetail(const char* accountID, const char* startDate, const char* endDate, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号历史委托明细
        * @note 调用此函数后，回调 COrderDetail结构
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::COrderDetail> reqHistoryOrderDetailSync(const char* accountID, const char* startDate, const char* endDate, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 请求账号历史成交明细
        * @note 调用此函数后，回调 onReqHistoryDealDetail
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqHistoryDealDetail(const char* accountID, const char* startDate, const char* endDate, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号历史成交明细
        * @note 调用此函数后，回调 CDealDetail结构
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CDealDetail> reqHistoryDealDetailSync(const char* accountID, const char* startDate, const char* endDate, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求产品下所有账号历史成交明细
        * @note 调用此函数后，回调 CDealDetail结构
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品ID
        */
        virtual std::vector<xti::CDealDetail> reqHistoryDealDetailSyncWithProductId(const char* startDate, const char* endDate, XtError& error, int productId) = 0;

        /**
        * @brief 同步请求产品下所有账号历史成交统计
        * @note 调用此函数后，回调 CDealStatics结构
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品ID
        */
        virtual std::vector<xti::CDealStatics> reqHistoryDealStaticsSyncWithProductId(const char* startDate, const char* endDate, XtError& error, int productId) = 0;

        /**
        * @brief 同步请求产品下所有账号历史持仓统计
        * @note 调用此函数后，回调 CPositionStatics结构
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] productId 产品ID
        */
        virtual std::vector<xti::CPositionStatics> reqHistoryPositionStaticsSyncWithProductId(const char* startDate, const char* endDate, XtError& error, int productId) = 0;
        
        /**
        * @brief 请求账号历史持仓统计，回调onReqHistoryPositionStatics
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqHistoryPositionStatics(const char* accountID, const char* startDate, const char* endDate, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号历史持仓统计
        * @note 调用此函数后，回调 CPositionStatics结构
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CPositionStatics> reqHistoryPositionStaticsSync(const char* accountID, const char* startDate, const char* endDate, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 同步请求账号历史资金信息
        * @note 调用此函数后，回调 CAccountDetail结构
        * @param [in] accountID 账号ID
        * @param [in] startDate 查询开始时间
        * @param [in] endDate 查询结束时间
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual std::vector<xti::CAccountDetail> reqHistoryAccountDetailSync(const char* accountID, const char* startDate, const char* endDate, XtError& error, const char* accountKey = "") = 0;

        /**
        * @brief 请求期货账号的保证金率
        * @note 调用此函数后，回调 onReqFtAccCommissionRateDetail
        * @param [in] accountID 账号ID
        * @param [in] exchangeId 市场，取值参考MarketType.h
        * @param [in] instrumentId 合约代码
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqFtAccCommissionRateDetail(const char* accountID, const char* exchangeId, const char* instrumentId, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求期货账号的手续费率
        * @note 调用此函数后，回调 onReqFtAccMarginRateDetail
        * @param [in] accountID 账号ID
        * @param [in] exchangeId 市场，取值参考MarketType.h
        * @param [in] instrumentId 合约代码
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqFtAccMarginRateDetail(const char* accountID, const char* exchangeId, const char* instrumentId, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 获取用户下所有的产品Id
        * @note 调用此函数后，回调 onReqProductIds
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqProductIds(int nRequestId) = 0;

        /**
        * @brief 创建新投资组合
        * @note 回调 onCreatePortfolio
        * @param [in] newPortfolioReq 创建新投资组合请求参数
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void createPortfolio(const CNewPortfolioReq* newPortfolioReq, int nRequestId) = 0;

        /**
        * @brief 查询产品Id下所有的投资组合
        * @note 调用此函数后，回调 onReqProductPortfolio
        * @param [in] nProductID 产品ID,可以从onReqProductIds获取
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqProductPortfolio(int nProductID, int nRequestId) = 0;

        /**
        * @brief 请求投资组合委托信息
        * @note 调用此函数后，回调 onReqPortfolioOrder
        * @param [in] nPortfolioID 投资组合ID
        * @param [in] nDate 查询日期
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPortfolioOrder(int nPortfolioID, int nDate, int nRequestId) = 0;

        /**
        * @brief 请求投资组合一段时间内的委托信息
        * @note 调用此函数后，回调 onReqPortfolioMultiOrder
        * @param [in] nPortfolioID 投资组合ID
        * @param [in] startDate 查询开始日期
        * @param [in] endDate 查询结束日期
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPortfolioMultiOrder(int nPortfolioID, int startDate, int endDate, int nRequestId) = 0;

        /**
        * @brief 请求投资组合成交信息
        * @note 调用此函数后，回调 onReqPortfolioDeal
        * @param [in] nPortfolioID 投资组合ID
        * @param [in] nDate 查询日期
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPortfolioDeal(int nPortfolioID, int nDate, int nRequestId) = 0;

        /**
        * @brief 请求投资组合一段时间内的成交信息
        * @note 调用此函数后，回调 onReqPortfolioMultiDeal
        * @param [in] nPortfolioID 投资组合ID
        * @param [in] startDate 查询开始日期
        * @param [in] endDate 查询结束日期
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPortfolioMultiDeal(int nPortfolioID, int startDate, int endDate, int nRequestId) = 0;

        /**
        * @brief 请求投资组合持仓信息
        * @note 调用此函数后，回调 onReqPortfolioPosition
        * @param [in] nPortfolioID 投资组合ID
        * @param [in] nDate 查询日期
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void reqPortfolioPosition(int nPortfolioID, int nDate, int nRequestId) = 0;

        /**
        * @brief 请求收益互换账号框架号
        * @note 调用此函数后，回调 onReqStrategyInfo
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqStrategyInfo(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求股东号，用于多股东时指定股东号
        * @note 调用此函数后，回调 onReqSecuAccount
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqSecuAccount(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求资金账号状态
        * @note 调用此函数后，回调 onReqAccountStatus
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqAccountStatus(int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 请求资金账号状态
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual bool reqAccountStatusSync(const char* accountKey = "") = 0;

        /**
        * @brief 按指令号暂停指令并暂停任务
        * @note 暂停指令，暂停某个单子的运行，回调 onPause
        * @param [in] orderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void pause(int orderID, int nRequestId) = 0;

        /**
        * @brief 按指令号暂停指令并暂停任务
        * @note 暂停指令，暂停某个单子的运行，回调 onPause
        * @param [in] orderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        */
        virtual void pauseSync(int orderID, XtError& error) = 0;

        /**
        * @brief 按指令号恢复指令并恢复任务
        * @note 恢复指令，恢复某个单子的运行，回调 onResume
        * @param [in] orderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        */
        virtual void resume(int orderID, int nRequestId) = 0;

        /**
        * @brief 按指令号恢复指令并恢复任务
        * @note 恢复指令，恢复某个单子的运行，回调 onResume
        * @param [in] orderID 下单的指令号，可通过onRtnOrder和onOrder获得
        * @param [in] error 错误信息，用于接收同步请求时的错误信息
        */
        virtual void resumeSync(int orderID, XtError& error) = 0;

        /**
        * @brief 查询投资组合资金信息
        * @note 调用此函数后，回调onReqPortfolioStat
        * @param [in] nPortfolioIds  投资组合id
        * @param [in] nTradeDate  查询日期，如果需要查当天，nTradeDate送-1
        * @param [in] nRequestId  请求编号
        */
        virtual void reqPortfolioStats(std::vector<int>& nPortfolioIds, int nTradeDate, int nRequestId) = 0;

        /**
        * @brief 请求市场状态信息
        * @note 调用此函数后，回调 onReqExchangeStatus
        * @param [in] accountID 账号ID
        * @param [in] nRequestId 客户自己维护的请求顺序ID
        * @param [in] accountKey 账号Key，用于区分不同类型的账号ID相同的账号
        */
        virtual void reqExchangeStatus(const char* accountID, int nRequestId, const char* accountKey = "") = 0;

        /**
        * @brief 查询枚举名称
        * @note 调用此函数后，返回枚举的中文名称
        * @param [in] enumName 枚举名称，传入字符串
        * @param [in] enumItemValue 枚举值
        */
        virtual char* reqEnumItemName(const char* enumName, int enumItemValue) = 0;

        /**
        * @brief 出借人信息查询
        * @note 调用此函数后，回调onReqSvrFsmpslqLendacct
        * @param [in] reflenderCode  出借人代码
        * @param [in] nRequestId  请求编号
        */
        virtual void reqSvrFsmpslqLendacct(const char * reflenderCode, int nRequestId) = 0 ;

        /**
        * @brief 约定号查询
        * @note 调用此函数后，回调onReqSvrFsmpslpRefbreport
        * @param [in] req  约定号查询请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqSvrFsmpslpRefbreport(const CSvrFsmpslpRefbreportQryReq* req, int nRequestId) = 0;

        /**
        * @brief 融券通出借行情查询(借入方发起，查询返回的行情用于借入)
        * @note 调用此函数后，回调onReqAgentSvrFsmpslpHq
        * @param [in] req  agent融券通出借行情查询请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqAgentSvrFsmpslpHq(const CtAgentSvrFsmpslpHqQryReq* req, int nRequestId) = 0;

        /**
        * @brief 融券通出借行情查询(出借方发起，查询自己出借记录)
        * @note 调用此函数后，回调onReqSvrFsmpslpHq
        * @param [in] req 融券通出借行情查询请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqSvrFsmpslpHq(const CSvrFsmpslpHqQryReq* req, int nRequestId) = 0;

        /**
        * @brief 融券通出借行情发布
        * @note 调用此函数后，回调onReqSvrFsmpslpHqIssue
        * @param [in] req  融券通出借行情发布请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqSvrFsmpslpHqIssue(const CSvrFsmpslpHqIssue* req, int nRequestId) = 0;

        /**
        * @brief 融券通出借行情撤单
        * @note 调用此函数后，回调onReqFsmpslpHqCancel
        * @param [in] accountID  资金账号
        * @param [in] slphqNo  行情编号
        * @param [in] nRequestId  请求编号
        */
        virtual void reqFsmpslpHqCancel(const char* accountID, const char* slphqNo, int nRequestId) = 0;
    
        /**
        * @brief 融券通行情确认
        * @note 调用此函数后，回调onReqSvrFsmpslpHqComfirm
        * @param [in] req  融券通出借行情发布请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqSvrFsmpslpHqComfirm(const CSvrFsmpslpHqComfirm* req, int nRequestId) = 0;

        /**
        * @brief 券源预约
        * @note 调用此函数后，回调onReqAppointOrder
        * @param [in] req  券源预约请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqAppointOrder(const CAppointOrder* req, int nRequestId) = 0;

        /**
        * @brief 券源撤单
        * @note 调用此函数后，回调onReqAppointCancel
        * @param [in] req  券源撤单请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqAppointCancel(const CAppointCancel* req, int nRequestId) = 0;
        
        /**
        * @brief 议价申请
        * @note 调用此函数后，回调onReqNegotiateOrder
        * @param [in] req  议价申请请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqNegotiateOrder(const CNegotiateOrder* req, int nRequestId) = 0;

        /**
        * @brief 议价行情查询
        * @note 调用此函数后，回调onReqNegotiateQuoterDetail
        * @param [in] req  议价行情查询请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqNegotiateQuoterDetail(const CNegotiateQuoterDetailReq* req, int nRequestId) = 0;

        /**
        * @brief 约券合约查询
        * @note 调用此函数后，回调onReqCashcompact
        * @param [in] accountID  资金账户
        * @param [in] operateType  操作类型 1是非竞价行情查询2 是合约查询
        * @param [in] nRequestId  请求编号
        */
        virtual void reqCashcompact(const char* accountID, int operateType, int nRequestId) = 0;

        /**
        * @brief 券源归还申请
        * @note 调用此函数后，回调onReqReturnApply
        * @param [in] req 券源归还申请请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqReturnApply(const CReturnApply* req, int nRequestId) = 0;

        /**
        * @brief 券源可约券单查询
        * @note 调用此函数后，回调onReqStockSource
        * @param [in] accountID  资金账户
        * @param [in] nRequestId  请求编号
        */
        virtual void reqStockSource(const char* accountID, int nRequestId) = 0;

        /**
        * @brief 融券通申请
        * @note 调用此函数后，回调onReqExtFsmpslpApply
        * @param [in] req  融券通申请请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqExtFsmpslpApply(const CExtFsmpslpApply* req, int nRequestId) = 0;

        /**
        * @brief 对手方行情利率获取
        * @note 调用此函数后，回调onReqExtFsmpslpOpphqRate
        * @param [in] req 对手方行情利率获取请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqExtFsmpslpOpphqRate(const CExtFsmpslpOpphqRateReq* req, int nRequestId) = 0;

        /**
        * @brief 融券通借入行情发布
        * @note 调用此函数后，回调onReqExtFsmpslphqIssue
        * @param [in] req  融券通借入行情发布请求入参
        * @param [in] nRequestId  请求编号
        */
        virtual void reqExtFsmpslphqIssue(const CExtFsmpslphqIssue* req, int nRequestId) = 0;

        /**
        * @brief 融券通借入行情撤单
        * @note 调用此函数后，回调onReqExtFsmpslpCancel
        * @param [in] accountID  资金账号
        * @param [in] slphqNo  行情编号
        * @param [in] nRequestId  请求编号
        */
        virtual void reqExtFsmpslpCancel(const char* accountID, const char* slphqNo, int nRequestId) = 0;

    };

    /**
    * @brief 交易api回调接口
    * @note 相应请求的回调函数
    * @param nRequestId 和请求函数中 nRequestId 想对应
    * @param error 请求函数的返回是否成功，如果失败有错误信息
    * @remark  函数名组成：以 on 为前缀，加上相应请求函数的函数名称（请求函数首字母大写）
    */
    class XT_API_EXPORT XtTraderApiCallback
    {
    public:
        XtTraderApiCallback(){}
        virtual ~XtTraderApiCallback(){}

        /**
        * @brief 连接服务器的回调函数
        * @param [out] success 服务器连接是否成功
        * @param [out] errorMsg 如果服务器连接失败，存储错误信息
        */
        virtual void onConnected(bool success, const char* errorMsg) {}

        /**
        * @brief 用户登录的回调函数
        * @param [out] userName 用户名
        * @param [out] password 用户密码
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含登陆失败原因
        */
        virtual void onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error) {}

        /**
        * @brief 用户登出的回调函数
        * @param [out] userName 用户名
        * @param [out] password 用户密码
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含登陆失败原因
        */
        virtual void onUserLogout(const char* userName, const char* password, int nRequestId, const XtError& error) {}

        /**
        * @brief 下单的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] orderID 指令号
        * @param [out] strRemark 下单时填写的投资备注
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含下单失败原因
        */
        virtual void onOrder(int nRequestId, int orderID, const char* strRemark, const XtError& error) {}

        /**
        * @brief 直接下单的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] strOrderSysID 委托号
        * @param [out] strRemark 下单时填写的投资备注
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含下单失败原因
        */
        virtual void onDirectOrder(int nRequestId, const char* strOrderSysID, const char* strRemark, const XtError& error) {}

        /**
        * @brief 撤指令的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含撤单失败原因，建议用下面带投资备注的onCancelWithRemark，该接口逐渐废弃
        */
        virtual void onCancel(int nRequestId, const XtError& error) {}

        /**
        * @brief 撤指令的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] strRemark 下单时填写的投资备注，如果撤销的指令号不存在返回空
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含撤单失败原因
        */
        virtual void onCancelWithRemark(int nRequestId, const char* strRemark, const XtError& error) {}

        /**
        * @brief 撤委托的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含撤单失败原因
        */
        virtual void onCancelOrder(int nRequestId,  const XtError& error) {}

        /**
        * @brief 风险试算的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 风险试算结果
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含撤单失败原因
        */
        virtual void onCheck(int nRequestId ,const CCheckData* data , const XtError& error) {}

        /**
        * @brief 请求账号资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqAccountDetail(const char* accountID, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号资金数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqAccountDetail(const char* accountID, int nRequestId, std::vector<CAccountDetail>* data, bool isLast, const XtError& error) {}
        /**
        * @brief 请求账号资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqAccountDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CAccountDetail* data, bool isLast, const XtError& error) {}
        /**
        * @brief 请求账号资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号资金数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqAccountDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CAccountDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号两融资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号两融资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditAccountDetail(const char* accountID, int nRequestId, const CCreditAccountDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号两融资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号两融资金数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditAccountDetail(const char* accountID, int nRequestId, std::vector<CCreditAccountDetail>* data, bool isLast, const XtError& error) {}
        
        /**
        * @brief 请求账号两融资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号两融资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditAccountDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CCreditAccountDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号两融资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号两融资金数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditAccountDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CCreditAccountDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqOrderDetail(const char* accountID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据，数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqOrderDetail(const char* accountID, int nRequestId, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}
        /**
        * @brief 请求账号委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqOrderDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号委托明细数据，数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqOrderDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqDealDetail(const char* accountID, int nRequestId, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号成交明细数据 ，数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqDealDetail(const char* accountID, int nRequestId, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqDealDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号成交明细数据 ，数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqDealDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号持仓明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPositionDetail(const char* accountID, int nRequestId, const CPositionDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号持仓明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓明细数据，数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPositionDetail(const char* accountID, int nRequestId, std::vector <CPositionDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号持仓明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号持仓明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPositionDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CPositionDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号持仓明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号持仓明细数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPositionDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CPositionDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPositionStatics(const char* accountID, int nRequestId, const CPositionStatics* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓统计数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPositionStatics(const char* accountID, int nRequestId, std::vector<CPositionStatics>* data, bool isLast, const XtError& error) {}
        /**
        * @brief 请求账号持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPositionStaticsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CPositionStatics* data, bool isLast, const XtError& error) {}

        /**
       * @brief 请求账号持仓统计的回调函数
       * @param [out] accountID 账号ID
       * @param [out] nRequestId 客户自己维护的请求顺序ID
       * @param [out] accountKey 账户key
       * @param [out] data 账号持仓统计数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
       * @param [out] error 错误信息
       * @attention 当error返回报错时，data内容不可用
       */
        virtual void onBatchReqPositionStaticsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CPositionStatics>*data, bool isLast, const XtError& error) {}


        /**
        * @brief 请求账号日初持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号持仓统计数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqInitialPositionStatics(const char* accountID, int nRequestId, const char* accountKey, std::vector<CPositionStatics>*data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号投资组合可用持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号投资组合可用持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqRevolvePositions(const char* accountID, int nRequestId, const CPFPositionStatics* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号投资组合可用持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号投资组合可用持仓统计数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqRevolvePositions(const char* accountID, int nRequestId, std::vector<CPFPositionStatics>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号投资组合可用持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号投资组合可用持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqRevolvePositionsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CPFPositionStatics* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号投资组合可用持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 账号投资组合可用持仓统计数据 ，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqRevolvePositionsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CPFPositionStatics>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 两融账号标的数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStksubjects(const char* accountID, int nRequestId, const CStkSubjects* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 两融账号标的数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStksubjects(const char* accountID, int nRequestId,  std::vector<CStkSubjects>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 两融账号标的数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStksubjectsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStkSubjects* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 两融账号标的数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStksubjectsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStkSubjects>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号负债的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 两融账号负债数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkcompacts(const char* accountID, int nRequestId, const CStkCompacts* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号负债的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 两融账号负债数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStkcompacts(const char* accountID, int nRequestId, std::vector<CStkCompacts>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号负债的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 两融账号负债数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkcompactsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStkCompacts* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号负债的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 两融账号负债数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStkcompactsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStkCompacts>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期权账号备兑持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 期权账号备兑持仓数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCoveredStockPosition(const char* accountID, int nRequestId, const CCoveredStockPosition* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期权账号备兑持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 期权账号备兑持仓数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCoveredStockPosition(const char* accountID, int nRequestId, std::vector<CCoveredStockPosition>* data, bool isLast, const XtError& error) {}
        /**
        * @brief 请求期权账号备兑持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 期权账号备兑持仓数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCoveredStockPositionWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CCoveredStockPosition* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期权账号备兑持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 期权账号备兑持仓数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCoveredStockPositionWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CCoveredStockPosition>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求产品信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 产品信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqProductData(int nRequestId, const CProductData* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求产品信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 产品信息数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqProductData(int nRequestId, std::vector<CProductData>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求合约数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 合约数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCInstrumentDetail(const char* accountID, int nRequestId, const CInstrumentDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求合约数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 合约数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCInstrumentDetail(const char* accountID, int nRequestId, std::vector<CInstrumentDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求合约数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 合约数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCInstrumentDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CInstrumentDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求合约数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 合约数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCInstrumentDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CInstrumentDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求行情数据的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 行情数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPriceData(int nRequestId, const CPriceData* data, const XtError& error) {}

        /**
        * @brief 请求期权账号组合持仓数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 期权账号组合持仓数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkOptCombPositionDetail(const char* accountID, int nRequestId, const CStockOptionCombPositionDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期权账号组合持仓数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 期权账号组合持仓数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStkOptCombPositionDetail(const char* accountID, int nRequestId, std::vector<CStockOptionCombPositionDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期权账号组合持仓数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 期权账号组合持仓数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkOptCombPositionDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStockOptionCombPositionDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期权账号组合持仓数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 期权账号组合持仓数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStkOptCombPositionDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStockOptionCombPositionDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 行情订阅的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 行情订阅数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onSubscribQuote(int nRequestId, const CSubscribData* data, const XtError& error) {}

        /**
        * @brief 行情退订的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 行情订阅数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onUnSubscribQuote(int nRequestId, const CSubscribData* data, const XtError& error) {}

        /**
        * @brief 请求港股账号汇率数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 港股账号汇率数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqReferenceRate(const char* accountID, int nRequestId, const CReferenceRate* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求港股账号汇率数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 港股账号汇率数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqReferenceRate(const char* accountID, int nRequestId, std::vector<CReferenceRate>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求港股账号汇率数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 港股账号汇率数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqReferenceRateWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CReferenceRate* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求港股账号汇率数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 港股账号汇率数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqReferenceRateWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CReferenceRate>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号两融综合资金数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 两融账号两融综合资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditDetail(const char* accountID, int nRequestId, const CCreditDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号两融综合资金数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 两融账号两融综合资金数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditDetail(const char* accountID, int nRequestId, std::vector<CCreditDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号两融综合资金数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 两融账号两融综合资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CCreditDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号两融综合资金数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 两融账号两融综合资金数据，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CCreditDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 主推的用户登录状态
        * @param [out] accountID 账号ID
        * @param [out] status 主推资金账号的登录状态
        * @param [out] brokerType 主推资金账号的类型
        * @param [out] errorMsg 错误信息
        * @note brokerType取值1:期货账号, 2:股票账号, 3:信用账号, 4:贵金属账号, 5:期货期权账号, 6:股票期权账号, 7:沪港通账号, 10:全国股转账号
        */
        virtual void onRtnLoginStatus(const char* accountID, EBrokerLoginStatus status, int brokerType, const char* errorMsg) {}

        /**
        * @brief 主推的用户登录状态
        * @param [out] accountID 账号ID
        * @param [out] status 主推资金账号的登录状态
        * @param [out] brokerType 主推资金账号的类型
        * @param [out] accountKey 账号 key，用于区分不同类型的相同账号ID的账号
        * @param [out] errorMsg 错误信息
        * @note brokerType取值1:期货账号, 2:股票账号, 3:信用账号, 4:贵金属账号, 5:期货期权账号, 6:股票期权账号, 7:沪港通账号, 10:全国股转账号
        */
        virtual void onRtnLoginStatusWithActKey(const char* accountID, EBrokerLoginStatus status, int brokerType, const char* accountKey, const char* errorMsg) {}

        /**
        * @brief 主推的用户登录状态
        * @param [out] accountID 账号ID
        * @param [out] status 主推资金账号的登录状态
        * @param [out] brokerType 主推资金账号的类型
        * @param [out] accountKey 账号 key，用于区分不同类型的相同账号ID的账号
        * @param [out] userName 用户名
        * @param [out] errorMsg 错误信息
        * @note brokerType取值1:期货账号, 2:股票账号, 3:信用账号, 4:贵金属账号, 5:期货期权账号, 6:股票期权账号, 7:沪港通账号, 10:全国股转账号
        */
        virtual void onRtnLoginStatusCustom(const char* accountID, EBrokerLoginStatus status, int brokerType, const char* accountKey, const char* userName, const char* errorMsg) {}

        /**
        * @brief 主推的报单状态（指令）
        * @param [out] data 下单指令信息
        */
        virtual void onRtnOrder(const COrderInfo* data){}
        
        /**
        * @brief 主推的指令统计信息（指令）
        * @param [out] data 指令统计信息
        */
        virtual void onRtnOrderStat(const COrderStat* data){}

        /**
        * @brief 主推的委托明细（委托）
        * @param [out] data 委托明细信息
        */
        virtual void onRtnOrderDetail(const COrderDetail* data) {}

        /**
        * @brief 主推的成交明细
        * @param [out] data 成交明细信息
        */
        virtual void onRtnDealDetail(const CDealDetail* data) {}

        /**
        * @brief 主推的委托错误信息
        * @param [out] data 下单失败的错误信息
        */
        virtual void onRtnOrderError(const COrderError* data) {}

        /**
        * @brief 主推的撤单失败信息
        * @param [out] data 撤单失败的错误信息
        */
        virtual void onRtnCancelError(const CCancelError* data) {}

        /**
        * @brief 主推的资金账号信息
        * @param [out] accountID 账号ID
        * @param [out] data 账号资金信息
        */
        virtual void onRtnAccountDetail(const char* accountID, const CAccountDetail* data) {}

        /**
        * @brief 主推的两融资金账号信息
        * @param [out] accountID 账号ID
        * @param [out] data 账号两融资金信息
        */
        virtual void onRtnCreditAccountDetail(const char* accountID, const CCreditAccountDetail* data) {}

        /**
        * @brief 主推的产品净值信息
        * @param [out] data 产品净值信息
        */
        virtual void onRtnNetValue(const CNetValue* data){}

        /**
        * @brief 主推行情数据
        * @param [out] data 行情数据
        */
        virtual void onRtnPriceData(const CPriceData& data){};

        /**
        * @brief 用户自己的定时器回调
        */
        virtual void onCustomTimer(){};

        /**
        * @brief 用户自己的定时器回调
        * @param [out] timerName 定时器名称
        */
        virtual void onNamedTimer(const char* timerName) {};

        /**
        * @brief 暂停恢复任务回调
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号 key，用于区分不同类型的相同账号ID的账号
        * @param [out] error 错误信息，暂停恢复失败时包含错误信息
        */
        virtual void onOperateTask(const char* accountID, int nRequestId, const char* accountKey, const XtError& error){};

        /**
        * @brief 指令改参的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] orderID 指令号
        * @param [out] strRemark 下单时填写的投资备注
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含下单失败原因
        */
        virtual void onModifyAlgoCommands(int nRequestId, int orderID, const char* strRemark, const XtError& error) {}

        /**
        * @brief 请求新股额度数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 新股信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSubscribeInfo(const char* accountID, int nRequestId, const CSubscribeInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求新股额度数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 新股询价额度信息数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqEnquirySubscribeInfo(const char* accountID, int nRequestId, const char* accountKey, std::vector<CEnquirySubscribeInfo>* data, const XtError& error) {}

        /**
        * @brief 请求新股额度数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 新股信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqSubscribeInfo(const char* accountID, int nRequestId,  std::vector<CSubscribeInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求新股额度数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 新股信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSubscribeInfoWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CSubscribeInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求新股额度数据的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 新股信息数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqSubscribeInfoWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CSubscribeInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求未了结负债信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 未了结负债信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkUnCloseCompact(const char* accountID, int nRequestId, const CStkUnClosedCompacts* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求未了结负债信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 未了结负债信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStkUnCloseCompact(const char* accountID, int nRequestId, std::vector<CStkUnClosedCompacts>* data, bool isLast, const XtError& error) {}


        /**
        * @brief 请求未了结负债信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 未了结负债信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkUnCloseCompactWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStkUnClosedCompacts* data, bool isLast, const XtError& error) {}

        /**
       * @brief 请求未了结负债信息的回调函数
       * @param [out] accountID 账号ID
       * @param [out] nRequestId 客户自己维护的请求顺序ID
       * @param [out] accountKey 账户key
       * @param [out] data 未了结负债信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
       * @param [out] error 错误信息
       * @attention 当error返回报错时，data内容不可用
       */
        virtual void onBatchReqStkUnCloseCompactWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStkUnClosedCompacts>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求已了结负债信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 已了结负债信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkClosedCompact(const char* accountID, int nRequestId, const CStkClosedCompacts* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求已了结负债信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 已了结负债信息数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStkClosedCompact(const char* accountID, int nRequestId, std::vector<CStkClosedCompacts>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求已了结负债信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] data 已了结负债信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStkClosedCompactWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStkClosedCompacts* data, bool isLast, const XtError& error) {}

       /**
       * @brief 请求已了结负债信息的回调函数
       * @param [out] accountID 账号ID
       * @param [out] nRequestId 客户自己维护的请求顺序ID
       * @param [out] accountKey 账户key
       * @param [out] data 已了结负债信息数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
       * @param [out] error 错误信息
       * @attention 当error返回报错时，data内容不可用
       */
        virtual void onBatchReqStkClosedCompactWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStkClosedCompacts>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 获取用户下所有账号key的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data accountKey数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqAccountKey(int nRequestId, const CAccountKey* data, bool isLast, const XtError& error) {}

        /**
        * @brief 获取用户下所有账号key的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data accountKey数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqAccountKey(int nRequestId, std::vector<CAccountKey>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 根据委托号请求账号成交明细信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] orderSyeId 委托号
        * @param [out] exchangeId 委托所属市场
        * @param [out] data 账号成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqDealDetailBySysID(const char* accountID, int nRequestId, const char* orderSyeId, const char* exchangeId, const CDealDetail* data, bool isLast, const XtError& error) {}
 
        /**
        * @brief 根据委托号请求账号成交明细信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] orderSyeId 委托号
        * @param [out] exchangeId 委托所属市场
        * @param [out] data 账号成交明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqDealDetailBySysID(const char* accountID, int nRequestId, const char* orderSyeId, const char* exchangeId, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 根据委托号请求账号成交明细信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] orderSyeId 委托号
        * @param [out] exchangeId 委托所属市场
        * @param [out] data 账号成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqDealDetailBySysIDWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const char* orderSyeId,  const char* exchangeId, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 根据委托号请求账号成交明细信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账户key
        * @param [out] orderSyeId 委托号
        * @param [out] exchangeId 委托所属市场
        * @param [out] data 账号成交明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqDealDetailBySysIDWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const char* orderSyeId, const char* exchangeId, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号结算单信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号结算单数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqDeliveryDetail(const char* accountID, int nRequestId, const CDeliveryDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号结算单信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号结算单数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqDeliveryDetail(const char* accountID, int nRequestId, std::vector<CDeliveryDetail>* data, bool isLast, const XtError& error) {}
        
        /**
        * @brief 请求账号结算单信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号结算单数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqDeliveryDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CDeliveryDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号结算单信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号结算单数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqDeliveryDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CDeliveryDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号合约行情信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 合约数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSingleInstrumentInfo(int nRequestId, const CInstrumentInfo* data, const XtError& error) {}

        /**
        * @brief 主推市场状态信息
        * @param [out] data 市场状态信息
        */
        virtual void onRtnExchangeStatus(const CExchangeStatus* data) {};

        /**
        * @brief 主推两融综合资金
        * @param [out] data 两融综合资金
        */
        virtual void onRtnCreditDetail(const CCreditDetail* data) {};

        /**
        * @brief 请求账号可下单量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] dataKey 数据标识，市场加合约
        * @param [out] nVolume 可下单量
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，nVolume内容不可用
        */
        virtual void onReqOpVolume(const char* accountID, int nRequestId, const char* dataKey, int nVolume, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号可下单量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] dataKey 数据标识，市场加合约
        * @param [out] nVolume 可下单量
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，nVolume内容不可用
        */
        virtual void onReqOpVolumeWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const char* dataKey, int nVolume, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号可下单量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] market 市场代码
        * @param [out] instrument 合约代码
        * @param [out] nVolume 可下单量
        * @param [out] error 错误信息
        * @attention 当error返回报错时，nVolume内容不可用
        */
        virtual void onReqCanOrderVolume(const char* accountID, int nRequestId, const char* accountKey, const char* market, const char* instrument, int nVolume, const XtError& error) {}

        /**
        * @brief 请求两融账号融券可融数量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 融券可融数量数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditSloCode(const char* accountID, int nRequestId, const CCreditSloCode* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号融券可融数量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 融券可融数量数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditSloCode(const char* accountID, int nRequestId, std::vector<CCreditSloCode>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号融券可融数量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 融券可融数量数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditSloCodeWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CCreditSloCode* data, bool isLast, const XtError& error) {}
                  
        /**
        * @brief 请求两融账号融券可融数量的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 融券可融数量数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditSloCodeWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CCreditSloCode>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号融资融券标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 融资融券标的数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditSubjects(const char* accountID, int nRequestId, const CCreditSubjects* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号融资融券标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 融资融券标的数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditSubjects(const char* accountID, int nRequestId, std::vector<CCreditSubjects>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号融资融券标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 融资融券标的数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditSubjectsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CCreditSubjects* data, bool isLast, const XtError& error) {}

        /**
       * @brief 请求两融账号融资融券标的的回调函数
       * @param [out] accountID 账号ID
       * @param [out] nRequestId 客户自己维护的请求顺序ID
       * @param [out] accountKey 账号key
       * @param [out] data 融资融券标的数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
       * @param [out] error 错误信息
       * @attention 当error返回报错时，data内容不可用
       */
        virtual void onBatchReqCreditSubjectsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CCreditSubjects>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号担保标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 担保标的数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditAssure(const char* accountID, int nRequestId, const CCreditAssure* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号担保标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 担保标的数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditAssure(const char* accountID, int nRequestId, std::vector<CCreditAssure>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求两融账号担保标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 担保标的数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCreditAssureWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CCreditAssure* data, bool isLast, const XtError& error) {}


        /**
        * @brief 请求两融账号担保标的的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 担保标的数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCreditAssureWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CCreditAssure>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号银证转账银行信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqTransferBank(const char* accountID, int nRequestId, const CQueryBankInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号银证转账银行信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqTransferBank(const char* accountID, int nRequestId, std::vector<CQueryBankInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号银证转账银行信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqTransferBankWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CQueryBankInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行信息的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号银证转账银行信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqTransferBankWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CQueryBankInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行流水的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号银证转账银行流水数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqTransferSerial(const char* accountID, int nRequestId, const CTransferSerial* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行流水的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号银证转账银行流水数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqTransferSerial(const char* accountID, int nRequestId, std::vector<CTransferSerial>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行流水的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号银证转账银行流水数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqTransferSerialWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CTransferSerial* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行流水的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号银证转账银行流水数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqTransferSerialWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CTransferSerial>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行余额的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号银证转账银行余额数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqBankAmount(const char* accountID, int nRequestId, const CQueryBankAmount* data, const XtError& error) {}

        /**
        * @brief 请求账号银证转账银行余额的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号银证转账银行余额数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqBankAmountWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CQueryBankAmount* data, const XtError& error) {}

        /**
        * @brief 银证转账的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含转账失败原因
        */
        virtual void onTransfer(int nRequestId, const XtError& error) {}

        /**
        * @brief 按市场请求合约信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 合约信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqInstrumentInfoByMarket(int nRequestId, const CInstrumentInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 按市场请求合约信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 合约信息数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqInstrumentInfoByMarket(int nRequestId, std::vector<CInstrumentInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 按市场请求合约信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] exchangeId 委托所属市场
        * @param [out] data 合约信息数据
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqInstrumentInfoByMarketWithMkt(int nRequestId, const char* exchangeId, const CInstrumentInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 按市场请求合约信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] exchangeId 委托所属市场
        * @param [out] data 合约信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqInstrumentInfoByMarketWithMkt(int nRequestId, const char* exchangeId, std::vector<CInstrumentInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号可撤单委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCanCancelOrderDetail(const char* accountID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号可撤单委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCanCancelOrderDetail(const char* accountID, int nRequestId, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号可撤单委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCanCancelOrderDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号可撤单委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号委托明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCanCancelOrderDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}


        /**
        * @brief 请求所有下单指令信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 下单指令信息数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqCommandsInfo(int nRequestId, const COrderInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求所有下单指令信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 下单指令信息数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqCommandsInfo(int nRequestId, std::vector<COrderInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 资金划拨的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含转账失败原因
        */
        virtual void onFundTransfer(int nRequestId, const XtError& error) {}

        /**
        * @brief 股份划拨的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含转账失败原因
        */
        virtual void onSecuTransfer(int nRequestId, const XtError& error) {}

        /**
        * @brief 请求账号账号普通柜台资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号资金划拨普通柜台资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqComFund(const char* accountID, int nRequestId, const CStockComFund* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号账号普通柜台资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号资金划拨普通柜台资金数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqComFund(const char* accountID, int nRequestId, std::vector<CStockComFund>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号账号普通柜台资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号资金划拨普通柜台资金数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqComFundWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStockComFund* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号账号普通柜台资金的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号资金划拨普通柜台资金数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqComFundWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStockComFund>* data, bool isLast, const XtError& error) {}


        /**
        * @brief 请求账号普通柜台持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号股份划拨普通柜台持仓数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqComPosition(const char* accountID, int nRequestId, const CStockComPosition* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号普通柜台持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号股份划拨普通柜台持仓数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqComPosition(const char* accountID, int nRequestId, std::vector<CStockComPosition>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号普通柜台持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号股份划拨普通柜台持仓数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqComPositionWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CStockComPosition* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号普通柜台持仓的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号股份划拨普通柜台持仓数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqComPositionWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStockComPosition>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 主推的算法母单错误信息
        * @param [out] nOrderID 指令ID
        * @param [out] strRemark 下单时填写的投资备注
        * @param [out] error 错误信息
        */
        virtual void onRtnAlgoError(int nOrderID, const char* strRemark, const XtError& error) {}

        /**
        * @brief 主推风控信息
        * @param [out] accountID 账号ID
        * @param [out] accountKey 账号Key
        * @param [out] strMsg 风控信息
        */
        virtual void onRtnRCMsg(const char* accountID, const char* accountKey, const char* strMsg) {}

        /**
        * @brief 请求当前交易日的回调函数
        * @param [out] tradeDay 当前交易日
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] error 错误信息
        */
        virtual void onReqTradeDay(const char* tradeDay, int nRequestId, const XtError& error) {}

        /**
        * @brief 请求账号历史委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqHistoryOrderDetail(const char* accountID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqHistoryOrderDetail(const char* accountID, int nRequestId, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqHistoryOrderDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史委托明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号Key
        * @param [out] data 账号委托明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqHistoryOrderDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号历史成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqHistoryDealDetail(const char* accountID, int nRequestId, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号历史成交明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqHistoryDealDetail(const char* accountID, int nRequestId, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号历史成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqHistoryDealDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史成交明细的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号历史成交明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqHistoryDealDetailWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqHistoryPositionStatics(const char* accountID, int nRequestId, const CPositionStatics* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓统计数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqHistoryPositionStatics(const char* accountID, int nRequestId, std::vector<CPositionStatics>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqHistoryPositionStaticsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, const CPositionStatics* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号历史持仓统计的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 账号持仓统计数据 , 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqHistoryPositionStaticsWithAccKey(const char* accountID, int nRequestId, const char* accountKey, std::vector<CPositionStatics>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求期货账号保证金率的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 保证金率
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqFtAccCommissionRateDetail(const char* accountID, int nRequestId, const char* accountKey, const CCommissionRateDetail* data, const XtError& error) {};

        /**
        * @brief 请求期货账号手续费率的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 手续费率
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqFtAccMarginRateDetail(const char* accountID, int nRequestId, const char* accountKey, const CMarginRateDetail* data, bool isLast, const XtError& error) {};

        /**
        * @brief 获取用户下所有的产品Id的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] nProductID 产品Id
        * @param [out] accountKey 账号key
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        */
        virtual void onReqProductIds(int nRequestId, int nProductID, const char* accountKey, bool isLast) {}

        /**
        * @brief 创建新投资组合的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] nPortfolioID 投资组合编号
        * @param [out] strRemark 下单时填写的投资备注
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含下单失败原因
        */
        virtual void onCreatePortfolio(int nRequestId, int nPortfolioID, const char* strRemark, const XtError& error) {}

        /**
        * @brief 查询产品Id下所有的投资组合的回调函数
        * @param [out] nProductID 产品ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 投资组合数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqProductPortfolio(int nProductID, int nRequestId, const CPortfolioInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 查询产品Id下所有的投资组合的回调函数
        * @param [out] nProductID 产品ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 投资组合数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqProductPortfolio(int nProductID, int nRequestId, std::vector<CPortfolioInfo>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合委托信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPortfolioOrder(int nPortfolioID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合委托信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPortfolioOrder(int nPortfolioID, int nRequestId, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}
        
        /**
        * @brief 请求投资组合一段时间内的委托信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPortfolioMultiOrder(int nPortfolioID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合一段时间内的委托信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号委托明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPortfolioMultiOrder(int nPortfolioID, int nRequestId, std::vector<COrderDetail>* data, bool isLast, const XtError& error) {}
        
        /**
        * @brief 请求账号成交明细的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPortfolioDeal(int nPortfolioID, int nRequestId, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求账号成交明细的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号成交明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPortfolioDeal(int nPortfolioID, int nRequestId, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合一段时间内的成交信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号成交明细数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPortfolioMultiDeal(int nPortfolioID, int nRequestId, const CDealDetail* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合一段时间内的成交信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号成交明细数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPortfolioMultiDeal(int nPortfolioID, int nRequestId, std::vector<CDealDetail>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合持仓信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓统计数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqPortfolioPosition(int nPortfolioID, int nRequestId, const CPositionStatics* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求投资组合持仓信息的回调函数
        * @param [out] nPortfolioID 投资组合ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 账号持仓统计数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqPortfolioPosition(int nPortfolioID, int nRequestId, std::vector<CPositionStatics>* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求收益互换账号框架号的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 收益互换账号框架号数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqStrategyInfo(const char* accountID, int nRequestId, const char* accountKey, const CStrategyInfo* data, bool isLast, const XtError& error) {}

        /**
        * @brief 请求收益互换账号框架号的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 收益互换账号框架号数据, 数据可能有多条，一次最大返回设置返回条数，数据超过设置数量多次返回
        * @param [out] isLast 请求数据可能超过单次最大返回条数，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqStrategyInfo(const char* accountID, int nRequestId, const char* accountKey, std::vector<CStrategyInfo>* data, bool isLast, const XtError& error) {}
        
        /**
        * @brief 请求股东号的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 股东号数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSecuAccount(const char* accountID, int nRequestId, const char* accountKey, const CSecuAccount* data, bool isLast, const XtError& error) {}
        
        /**
        * @brief 请求股东号的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 股东号数据
        * @param [out] isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onBatchReqSecuAccount(const char* accountID, int nRequestId, const char* accountKey, std::vector<CSecuAccount>* data, bool isLast, const XtError& error) {}


        /**
        * @brief 请求资金账号状态的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] status 账号状态
        */
        virtual void onReqAccountStatus(int nRequestId, const char* accountKey, bool status) {}

        /**
        * @brief 暂停指令的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] strRemark 下单时填写的投资备注，如果暂停的指令号不存在返回空
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含撤单失败原因
        */
        virtual void onPause(int nRequestId, const char* strRemark, const XtError& error) {}

        /**
        * @brief 恢复指令的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] strRemark 下单时填写的投资备注，如果恢复的指令号不存在返回空
        * @param [out] error 错误信息
        * @attention 当error返回报错时，error.errorMsg()包含撤单失败原因
        */
        virtual void onResume(int nRequestId, const char* strRemark, const XtError& error) {}
        /**
        * @brief 根据投资组合编号请求投资组合资金信息回调接口
        * @param [out] nRequestId 请求编号 与reqPortfolioStats接口入参的nRequestId对应
        * @param [out] nPortfolioId 投资组合编号 与reqPortfolioStats接口入参的nPortfolioId对应
        * @param [out] data 投资组合资金信息 具体参见XtStruct.h中CPortfolioStat结构定义
        * @param [out] isLast 是否是最后一条，请求的信息可能返回多条，true表示最后一条数据
        * @param [out] error 请求过程中返回的错误信息，XtError 见XtError.h定义
        * @attention 当error返回报错时，error.errorMsg()包含查询失败原因
        */
        virtual void onReqPortfolioStat(int nRequestId, int nPortfolioId, const CPortfolioStat* data, bool isLast, const XtError& error) {}


        /**
        * @brief 市场状态信息回调
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] accountKey 账号key
        * @param [out] data 市场状态信息 具体参见XtStruct.h中CExchangeStatus结构定义
        * @param [out] error 请求过程中返回的错误信息，XtError 见XtError.h定义
        * @attention 当error返回报错时，error.errorMsg()包含查询失败原因
        */
        virtual void onReqExchangeStatus(const char* accountID, int nRequestId, const char* accountKey, std::vector<CExchangeStatus>* data, const XtError& error) {}

        /**
        * @brief 查询出借人信息的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data 出借人信息
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSvrFsmpslqLendacct(int nRequestId, std::vector<CSvrFsmpslqLendacct>* data, const XtError& error) {}

        /**
        * @brief 约定号查询的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  约定号查询返回数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSvrFsmpslpRefbreport(int nRequestId, std::vector<CSvrFsmpslpRefbreportQryResp>* data, const XtError& error) {}

        /**
        * @brief agent融券通出借行情查询的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  agent融券通出借行情查询返回数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqAgentSvrFsmpslpHq(int nRequestId, std::vector<CtAgentSvrFsmpslpHqQryResp>* data, const XtError& error) {}

        /**
        * @brief 融券通出借行情查询的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  融券通出借行情查询返回数据
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data内容不可用
        */
        virtual void onReqSvrFsmpslpHq(const char* accountID, int nRequestId, std::vector<CSvrFsmpslpHqQryResp>* data, const XtError& error) {}
        /**
        * @brief融券通出借行情发布的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqSvrFsmpslpHqIssue(int nRequestId, const char* entrustNo, const XtError& error) {}

        /**
        * @brief 融券通出借撤单的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqFsmpslpHqCancel(int nRequestId, const char* entrustNo, const XtError& error) {}
        /**
        * @brief 融券通行情确认的回调函数
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        
        virtual void onReqSvrFsmpslpHqComfirm(int nRequestId, const char* entrustNo, const XtError& error) {}
        /**
        * @brief 券源预约的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqAppointOrder(const char* accountID, int nRequestId, const char* entrustNo, const XtError& error) {}

        /**
        * @brief 券源撤单的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqAppointCancel(const char* accountID, int nRequestId, const char* entrustNo, const XtError& error) {}

        /**
        * @brief 议价申请的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqNegotiateOrder(const char* accountID, int nRequestId, const char* entrustNo, const XtError& error) {}

        /**
        * @brief 议价行情查询的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  议价行情查询返回结构
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data不可用
        */
        virtual void onReqNegotiateQuoterDetail(const char* accountID, int nRequestId, std::vector<CNegotiateQuoterDetailResp>* data, const XtError& error) {}

        /**
        * @brief 约券合约查询的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  约券合约查询返回结构
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data不可用
        */
        virtual void onReqCashcompact(const char* accountID, int nRequestId, std::vector<CCashcompact>* data, const XtError& error) {}

        /**
        * @brief 券源归还申请的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  券源归还申请返回结构
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data不可用
        */
        virtual void onReqReturnApply(const char* accountID, int nRequestId, std::vector<CReturnApplyResp>* data, const XtError& error) {}

        /**
        * @brief 券源可约券单查询的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  券源可约券单查询返回结构
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data不可用
        */
        virtual void onReqStockSource(const char* accountID, int nRequestId, std::vector<CStockSource>* data, const XtError& error) {}

        /**
        * @brief 融券通申请的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqExtFsmpslpApply(const char* accountID, int nRequestId, const char* entrustNo, const XtError& error) {}

        /**
        * @brief 对手方行情利率获取返回的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] data  对手方行情利率获取返回结构
        * @param [out] error 错误信息
        * @attention 当error返回报错时，data不可用
        */
        virtual void onReqExtFsmpslpOpphqRate(const char* accountID, int nRequestId, std::vector<CExtFsmpslpOpphqRateResp>* data, const XtError& error) {}

        /**
        * @brief融券通借入行情发布的回调函数
        * @param [out] accountID 账号ID
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqExtFsmpslphqIssue(const char* accountID, int nRequestId, const char* entrustNo, const XtError& error) {}
        
        /**
        * @brief 融券通借入行情撤单的回调函数
        * @param [out] accountID 账号ID
        * @param [out] nRequestId 客户自己维护的请求顺序ID
        * @param [out] entrustNo  委托编号
        * @param [out] error 错误信息
        * @attention 当error返回报错时，entrustNo不可用
        */
        virtual void onReqExtFsmpslpCancel(const char* accountID, int nRequestId, const char* entrustNo, const XtError& error) {}

    };
}

#endif
