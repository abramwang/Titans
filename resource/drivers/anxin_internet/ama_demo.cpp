#include <string.h>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <ama.h>
#include <ama_tools.h>

#ifdef _WIN32
#pragma warning (disable:4996)
#endif
        
static std::mutex g_mutex;  //同步cout输出,回调的线程模型请参看开发指南
// 继承IAMDSpi,实现自己的回调处理类
class MySpi : public amd::ama::IAMDSpi
{
public:
    // 定义日志回调处理方法
    virtual void OnLog(const int32_t& level,
                       const char* log,
                       uint32_t len) override
    {
        std::lock_guard<std::mutex> _(g_mutex);
        std::cout << "AMA log: " << "    level: " << level << "    log:   " << log << std::endl;
    }

    // 定义监控回调处理方法
    virtual void OnIndicator(const char* indicator,
                             uint32_t len) override
    {
        std::lock_guard<std::mutex> _(g_mutex);
        std::cout << "AMA indicator: " << indicator << std::endl;
    }

    // 定义事件回调处理方法  level 对照 EventLevel 数据类型 code 对照 EventCode 数据类型
    virtual void OnEvent(uint32_t level, uint32_t code, const char* event_msg, uint32_t len)
    {
        std::cout << "AMA event: " << event_msg << std::endl;
    }

    // 定义快照数据回调处理方法
    virtual void OnMDSnapshot(amd::ama::MDSnapshot* snapshot,
                              uint32_t cnt) override
    {
        for (uint32_t i = 0; i < cnt; ++i)
        {
            std::lock_guard<std::mutex> _(g_mutex);
            std::cout << amd::ama::Tools::Serialize(snapshot[i]) << std::endl;
        }
        
        /* 手动释放数据内存, 否则会造成内存泄露 */
        amd::ama::IAMDApi::FreeMemory(snapshot);
    }

    //定义指数快照数据回调处理方法
    virtual void OnMDIndexSnapshot(amd::ama::MDIndexSnapshot* snapshots, uint32_t cnt)
    {

        for (uint32_t i = 0; i < cnt; ++i)
        {
            std::lock_guard<std::mutex> _(g_mutex);
            std::cout << amd::ama::Tools::Serialize(snapshots[i]) << std::endl;
        }
        
        /* 手动释放数据内存, 否则会造成内存泄露 */
        amd::ama::IAMDApi::FreeMemory(snapshots);
    }

    //定义逐笔委托数据回调处理方法
    virtual void OnMDTickOrder(amd::ama::MDTickOrder* ticks, uint32_t cnt)
    {

        for (uint32_t i = 0; i < cnt; ++i)
        {
            std::lock_guard<std::mutex> _(g_mutex);
            std::cout << amd::ama::Tools::Serialize(ticks[i]) << std::endl;
        }
        
        /* 手动释放数据内存, 否则会造成内存泄露 */
        amd::ama::IAMDApi::FreeMemory(ticks);
    }

    // 定义逐笔成交数据回调处理方法
    virtual void OnMDTickExecution(amd::ama::MDTickExecution* tick, uint32_t cnt) override
    {
        for (uint32_t i = 0; i < cnt; ++i)
        {
            std::lock_guard<std::mutex> _(g_mutex);
            std::cout << amd::ama::Tools::Serialize(tick[i]) << std::endl;
        }

        /* 手动释放数据内存, 否则会造成内存泄露 */
        amd::ama::IAMDApi::FreeMemory(tick);
    }

    //定义委托簿数据回调处理方法(本地构建模式下非单线程递交, cfg.thread_num为递交委托簿数据线程数, 服务端推送模式下为单线程递交)
    virtual void OnMDOrderBook(std::vector<amd::ama::MDOrderBook>& order_book)
    {
        std::lock_guard<std::mutex> _(g_mutex);
        for(uint32_t i = 0; i< order_book.size(); i++)
        {
            std::cout << amd::ama::Tools::Serialize(order_book[i]) << std::endl;
        }
        //不需要手动释放内存
    }
};


