run.sh 脚本参数说明:
基本参数设置(非必要场景使用run.sh 中即可)
--log-to-console                                       日志和数据是否允许直接输出到到终端
--log-level  （-l）                                    指定应用程序日志级别，如果不使用此参数默认日志级别是Info。（使用此参数需指定应用程序日志级别，日志级别有：Trace , Debug , Info  , Warn  , Error , Fatal ，日志严重级别从左到右依次递增）

业务数据控制参数(输出屏幕/落地csv)
--data-to-console                                      业务数据是否允许输出到终端
--data-to-csv                                          业务数据是否落地到 auto-csv-dir 指定的路径，落地格式为csv文件格式
--auto-csv-dir                                         设置csv数据落地的存储路径，如果不使用则默认存储路径为(./auto-csv)
--static-data-dir                                         设置静态数据的存储路径，如果不使用则默认存储路径为(./static_data_data)


json 关键配置说明:
//基本功能配置
{
    //基本参数配置
    "IsThreadSafe": false,
    "ChannelMode": ["TCP"],    //通道标识,取值范围[TCP/AMI/RDMA/EXA/PCAP/MDDP/FPGA]
    "HAMode": 0,               //主备通道标识,取值范围[0/1/2]
    "IsSubscribeFull": false,  //初始订阅状态是否设置为全订阅
    "KeepOrder": false,        //保序开关(不建议打开，暂时只对委托簿构建数据生效，同时上海逐笔数据由于原始数据乱序,会产生大量乱序告警)
    "KeepOrderTimeOutMs": 3000,//保序超时时间
    "IsOutputMonData": true,   //是否落地监控日志
    "EnableOrderBook": 0,      //委托簿标识,取值范围[0-不开启委托簿/ 1-本地构建委托簿，向服务端请求同步初始状态(需订阅相应逐笔成交和逐笔委托)/ 2-服务端远程TCP方式推送委托簿数据]
    "OrderBookThread": 3,      //本地构建委托簿线程数
    "OrderBookEntrySize": 10,  //委托簿档位数量,理论上无上限
    "OrderQueueSize": 0,       //每一档委托队列数量上限(最大设置为50)
    "OrderBookOutputInternal": 10000, //委托簿递交最快频率(单位：微妙)
    "TcpCompressMode": 0,      //设置tcp压缩模式，取值范围[0-不压缩/1-华锐自定义压缩/2-zstd压缩(仅TCP模式有效)]
    
    //设置UMS地址信息/账户/密码
    "UMSInfo":
    [
        {
            "ServerIp": "10.128.0.51",
            "ServerPort": 8305
        }
    ],

    "Username" : "wfb_ama1",
    "Password" : "1",

    //静态数据查询相关配置
    "DynamicGetStaticInfo": false,  //是否需要启用动态查询功能
    //代码表查询
    "CodeTableQuery":
    {
        "Enable": false,
        "CodeTableInfo":
        [
            {
                "Enable": true,
                "Market": "SSE",    //支持市场范围为SSE/SZSE/NEEQ/SHFE/CFFEX/DCE/CZCE/INE, NONE代表订阅所有支持的市场
                "SecurityCode":[]   //空或者""代表订阅全代码
            }
        ]
    },
    //ETF代码表查询
    "ETFQuery":
    {
        "Enable": false,
        "QueryInfo":
        [
            {
                "Enable": true,
                "Market": "SSE",  //支持市场范围为SSE/SZSE, NONE代表订阅所有支持的市场
                "SecurityCode":[] //空或者""代表订阅全代码
            }
        ]
    },
    "GetIMCExchangeRate": false, //是否需要查询IMC港股汇率数据
    //RDI数据查询
    "RDIQuery":
    {
        "Enable": false,
        "QueryInfo":
        [
            {
                "Enable": true,
                //RDI数据查询类型,取值范围[BondInfoInterbank/ABSInfo/ABSHistoryInfo/ABSCreditRatings/PreIssuedBondInfo/PreIPOBondInfo/XBondTradeBondInfo/PledgedConvertRateACInfo/XRepoHierQuoteGroupInfo/XRepoContractInfo/SwapFixedFloatInfo/SwapFixedFloatBasisContractInfo]
                "Type": "BondInfoInterbank",
                "SecurityCode":[""]
            }
        ]
    },

    //历史回放功能相关配置
    "DynamicReplayRequest": false,  //是否需要动态发起回放请求
    //历史回放请求配置
    "ReplayRequest": 
    {
        "Enable": false,
        "SubInfo":
        [
            {
                "Enable": true,
                "RequestType" : "Add",              //新增回放请求
                "DataType": "GeneralSnapshot",      //回放数据类型，取值范围[GeneralSnapshot/GeneralTickExecution]
                "TaskId": 5211433320000001,         //可选，如果无此项内部会默认生成一个id号
                "BeginTime":20220521143332000,
                "EndTime":20220521153332000,
                "CodeInfos":
                [
                    {
                        "Market": "SSE",
                        "SecurityCode":["510180"]
                    },
                    {
                        "Market": "SZSE",
                        "SecurityCode":["159767"]
                    }
                ]
            },
            {
                "Enable": false,
                "RequestType" : "Cancel",       //取消回放请求
                "TaskId": 2022052108270001
            }
        ]
    },

    //实时行情以及实时衍生数据订阅配置
    //以下订阅订阅操作类型取值范围[Set/Add/Del/CancelAll]
    //market 取值范围[SSE/SZSE/NEEQ/SHFE/CFFEX/DCE/CZCE/INE/HKEx/CFETS], NONE代表订阅全部市场
    "DynamicLoadSubInfo":false,         //是否需要启用动态订阅功能
    //委托簿数据订阅配置(不推荐使用，后续会逐渐被SubDerivedData替代)
    "SubOrderBook": 
    {
        "SubscribeType" : "Set",        //订阅操作类型
        "Enable": false,
        "SubInfo":
        [
            {
                "Enable": true,
                "Market": "SSE",        //暂时只支持SSE/SZSE两个市场
                "Flag": [],             //订阅类型,取值范围[OrderBook/OrderBookSnapshot], 填空代表订阅所有支持类型
                "SecurityCode":["600000"]
            }
        ]
    },
    //衍生数据订阅配置(包括委托簿数据订阅)
    "SubDerivedData": 
    {
        "Enable": false,
        "SubInfo":
        [
            {
                "Enable": true,
                "SubscribeType" : "Set",
                "Flag": "IOPVSnapshot", //订阅类型,取值范围[OrderBook/OrderBookSnapshot/IOPVSnapshot]
                "CodeInfos":
                [
                    {
                        "Market": "SSE",
                        "SecurityCode":["510180"]
                    }
                ]
            }
        ]
    },
    //按照品种订阅实时行情数据
    "SubscribeCategory" :
    {
        "SubscribeType" : "Set",
        "Enable": true,
        "SubInfo":
        [
            {
                "Enable": true,
                "Market": "SSE",                    //市场类型设置,取值范围[NEEQ/SHFE/CFFEX/DCE/CZCE/INE/NASDAQ/SSE/SZSE/HKEx/CFETS/NONE],填NONE代表订阅所有市场
                "SubSecuDataType": [],              //一级数据类型,取值范围[Snapshot/TickExecution/TickOrder/OrderQueue],填空代表订阅全部支持类型
                "SubCategoryType": [],              //品种类型设置,取值范围[Stock/Fund/Bond/Index/HKT/Option/FutureOption/HKEx/CFETSRMB/Others]
                "SecurityCode":["600000", "510180"]
            }
        ]
    },
    //按照数据权限范围订阅实时行情数据
    "Subscribe" :
    {
        "SubscribeType" : "Set",
        "Enable": false,
        "SubInfo":
        [
            {
                "Market": "SSE",                    //市场类型设置,取值范围[NEEQ/SHFE/CFFEX/DCE/CZCE/INE/NASDAQ/SSE/SZSE/HKEx/CFETS/NONE],填NONE代表订阅所有市场
                "Enable": false,
                "SubDataType": [],                  //权限数据类型,取值范围[Snapshot/IndexSnapshot/OptionSnapshot/HKTSnapshot/HKTRealtimeLimit/HKMarketStatus/TickOrder/TickExecution/OrderQueue/FutureSnapshot/AfterHourFixedPriceSnapshot/AfterHourFixedPriceTickExecution/CSIIndexSnapshot/NEEQSnapshot/FundExpertSnapshot/HKExSnapshot/HKExTickExecution/HKExConnectTurnoverSnapshot/CFETSSnapshot/CFETSTickExecution/CFETSPrivateMarket/UsaSnapshot/FixedIncomeSnapshot]
                "SecurityCode":["600000"]
            }
        ]
    }
}