// 数据权限订阅示例
int32_t SubscribeWithDataAuth()
{
    /*
    按数据权限订阅信息设置,详细信息参考对应版本开发指南:
    1. 订阅信息分三个维度:
        market: 设置订阅的市场
        flag：设置订阅的权限数据类型
        security_code：设置订阅的代码
    2. 订阅操作有三种:
        kSet 设置订阅, 覆盖所有订阅信息
        kAdd 增加订阅, 在前一个基础上增加订阅信息
        kDel 删除订阅, 在前一个基础上删除订阅信息
        kCancelAll 取消所有订阅信息
    */

    //创建2个存储订阅代码信息的subItems数组空间(数组空间在没有初始化的情况下,里面的值是不确定的,订阅上送时可能会导致订阅与预期不符)
    amd::ama::SubscribeItem subItems[2];
    memset(subItems, 0, sizeof(subItems));

    // 订阅信息1：订阅深交所全部证券的逐笔委托和逐笔成交(设置subItems数组的第0位内存空间)
    subItems[0].market = amd::ama::MarketType::kSZSE;
    subItems[0].flag = amd::ama::SubscribeDataType::kTickOrder
                | amd::ama::SubscribeDataType::kTickExecution;
    subItems[0].security_code[0] = '\0';

    // 订阅信息2：订阅上交所600000证券的快照(设置subItems数组的第0位内存空间)
    subItems[1].market = amd::ama::MarketType::kSSE;
    subItems[1].flag = amd::ama::SubscribeDataType::kSnapshot;
    strcpy(subItems[1].security_code, "600000");
    
    /*
        以kSet发起订阅,设置两条订阅信息(此前所有已设置好的订阅信息都将会被覆盖)
        因为发起订阅的数量为2,因此会上送subItems[0]和subItems[1]的订阅信息
        订阅结果：
            深交所全部证券的逐笔委托和逐笔成交 + 上交所600000证券的快照
    */
    int ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kSet, subItems, 2);
    if(ret != amd::ama::ErrorCode::kSuccess)
    {
        return -1;
    }

    //kAdd、kDel、kCancelAll订阅操作示例,可根据需求调用
    if(false)
    {
        //订阅信息3：订阅上交所600001证券的快照(设置subItems数组的第0位内存空间)
        subItems[0].market = amd::ama::MarketType::kSSE;
        subItems[0].flag = amd::ama::SubscribeDataType::kSnapshot;
        strcpy(subItems[0].security_code, "600001");

        /*
            以kAdd方式发起订阅,在已有基础上增加一条订阅信息
            因为发起订阅的数量为1,因此最终只会上送subItems[0]的订阅信息
            订阅结果：
                深交所全部证券的逐笔委托和逐笔成交 + 上交所600000证券的快照 + 上交所600001证券的快照
        */
        ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kAdd, subItems, 1);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }

        /*
            以kDel发起订阅,在已有基础上删除一条订阅信息
            因为发起订阅的数量为1,因此最终只会上送subItems[0]的订阅信息
            订阅结果：
                 深交所全部证券的逐笔委托和逐笔成交 + 上交所600000证券的快照
        */
        ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kDel, subItems, 1);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }

        /*
            以kCancelAll发起订阅,取消所有订阅信息
            订阅结果：
                无
        */
        ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kCancelAll, subItems, 2);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }
    }

    return 0;
}

// 品种订阅示例
int32_t SubscribeWithCategory()
{
    /*
    按品种类型订阅信息设置:
    1. 订阅信息分三个维度:
        market: 设置订阅的市场
        data_type: 设置订阅的证券数据类型
        category_type: 设置订阅的品种类型
        security_code: 设置订阅的代码
    2. 订阅操作有三种:
        kSet 设置订阅, 覆盖所有订阅信息
        kAdd 增加订阅, 在前一个基础上增加订阅信息
        kDel 删除订阅, 在前一个基础上删除订阅信息
        kCancelAll 取消所有订阅信息
    */

    //创建2个存储订阅代码信息的subItems数组空间(数组空间在没有初始化的情况下,里面的值是不确定的,订阅上送时可能会导致订阅与预期不符)
    amd::ama::SubscribeCategoryItem subItems[2];
    memset(subItems, 0, sizeof(subItems));

    // 订阅信息1：深交所全部证券代码的股票快照和逐笔成交(设置subItems数组的第0位内存空间)
    subItems[0].market = amd::ama::MarketType::kSZSE;
    subItems[0].data_type = amd::ama::SubscribeSecuDataType::kSnapshot
                | amd::ama::SubscribeSecuDataType::kTickExecution;
    subItems[0].category_type = amd::ama::SubscribeCategoryType::kStock;
    subItems[0].security_code[0] = '\0';

    // 订阅信息2：上交所全部证券代码的基金快照(设置subItems数组的第1位内存空间)
    subItems[1].market = amd::ama::MarketType::kSSE;
    subItems[1].data_type = amd::ama::SubscribeSecuDataType::kSnapshot;
    subItems[1].category_type = amd::ama::SubscribeCategoryType::kFund;
    subItems[1].security_code[0] = '\0';
    
    /*
        以kSet发起订阅,设置两条订阅信息(此前所有已设置好的订阅信息都将会被覆盖)
        因为发起订阅的数量为2,因此会上送subItems[0]和subItems[1]的订阅信息
        订阅结果：
            深交所全部证券代码的股票快照和逐笔成交 + 上交所全部证券代码的基金快照
    */
    int ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kSet, subItems, 2);
    if(ret != amd::ama::ErrorCode::kSuccess)
    {
        return -1;
    }

    //kAdd、kDel、kCancelAll订阅操作示例,可根据需求调用
    if(false)
    {
        // 订阅信息3：上交所全部证券代码的债券快照(设置subItems数组的第0位内存空间)
        subItems[0].market = amd::ama::MarketType::kSSE;
        subItems[0].data_type = amd::ama::SubscribeSecuDataType::kSnapshot;
        subItems[0].category_type = amd::ama::SubscribeCategoryType::kBond;
        subItems[0].security_code[0] = '\0';

        /*
            以kAdd方式发起订阅,在已有基础上增加一条订阅信息
            因为发起订阅的数量为1,因此最终只会上送subItems[0]的订阅信息
            订阅结果：
                深交所全部证券代码的股票快照和逐笔成交 + 上交所全部证券代码的基金快照 + 上交所全部证券代码的债券快照
        */
        ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kAdd, subItems, 1);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }

        /*
            以kDel发起订阅,在已有基础上删除一条订阅信息
            因为发起订阅的数量为1,因此最终只会上送subItems[0]的订阅信息
            订阅结果：
                深交所全部证券代码的股票快照和逐笔成交 + 上交所全部证券代码的基金快照
        */
        ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kDel, subItems, 1);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }

        /*
            以kCancelAll发起订阅,取消所有订阅信息
            订阅结果：
                无
        */
        ret = amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kCancelAll, subItems, 2);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }
    }

    return 0;
}

// 衍生数据订阅代码示例
int32_t SubDerivedData()
{
    // 创建2个存储订阅代码信息的derivedItems数组空间(数组空间在没有初始化的情况下,里面的值是不确定的,订阅上送时可能会导致订阅与预期不符)
    amd::ama::SubscribeDerivedDataItem derivedItems[2];
    memset(derivedItems, 0, sizeof(derivedItems));

    // 订阅信息1：订阅深交所000001证券代码(设置derivedItems数组的第0位内存空间)
    derivedItems[0].market = amd::ama::MarketType::kSZSE;
    strcpy(derivedItems[0].security_code, "000001");

    // 订阅信息2：订阅上交所600000证券代码(设置derivedItems数组的第1位内存空间)
    derivedItems[1].market = amd::ama::MarketType::kSSE;
    strcpy(derivedItems[1].security_code, "600000");

    /*
        以kSet发起订阅,设置两条订阅信息(此前所有已设置好的订阅信息都将会被覆盖),订阅数据类型见对应开发指南
        因为发起订阅的数量为2,因此会上送derivedItems[0]和derivedItems[1]的订阅信息
        订阅结果：
            深交所000001证券代码的IOPV快照数据 + 上交所600000证券代码的IOPV快照数据
    */
    int ret = amd::ama::IAMDApi::SubscribeDerivedData(amd::ama::SubscribeType::kSet, amd::ama::SubscribeDerivedDataType::kIOPVSnapshot, derivedItems, 2);
    if(ret != amd::ama::ErrorCode::kSuccess)
    {
        return -1;
    }

    // kAdd、kDel、kCancelAll订阅操作示例,可根据需求调用
    if(false)
    {
        /*
            以kAdd发起订阅,在已有基础上增加两条订阅信息,订阅数据类型见对应开发指南
            因为发起订阅的数量为2,因此会上送derivedItems[0]和derivedItems[1]的订阅信息
            订阅结果：
            深交所000001证券代码的IOPV快照数据 & 委托簿快照数据 + 上交所600000证券代码的IOPV快照数据 & 委托簿快照数据
        */
        ret = amd::ama::IAMDApi::SubscribeDerivedData(amd::ama::SubscribeType::kAdd, amd::ama::SubscribeDerivedDataType::kOrderBookSnapshot, derivedItems, 2);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }

         /*
            以kDel发起订阅,在已有基础上删除一条订阅信息,订阅数据类型见对应开发指南
            因为发起订阅的数量为1,因此会上送derivedItems[0]的订阅信息
            订阅结果：
            深交所000001证券代码的IOPV快照数据 + 上交所600000证券代码的IOPV快照数据 & 委托簿快照数据
        */
        ret = amd::ama::IAMDApi::SubscribeDerivedData(amd::ama::SubscribeType::kDel, amd::ama::SubscribeDerivedDataType::kOrderBookSnapshot, derivedItems, 1);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }

        /*
            以kCancelAll发起订阅,取消所有订阅信息
            订阅结果：
                无
        */
        ret = amd::ama::IAMDApi::SubscribeDerivedData(amd::ama::SubscribeType::kCancelAll, amd::ama::SubscribeDerivedDataType::kOrderBookSnapshot, derivedItems, 2);
        if(ret != amd::ama::ErrorCode::kSuccess)
        {
            return -1;
        }
    }

    return 0;
}

// 代码表接口获取示例
bool GetCodeList()
{
    amd::ama::CodeTableRecordList list;

    // 预先分配2条查询信息的items数组空间(数组空间在没有初始化的情况下,里面的值是不确定的,查询信息上送时可能会与预期不符)
    amd::ama::SubCodeTableItem items[2] ;
    memset(items, 0, sizeof(items));

    //查询信息1：查询深交所000001证券代码(设置items数组的第0位内存空间)
    items[0].market = amd::ama::MarketType::kSZSE;
    strcpy(items[0].security_code, "000001");

    //查询信息2：查询上交所600000证券代码(设置items数组的第1位内存空间)
    items[1].market = amd::ama::MarketType::kSSE;
    strcpy(items[1].security_code, "600000");

    // 发起查询,上送items[0]和items[1]的查询信息；查询后的数据通过list返回
    bool ret = amd::ama::IAMDApi::GetCodeTableList(list, items, 2);
    if(ret)
    {
        std::lock_guard<std::mutex> _(g_mutex);
        for(uint32_t i=0; i< list.list_nums; i++)
        {
            /* TODO */
            //获取到代码表信息后,可以根据需求对代码表信息进行处理,records 是代码表数据头指针
            std::cout << amd::ama::Tools::Serialize(list.records[i]) << std::endl;
        }

        if(list.list_nums > 0)
            amd::ama::IAMDApi::FreeMemory(list.records);  //释放代码表内存池数据
    }
    return ret;
}

// ETF代码表接口获取示例
bool GetETFCodeList()
{
    amd::ama::ETFCodeTableRecordList list;

    // 预先分配2条查询信息的items数组空间(数组空间在没有初始化的情况下,里面的值是不确定的,查询信息上送时可能会与预期不符)
    amd::ama::ETFItem items[2] ;
    memset(items, 0, sizeof(items));

    //查询信息1：查询深交所000001证券代码(设置items数组的第0位内存空间)
    items[0].market = amd::ama::MarketType::kSZSE;
    strcpy(items[0].security_code, "000001");

    //查询信息2：查询上交所600000证券代码(设置items数组的第1位内存空间)
    items[1].market = amd::ama::MarketType::kSSE;
    strcpy(items[1].security_code, "600000");
    
    // 发起查询,上送items[0]和items[1]的查询信息；查询后的数据通过list返回
    bool ret = amd::ama::IAMDApi::GetETFCodeTableList(list, items, 2);
    if(ret)
    {
        std::lock_guard<std::mutex> _(g_mutex);
        for(uint32_t i=0; i< list.etf_list_nums; i++)
        {
            /* TODO */
            //获取到代码表信息后,可以根据需求对代码表信息进行处理,etf_records 是ETF代码表数据头指针
            std::cout << amd::ama::Tools::Serialize(list.etf_records[i]) << std::endl;
        }
        
        if(list.etf_list_nums > 0)
            amd::ama::IAMDApi::FreeMemory(list.etf_records);  //释放ETF代码表内存池数据
    }
    return ret;
}

//国际市场汇率数据获取示例
bool GetIMCExchangeRateData()
{
    amd::ama::IMCExchangeRateList list;

    bool ret = amd::ama::IAMDApi::GetIMCExchangeRate(list);         //查询国际市场汇率数据,查询后的数据通过list返回
    if(ret)
    {
        std::lock_guard<std::mutex> _(g_mutex);
        for(uint32_t i=0; i<list.imc_list_nums; i++)
        {
            /* TODO */
            //获取到国际市场汇率数据后,可以根据需求数据进行处理,imc_rate_data 是国际市场汇率数据头指针
            std::cout << amd::ama::Tools::Serialize(list.imc_rate_data[i]) << std::endl;
        }

        if(list.imc_list_nums > 0)
            amd::ama::IAMDApi::FreeMemory(list.imc_rate_data);      //释放国际市场汇率数据内存
    }

    return ret;
}

//RDI静态数据获取示例
bool GetRDIData()
{
    //预先分配2条查询信息的rdiItems数组空间(数组空间在没有初始化的情况下,里面的值是不确定的,查询信息上送时可能会与预期不符)
    amd::ama::RDIQueryItem rdiItems[2];

    //查询信息1(查询代码"010011"的RDI数据,代码为空时("")表示订阅所有代码),设置rdiItems数组的第0位内存空间
    strcpy(rdiItems[0].security_code, "000001");

    //查询信息2(查询代码"010214"的RDI数据,代码为空时("")表示订阅所有代码),设置rdiItems数组的第0位内存空间
    strcpy(rdiItems[1].security_code, "010214");

    // 获取RDI信息
    /*
    获取债券信息(银行间) BondInfoInterbankList/GetBondInfoInterbank
    获取资产支持证券信息(ABS) ABSInfoList/GetABSInfo
    获取资产支持证券历史信息(ABS历史) ABSHistoryInfoList/GetABSHistoryInfo
    获取资产支持证券信用评级数据 ABSCreditRatingsList/GetABSCreditRatings
    获取预发行债券信息数据 PreIssuedBondInfoList/GetPreIssuedBondInfo
    获取上市前债券信息(银行间)数据 PreIPOBondInfoList/GetPreIPOBondInfo
    获取可交易债券信息数据 XBondTradeBondInfoList/GetXBondTradeBondInfo
    获取质押券折算率(匿名点击)信息 PledgedConvertRateACInfoList/GetPledgedConvertRateACInfo
    获取X-Repo 分层报价群组信息数据 XRepoHierQuoteGroupInfoList/GetXRepoHierQuoteGroupInfo(没有代码信息,接口无item参数)
    获取X-Repo 合约信息数据 XRepoContractInfoList/GetXRepoContractInfo
    获取利率互换(固浮)信息数据 SwapFixedFloatInfoList/GetSwapFixedFloatInfo
    获取利率互换(固浮)期差基差合约信息数据 SwapFixedFloatBasisContractInfoList/GetSwapFixedFloatBasisContractInfo
    */

    amd::ama::BondInfoInterbankList list;
    bool result = amd::ama::IAMDApi::GetBondInfoInterbank(list,rdiItems, 2);    //查询RDI数据,查询后的数据通过list返回
    if (result) 
    {
        std::lock_guard<std::mutex> _(g_mutex);
        for(uint32_t i=0; i<list.data_cnt; i++)
        {
            /* TODO */
            //获取到RDI数据后,可以根据需求数据进行处理,data 是RDI数据头指针
            std::cout << amd::ama::Tools::Serialize(list.data[i]) << std::endl;
        }

        if(list.data_cnt > 0)
            amd::ama::IAMDApi::FreeMemory(list.data);      //释放RDI数据内存
    }

    return 0;
}


int main()
{
    
    amd::ama::Cfg cfg; // 准备AMA配置

    /*
        通道模式设置及各个通道说明:
        cfg.channel_mode = amd::ama::ChannelMode::kTCP;   ///< TCP 方式计入上游行情系统
        cfg.channel_mode = amd::ama::ChannelMode::kAMI;   ///< AMI 组播方式接入上游行情系统
        cfg.channel_mode = amd::ama::ChannelMode::kRDMA;  ///< 开启硬件加速RDMA通道,抓取网卡数据包数据
        cfg.channel_mode = amd::ama::ChannelMode::kEXA;   ///< 开启硬件加速EXA通道,抓取网卡数据包数据
        cfg.channel_mode = amd::ama::ChannelMode::kPCAP;  ///< 开启硬件加速PCAP通道,抓取网卡数据包数据
        cfg.channel_mode = amd::ama::ChannelMode::kMDDP;  ///< 直接接入交易所网关组播数据,现在只有深圳交易所开通了此服务
        cfg.channel_mode = amd::ama::ChannelMode::kFPGA;  ///< FPGA接入数据
        cfg.channel_mode = amd::ama::ChannelMode::kUDP;   ///< AMD组播方式获取数据
        cfg.channel_mode = amd::ama::ChannelMode::kTCP|amd::ama::ChannelMode::kAMI;  ///< 同时通过TCP方式和AMI组播方式接入上游,通过cfg.ha_mode 设置对应的高可用设置模式
    */
    cfg.channel_mode = amd::ama::ChannelMode::kTCP;

    cfg.tcp_compress_mode = 0;  //TCP传输数据方式: 0 不压缩 1 华锐自定义压缩 2 zstd压缩(仅TCP模式有效)
    
    
    /*
        通道高可用模式设置
        1. cfg.channel_mode 为单通道时,建议设置值为kMasterSlaveA / kMasterSlaveB
        2. cfg.channel_mode 混合开启多个通道时,根据需求设置不同的值
            1) 如果需要多个通道为多活模式接入,请设置kRegularDataFilter值
            2) 如果需要多个通道互为主备接入,请设置值为kMasterSlaveA / kMasterSlaveB,kMasterSlaveA / kMasterSlaveB 差别请参看注释说明
               通道优先级从高到低依次为 kRDMA/kEXA/kMDDP/kAMI/kTCP/kPCAP
    */
    cfg.ha_mode = amd::ama::HighAvailableMode::kMasterSlaveA;


    
    cfg.min_log_level = amd::ama::LogLevel::kInfo; // 设置日志最小级别：Info级(高于等于此级别的日志就可输出了), AMA内部日志通过 OnLog 回调函数返回
    
    /*
        设置是否输出监控数据: true(是), false(否), 监控数据通过OnIndicator 回调函数返回
        监控数据格式为json, 主要监控数据包括订阅信息,数据接收数量统计
        数据量统计:包括接收数量和成功下发的数量统计,两者差值为过滤的数据量统计
        eg: "RecvSnapshot": "5926", "SuccessSnapshot": "5925",表示接收了快照数据5926个,成功下发5925个,过滤数据为 5926 - 5925 = 1 个
            过滤的数据有可能为重复数据或者非订阅数据
    */
    cfg.is_output_mon_data = false;   
    
    /*
        设置逐笔保序开关: true(开启保序功能) , false(关闭保序功能),现阶段只对MDDP通道和委托簿本地构建生效
        主要校验逐笔成交数据和逐笔委托数据是否有丢失,如果丢失会有告警日志,缓存逐笔数据并等待keep_order_timeout_ms(单位ms)时间等待上游数据重传,
        如果超过此时间,直接下发缓存数据,默认数据已经丢失,如果之后丢失数据过来也会丢弃。
        同时由于深圳和上海交易所都是通道内序号连续,如果打开了保序开关,必须订阅全部代码的逐笔数据,否则一部分序号会被订阅过滤,导致数据超时等待以及丢失告警。
    */
    cfg.keep_order = false;
    cfg.keep_order_timeout_ms = 3000;


    cfg.is_subscribe_full = false; //设置默认订阅: true 代表默认全部订阅, false 代表默认全部不订阅

    /*
        配置UMS信息:
        username/password 账户名/密码, 一个账户只能保持一个连接接入(注意: 如果需要使用委托簿功能,注意账号需要有委托簿功能权限)
        ums地址配置:
            1) ums地址可以配置1-8个 建议值为2 互为主备, ums_server_cnt 为此次配置UMS地址的个数
            2) ums_servers 为UMS地址信息数据结构:
                local_ip 为本地地址,填0.0.0.0 或者本机ip
                server_ip 为ums服务端地址
                server_port 为ums服务端端口
    */
    strcpy(cfg.username, "user1");
    strcpy(cfg.password, "user1");
    cfg.ums_server_cnt = 2;
    strcpy(cfg.ums_servers[0].local_ip, "0.0.0.0");
    strcpy(cfg.ums_servers[0].server_ip, "10.23.128.35");
    cfg.ums_servers[0].server_port = 8200;

    strcpy(cfg.ums_servers[1].local_ip, "0.0.0.0");
    strcpy(cfg.ums_servers[1].server_ip, "10.23.128.49");
    cfg.ums_servers[1].server_port = 8200;

    
    /*
        业务数据回调接口(不包括 OnIndicator/OnLog等功能数据回调)的线程安全模式设置:
        true: 所有的业务数据接口为接口集线程安全
        false: 业务接口单接口为线程安全,接口集非线程安全 
    */
    cfg.is_thread_safe = false;

    /*
        委托簿前档行情参数设置(仅委托簿版本API有效,若非委托簿版本参数设置无效):
            1) 行情输出设置,包括档位数量以及每档对应的委托队列数量
            2)委托簿计算输出参数设置,包括线程数量以及递交间隔设置
    */
   cfg.enable_order_book = amd::ama::OrderBookType::kNone; //是否开启委托簿计算功能
   cfg.entry_size = 10;                                    //委托簿输出最大档位数量(递交的委托簿数据档位小于等于entry_size)
   cfg.order_queue_size = 10;                              //每个档位输出的委托队列揭示(最大设置50)
   cfg.thread_num = 3;                                     //构建委托簿的线程数量(递交和构建每一个市场分别开启thread_num个线程)
   cfg.order_book_deliver_interval_microsecond = 0;        /*递交的最小时间间隔(单位:微妙, 委托簿递交间隔大于等于order_book_deliver_interval_microsecond)
                                                             设置为0时, 委托簿实时下发, 不会等待间隔.
                                                            */

    MySpi spi;   //初始化回调类
    
    /*
        初始化回调以及配置信息, Init函数返回amd::ama::ErrorCode::kSuccess 只能说明配置信息cfg和spi符合配置规范, 后续的鉴权和登陆过程为异步过程, 
        如果鉴权和登陆过程遇到环境异常(如网络地址不通, 账号密码错误等), 会通过OnLog/OnEvent两个回调函数返回鉴权和登陆结果
    */
    if (amd::ama::IAMDApi::Init(&spi, cfg)
        != amd::ama::ErrorCode::kSuccess)
    {
        std::lock_guard<std::mutex> _(g_mutex);
        std::cout << "Init AMA failed" << std::endl;
        amd::ama::IAMDApi::Release();
        return -1;
    }
    
    /*TODO 按照实际需求调用不同的接口函数*/
    // SubscribeWithDataAuth();    // 数据权限订阅示例
    // SubscribeWithCategory();    // 品种订阅示例
    // SubDerivedData();           // 衍生数据订阅代码示例
    // GetCodeList();              // 代码表接口获取示例
    // GetETFCodeList();           // ETF代码表接口获取示例
    // GetIMCExchangeRateData();   // 国际市场汇率数据获取示例
    // GetRDIData();               // RDI静态数据获取示例
    // ReplayRequest();            // 回放请求示例

    /* 保持主线程不退出 */
    bool is_running = true;
    while (is_running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> _(g_mutex);
        std::cout << "AMA is running ..." << std::endl;
    }

    amd::ama::IAMDApi::Release();

    return 0;
}
