<font size="6">**东方财富证券极速行情系统行情API开发手册(Ver2.19.0)**</font>

<font size="6">文档说明</font>

<table><tbody>
    <tr>
        <th>日期</th><th>api 版本</th><th>修订摘要</th>
    </tr>
    <tr>
        <td>2023.03.03</td><td>2.16.0</td><td>API初版</td>
    </tr>
    <tr>
        <td>2023.05.22</td><td>2.17.0</td><td>API支持北交所股转相关数据</td>
    </tr>
    <tr>
        <td>2023.07.07</td><td>2.18.0</td><td>API修复接口类型，修复版本号问题</td>
    </tr>
    <tr>
        <td>2024.04.12</td><td>2.19.0</td><td>完善接口说明，Create接口拼写错误修复</td>
    </tr>
</table>

<font size="6">前言</font>

本接口规范用以指导开发通过 EMTAPI 对接的方式访问东财快速行情系统，获取指数、现货的相关数据。

本接口规范描述内容包括可开展的业务、必要的运行指导以及详细的数据交换格式

#  EMTQuoteAPI介绍

本部分主要介绍极速行情系统的接口，包括：

* EMTQuoteAPI简介。
* EMTQuoteAPI运行模式。
* EMTQuoteAPI编程接口

## EMTQuoteAPI简介

### 背景

&emsp;&emsp;为实现客户快速交易中获得行情，东方财富证券推出了极速行情系统。客户端系统调用 EMTQuoteAPI 即可完成与极速行情系统的对接，接收到极速行情系统的行情后 EMTQuoteAPI 将回调（Callback）客户端系统。

### 简介

&emsp;&emsp;EMTQuoteAPI是一个基于C++的类库，通过使用和扩展类库提供的接口来实现全部的行情功能，支持指数、现货快照相关数据。支持静态数据，最新快照，最新价等数据。

&emsp;&emsp;该类库包含以下文件：

|文件路径        |文件名                    |文件描述                  |
|:--------------|:------------------------|:-------------------------|
|demo           |CMakeLists.txt           |示例编译文件                |
|               |config.ini               |配置文件                   |
|               |Main.cpp         |示例主函数                 |
|               |my_quote_spi.cpp           |示例接口方法封装            |
|               |my_quote_spi.h             |示例接口方法头文件           |
|doc            |API开发手册               |                          |
|include        |emt_api_data_type.h      |定义兼容数据基本类型         |
|               |quote_api.h          |定义行情相关接口            |
|               |emt_quote_struct.h       |定义L1行情相关数据结构        |
| |sze_hpf_define.h |定义L2 深市相关数据结构（L1用户暂不支持使用） |
| |sse_hpf_define.h |定义L2 沪市相关数据结构（L1用户暂不支持使用） |
|lib/windows/x64|emt_quote_api.dll          |Windows行情接口动态链接库   |
|               |emt_quote_api.lib          |Windows行情接口导入库文件   |
|linux          |libemt_quote_api.so        |Linux行情接口动态链接库     |

&emsp;&emsp;客户系统在开发过程中需注意<font color=red>"当前版本不开放的业务"</font>，同时在每个功能描述中的具体描述。

### 发行的平台

目前发布了以下操作系统平台的版本：

* Intel X86/Windows10：包括.h文件、.dll文件和.lib文件。
* Intel Linux：包括.h文件和.so文件。

如果需要其他操作系统版本，请联系行情专线。

## 运行模式

### 工作线程

​    QuoteApi使用TCP模式接收行情。客户系统应用程序主要有三个线程，一是应用程序主线程，二是数据接收线程，接收上游服务的行情数据；三是数据处理线程，处理API业务消息以及对应的行情数据回调。

&emsp;&emsp;QuoteSpi提供的接口回调是由EMTQuoteAPI数据处理线程驱动，通过实现SPI中的接口方法，从极速行情系统收取所需数据。

&emsp;&emsp;如果重载的客户系统的应用程序的某个回调函数阻塞，则等于阻塞了EMTQuoteAPI工作线程，API与极速行情系统的通信会停止，因此通常应该迅速返回。在QuoteSpi派生类的回调函数中，可以利用将数据放入缓冲区或通过Windows的消息机制来实现迅速返回。

### 运行流程

客户系统和极速行情系统的交互过程分为2个阶段：

* 初始化阶段。
* 功能调用阶段。

#### 初始化阶段

&emsp;&emsp;在初始化阶段，客户系统的程序必须完成如下步骤（具体代码请参考开发实例）：

|顺序    |客户系统    |调用方法|
|:------|:-----------|:-------|
|1     |创建一个QuoteApi实例             |QuoteApi::CreateQuoteApi|
|2     |产生一个事件处理的实例；（步骤1返回）|                        |
|3     |注册一个事件处理的实例；           |QuoteApi::RegisterSpi   |
|4 |登录服务 |QuoteApi::Login |
|5     |订阅行情流                       |QuoteApi::SubscribeMarketData|
|6     |设置开发代码     |相关业务实现及返回处理 QuoteSpi::OnMarketData |

&emsp;&emsp;<font color=red>*开发代码由我司人员提供，需客户申请。*</font>

#### 功能调用阶段

&emsp;&emsp;在功能调用阶段，客户系统可以任意调用行情接口中的请求方法，如SubscribeMarketData；同时提供回调函数以响应回报数据，如OnDepthMarketData；

### 时序示例

&emsp;&emsp;EMTQuoteAPI提供了两个接口，分别为QuoteApi和QuoteSpi。

&emsp;&emsp;客户系统可以通过QuoteApi发出操作请求，通过继承QuoteSpi并重载回调函数来处理极速行情系统的回复或响应。

#### 查询流程时序

&emsp;&emsp;各种查询功能，是通过不同的请求功能方法实现的，但大体流程基本相似。

&emsp;&emsp;查询多条数据，会分多次返回，返回参数中的 is_last 作为标记是否为最后一条

### 连接方式

#### 单账户登录
&emsp;&emsp;目前一个QuoteApi的示例，仅第一次成功登录有效。

&emsp;&emsp;API中登录Login()接口为同步的，返回后，可以视为已经登录成功。

&emsp;&emsp;API支持断线重连功能，并且支持恢复订阅记录

#### demo示例
```cpp
#include <thread>
#include <iostream>

#include "emt_api_data_type.h"
#include "inicpp.h"
#include "quote_api.h"
#include "my_quote_spi.h"

using namespace EMQ::API;

int main()
{
    ini::IniFile cfg;
    cfg.load("config.ini");
    std::string gateway_ip =cfg["API"]["gatewayIp"].as<std::string>(); 
    uint16_t gateway_port =cfg["API"]["gatewayPort"].as<uint16_t>(); 
    std::string username =cfg["API"]["username"].as<std::string>(); 
    std::string password =cfg["API"]["password"].as<std::string>(); 
    uint32_t channel =cfg["Rebuild"]["chno"].as<uint32_t>(); 
    uint64_t seq_beg =cfg["Rebuild"]["begSeq"].as<uint64_t>(); 
    uint64_t seq_end =cfg["Rebuild"]["endSeq"].as<uint64_t>(); 
    
    //创建API实例
    QuoteApi* api = QuoteApi::CreatQuoteApi("./logs", EMQ_LOG_LEVEL::EMQ_LOG_LEVEL_DEBUG, EMQ_LOG_LEVEL::EMQ_LOG_LEVEL_DEBUG);
    
    //注册自定义实现的spi
    QuoteSpi* spi = new MyQuoteSpi();
    api->RegisterSpi(spi);

    //登陆行情服务器
    api->Login(gateway_ip, gateway_port, username, password);

    char* securityCodesSz[] = { (char*)"300059",(char*)"300033" };

    //订阅数据
    api->SubscribeMarketData(securityCodesSz, 2, EMQ_EXCHANGE_SZ);
    
    //api退出
    //api->logout();
        int num = 0;
    while (num < 100000000000)
    {
        num += 1;
        // 主线程不停
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    return 0;
}
    
```

# API公共接口

## 业务支持索引
|方法|描述|
|:----|:----|
|[CreateQuoteApi](#CreateQuoteApi)|创建一个Quote API接口类实例|
|[Login](#login)|用户登录请求|
|[Logout](#logout)|登出请求|
|[RegisterSpi](#registerspi)|注册回调接口|
|[SetBuffer](#setbuffersize)|设置连接时的接收缓冲区大小|
|[SetThreadAffinity](#SetThreadAffinity)|设置系统接收线程和处理线程的cpu id (Windows下无效)|

## API接口描述

### <a id="CreateQuoteApi">CreateQuoteApi </a>

```cpp
/**
 * @brief  创建API接口类实例
 * 
 * @param  log_path                   生成日志文件的路径
 * @param  log_file_level             文件日志的日志级别  
 * @param  log_console_level          控制台日志的日志级别 0 - trace, 1 - debug, 2 - info, 3 -warn, 4 - error, 5 - FATAL, 6 - OFF 
 * @return                            EMT API类的实例
 */
static QuoteApi* CreateQuoteApi(const char* log_path, EMQ_LOG_LEVEL log_file_level, EMQ_LOG_LEVEL log_console_level);
```

### <a id="logout">Login</a>
```cpp
/**
* @brief  发送登录消息到网关服务器
* 
* @attention                          此函数为同步阻塞模式，不需要异步等待登录成功，当函数返回即可进行后续操作，此api只能有一个连接
* @param  ip                          网关IP地址
* @param  port                        网关端口号
* @param  user                        登录用户账号
* @param  pwd                         登录用户密码
* @return                             登录是否成功  登录是否成功，"0"表示登录成功，"-1"表示连接服务器出错，"-2"表示登录链接被打断失败，可能存在发送接收消息，或处理消息失败，可以尝试重新登录。"-3"表示登录失败，可能未发送正确的登陆消息格式或者鉴权失败。    
*/
virtual int32_t Login(const char* ip, const uint16_t port, const char* user, const char* pwd) = 0;

```
###  <a id="logout">Logout</a>
```cpp
/**
 * @brief  发送登出消息到网关服务器，释放资源
 * 
 * @attention                         此函数为同步阻塞模式，不需要异步等待登录成功，当函数返回即可进行后续操作
 */
virtual void Logout() = 0;
```
###  <a id="registerspi">RegisterSpi</a>
```cpp
/**
* @brief  注册回调接口类spi实例
* 
* @param  quote_spi                   派生自回调接口类的实例，请在登录之前完成设定
*/
virtual void RegisterSpi(QuoteSpi* quote_spi) = 0;
```
### <a id="setbuffersize">SetBuffer</a>
```cpp
/**
*   设置缓冲区大小
*   @attention                        设置api内部消息缓存区大小,登录前设置生效
*   @param buffer_size                缓存区大小，单位MB
*   @return                           "0"表示设置成功，非"0"表示输入无效启用默认值
*/
virtual int32_t SetBuffer(size_t buffer_size) = 0;

```

### <a id="SetThreadAffinity">SetThreadAffinity</a>

```cpp
/**
 * @brief  设置系统线程绑定的cpu id
 * 
 * @attention                         此函数可不调用,调用需要在登录前调用。id填写 -1 则操作系统自由分配，不进行绑核。
 * @param  recv_cpu_no                接收线程绑定cpu id号 例如绑定cpu 0，可以设置0，绑定cpu 2，可以设置2，建议绑定后面的cpu
 * @param  process_cpu_no             处理线程绑定cpu id号
 * @return
 */
virtual void SetThreadAffinity(int32_t recv_cpu_no = -1,int32_t process_cpu_no = -1) = 0;

```



# API业务接口

## 业务支持索引

|方法API|回调SPI|描述|
|:----|:----|-----|
|[SubscribeAllIndexData](#subscribeallindexdata)|[onSubscribeAllIndexData](#onsubscribeallindexdata)<br/>[OnIndexData](#onindexdata)|订阅全市场指数快照行情|
|[UnSubscribeAllIndexData](#unsubscribeallindexdata)|[onUnSubscribeAllIndexData](#onunsubscribeallindexdata)|取消订阅全市场指数快照行情|
|[SubscribeIndexData](#subscribeindexdata)|[onSubIndexData](#onsubindexdata)<br/>[OnIndexData](#onindexdata)|订阅指数快照行情|
|[UnSubscribeIndexData](#unsubscribeindexdata)|[onUnSubIndexData](#onunsubindexdata)|取消订阅指数快照行情|
|[SubscribeAllMarketData](#subscribeallmarketdata)|[onSubscribeAllMarketData](#onsubscribeallmarketdata)<br/>[OnDepthMarketData](#ondepthmarketdata)|订阅全市场合约快照行情|
|[UnSubscribeAllMarketData](#unsubscribeallmarketdata)|[onUnSubscribeAllMarketData](#onunsubscribeallmarketdata)|退订全市场合约快照行情|
|[SubscribeMarketData](#subscribemarketdata)|[onSubMarketData](#onsubmarketdata)<br/>[OnDepthMarketData](#ondepthmarketdata)|订阅快照行情，包括股票，基金，债券，权证和质押式回购|
|[UnSubscribeMarketData](#unsubscribemarketdata)|[onUnSubMarketData](#onunsubmarketdata)|取消订阅快照行情，包括股票，基金，债券，权证和质押式回购|
|[QueryAllTickers](#queryalltickers)|[onQueryAllTickers](#onqueryalltickers)|获取所有合约的部分静态信息，包括沪深市场所有合约的部分静态信息|
|[QueryAllTickersFullInfo](#queryalltickersfullinfo)|[OnQueryAllTickersFullInfo](#onqueryalltickersfullinfo)<br/>[OnQueryHkStaticInfo](#OnQueryHkStaticInfo)<br/>[OnQueryBJGZStaticInfo](#OnQueryBJGZStaticInfo)|获取所有合约的完整静态信息，包括全市场所有合约的完整静态信息|
|[QueryLatestInfo](#querylatestinfo)|[OnQueryLatestIndexData](#onquerylatestindexdata) <br/>[OnQueryLatestMarketData](#onquerylatestmarketdata)|获取最新快照信息|
|[QueryTickersPriceInfo](#queryminhistoryinfo)|[onQueryTickersPriceInfo](#onquerytickerspriceinfo)|获取最新价|
|[RebuildSzData](#RebuildSzData)|[OnRebuildSzData](#OnRebuildSzData)|获取重建回补深市逐笔数据（L1用户不提供此功能）|

## API接口描述
### <a id="subscribeallindexdata">SubscribeAllIndexData</a>
```cpp
/**
*   订阅全市场指数快照行情
*   @attention                      需要与取消订阅全市场指数行情接口配套使用
*   @param exchange_id              表示当前订阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         订阅全市场指数快照行情接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int SubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="unsubscribeallindexdata">UnSubscribeAllIndexData</a>
```cpp
/**
*   取消订阅全市场指数快照行情
*   @attention                      需要与订阅全市场指数行情接口配套使用
*   @param exchange_id              表示当前取消订阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         取消订阅全市场指数快照行情接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int UnSubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="subscribeindexdata">SubscribeIndexData</a>
```cpp
/**
*   订阅指数快照行情
*   @attention                      可一次性订阅同一证券交易所的多个指数，无论用户因为何种问题需要重新登录行情服务器，都需要重新订阅
*   @param tickers                  指数代码数组，注意指数代码必须以"\0"结尾，不包括空格
*   @param count                    需要订阅行情的指数个数
*   @param exchange_id              交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         订阅接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int SubscribeIndexData(char* tickers[], int count, EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="unsubscribeindexdata">UnSubscribeIndexData</a>
```cpp
/**
*   取消订阅指数快照行情
*   @attention                      可一次性取消订阅同一证券交易所的多个指数，无论用户因为何种问题需要重新登录行情服务器，都需要重新订阅
*   @param tickers                  指数代码数组，注意指数代码必须以"\0"结尾，不包括空格
*   @param count                    需要订阅行情的指数个数
*   @param exchange_id              交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         取消订阅接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int UnSubscribeIndexData(char* tickers[], int count, EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="subscribeallmarketdata">SubscribeAllMarketData</a>
```cpp
/**
*   订阅全市场合约快照行情
*   @attention                      需要与全市场退订行情接口配套使用
*   @param exchange_id              表示当前全订阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         订阅全市场行情接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int SubscribeAllMarketData(EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="unsubscribeallmarketdata">UnSubscribeAllMarketData</a>
```cpp
/**
*   退订全市场合约快照行情
*   @attention                      需要与全市场订阅行情接口配套使用
*   @param exchange_id              表示当前全订阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         退订全市场行情接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int UnSubscribeAllMarketData(EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="subscribemarketdata">SubscribeMarketData</a>
```cpp
/**
*   订阅快照行情，包括股票，基金，债券，权证和质押式回购,沪深港股通
*   @attention                      可一次性订阅同一证券交易所的多个合约，无论用户因为何种问题需要重新登录行情服务器，都需要重新订阅
*   @param tickers                  合约代码数组，注意合约代码必须以"\0"结尾，不包括空格
*   @param count                    需要订阅行情的合约个数
*   @param exchange_id              交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         订阅接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int SubscribeMarketData(char* tickers[], int count, EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="unsubscribemarketdata">UnSubscribeMarketData</a>
```cpp
/**
*   取消订阅快照行情，包括股票，基金，债券，权证和质押式回购，沪深港股通
*   @attention                      可一次性取消订阅同一证券交易所的多个合约，无论用户因为何种问题需要重新登录行情服务器，都需要重新订阅
*   @param tickers                  合约代码数组，注意合约代码必须以"\0"结尾，不包括空格
*   @param count                    需要订阅行情的合约个数
*   @param exchange_id              交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         取消订阅接口调用是否成功，"0"表示接口调用成功，非"0"表示接口调用出错
*/
virtual int UnSubscribeMarketData(char* tickers[], int count, EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="queryalltickers">QueryAllTickers</a>
```cpp
/**
*   获取所有合约的部分静态信息，包括全市场所有合约的部分静态信息
*   @param exchange_id              交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场
*   @return                         发送查询请求是否成功，"0"表示发送查询请求成功，非"0"表示发送查询请求不成功
*/
virtual int QueryAllTickers(EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="queryalltickersfullinfo">QueryAllTickersFullInfo</a>
```cpp
/**
*   获取所有合约的完整静态信息，包括全市场所有合约的完整静态信息 
*   @param exchange_id              交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                         发送查询请求是否成功，"0"表示发送查询请求成功，非"0"表示发送查询请求不成功
*/
virtual int QueryAllTickersFullInfo(EMQ_EXCHANGE_TYPE exchange_id = EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_UNKNOWN) = 0;
```
### <a id="querylatestinfo">QueryLatestInfo</a>
```cpp
/**
*   获取最新快照信息
*    @param tickers                  合约ID数组，注意合约代码必须以'\0'结尾，不包含空格
*    @param count                    合约个数，为0表示查询交易所所有合约
*    @param ticker_type              合约类型
*   @param exchange_id               交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @return                          发送查询请求是否成功，"0"表示发送查询请求成功，非"0"表示发送查询请求不成功
*/
virtual int QueryLatestInfo(char* tickers[], int count, EMQ_TICKER_TYPE ticker_type, EMQ_EXCHANGE_TYPE exchange_id) = 0;
```

### <a id="querytickerspriceinfo">QueryTickersPriceInfo</a>
```cpp
/**
* 获取最新价
* @param tickers                      合约ID数组，注意合约代码必须以'\0'结尾，不包含空格
* @param count                        合约个数，为0表示查询交易所所有合约
* @param exchange_id                  交易所代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
* @return                             发送查询请求是否成功，"0"表示发送查询请求成功，非"0"表示发送查询请求不成功
*/
virtual int QueryTickersPriceInfo(char* tickers[], int count, EMQ_EXCHANGE_TYPE exchange_id) = 0;
```
### <a id="RebuildSzData">RebuildSzData</a>

```cpp
/**
* @brief  查询回补深市逐笔数据
* 
* @attention                          此接口对L1用户不提供服务
* @param  chno                        回补数据通道号
* @param  beg_seq                     回补数据起始序号
* @param  end_seq                     回补数据结束序号
* @param  request_id                  保留字段，填写大于等于0的随机值
* @return                             请求是否成功 "0"表示请求成功，"-2"表示请求链接被打断失败,发送回补消息失败，可以尝试重新请求 "-3"表示权限校验失败
*/
virtual int32_t RebuildSzData(const uint32_t chno, const uint64_t beg_seq, const uint64_t end_seq, const uint64_t request_id) = 0;
```



# SPI接口

## 业务支持索引

|方法|描述|
|:----|:----|
|[OnError](#onerror)|错误应答|
|[OnIndexData](#onindexdata)|指数行情通知|
|[OnDepthMarketData](#ondepthmarketdata)|L1快照行情通知|
|[OnSubscribeAllIndexData](#onsubscribeallindexdata)|订阅全市场的指数行情应答|
|[OnUnSubscribeAllIndexData](#onunsubscribeallindexdata)|退订全市场的指数行情应答|
|[OnSubIndexData](#onsubindexdata)|订阅指数行情应答|
|[OnUnSubIndexData](#onunsubindexdata)|退订指数行情应答|
|[OnSubscribeAllMarketData](#onsubscribeallmarketdata)|订阅全市场股票快照行情应答|
|[OnUnSubscribeAllMarketData](#onunsubscribeallmarketdata)|退订全市场的股票快照行情应答|
|[OnSubMarketData](#onsubmarketdata)|订阅快照行情应答，包括股票，基金，债券，权证和质押式回购|
|[OnUnSubMarketData](#onunsubmarketdata)|退订快照行情应答，包括股票，基金，债券，权证和质押式回购|
|[OnQueryAllTickers](#onqueryalltickers)|查询全市场合约部分静态信息的应答|
|[OnQueryAllTickersFullInfo](#onqueryalltickersfullinfo)|查询全市场合约完整静态信息的应答|
|[OnQueryLatestIndexData](#onquerylatestindexdata)|查询合约最新指数的应答|
|[OnQueryLatestMarketData](#onquerylatestmarketdata)|查询合约最新现货快照的应答|
|[OnQueryTickersPriceInfo](#onquerytickerspriceinfo)|查询最新价的应答|
|[OnQueryHkStaticInfo](#OnQueryHkStaticInfo)|查询港股基础静态信息响应|
|[OnQueryBJGZStaticInfo](#OnQueryBJGZStaticInfo)|查询股转基础静态信息响应|
|[OnRebuildSzData](#OnRebuildSzData)|查询重建回补深市逐笔数据应答|

## SPI接口描述
### <a id="onerror">OnError</a>
```cpp
/**
 *   错误应答
 *   @attention                    此函数只有在服务器发生错误时才会调用，一般无需用户处理
 *   @param error_info             当服务器响应发生错误时的具体错误内容
 */
virtual void OnError(const EMTRspInfoStruct* error_info) {};
```
### <a id="onheartbeat">OnHeartBeat</a>
```cpp
/**
*   心跳应答
*   @attention                     服务端会向客户端定时发送心跳消息，来检测客户端是否在线
*   @return                        该函数返回为void
*/
virtual void OnHeartBeat() = 0;
```
### <a id="onindexdata">OnIndexData</a>
```cpp
/**
*   指数行情通知
*   @attention                    需要快速返回，否则阻塞后续消息，导致延时，当阻塞严重时，会触发断线
*   @param index_data             指数行情数据，只有指数行情数据相关字段
*/
virtual void OnIndexData(EMTIndexDataStruct* index_data) = 0;
```
### <a id="ondepthmarketdata">OnDepthMarketData</a>
```cpp
/**
*   深度行情通知，包含买一卖一队列
*   @attention                    需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
*   @param market_data            行情数据
*   @param bid1_qty               买一队列数据
*   @param bid1_count             买一队列的有效委托笔数
*   @param max_bid1_count         买一队列总委托笔数
*   @param ask1_qty               卖一队列数据
*   @param ask1_count             卖一队列的有效委托笔数
*   @param max_ask1_count         卖一队列总委托笔数
*/
virtual void OnDepthMarketData(EMTMarketDataStruct* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) = 0;
```
### <a id="onsubscribeallindexdata">OnSubscribeAllIndexData</a>
```cpp
/**
*   订阅全市场的指数行情应答
*   @attention                    需要快速返回
*   @param exchange_id            表示当前全部订阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*    @param error_info            订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*/
virtual void OnSubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct* error_info) = 0;
```
### <a id="onunsubscribeallindexdata">OnUnSubscribeAllIndexData</a>
```cpp
/**
*   退订全市场的指数行情应答
*   @attention                    需要快速返回
*   @param exchange_id            表示当前全部退订的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*    @param error_info            取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*/
virtual void OnUnSubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct* error_info) = 0;
```
### <a id="onsubindexdata">OnSubIndexData</a>
```cpp
/**
*   订阅指数行情应答
*   @attention                    每条订阅的合约对应一条订阅应答，需要快速返回，否则会阻塞后续消息，当阻塞严重时，会触发断线
*   @param ticker                 表示当前全阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @param error_info             订阅的合约编号代码
*    @param is_last               是否此次订阅的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*/
virtual void OnSubIndexData(EMTSpecificTickerStruct* ticker, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onunsubindexdata">OnUnSubIndexData</a>
```cpp
/**
*   退订指数行情应答
*   @attention                    每条订阅的合约对应一条退订应答，需要快速返回，否则会阻塞后续消息，当阻塞严重时，会触发断线
*   @param ticker                 订阅的合约编号代码，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @param error_info             取消订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*    @param is_last               是否此次取消订阅的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*/
virtual void OnUnSubIndexData(EMTSpecificTickerStruct* ticker, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onsubscribeallmarketdata">OnSubscribeAllMarketData</a>
```cpp
/**
*   订阅全市场股票快照行情应答
*   @attention                    需要快速返回 
*   @param exchange_id            表示当前全订阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*    @param error_info            当error_info为空，或者error_info.error_id为0时，表明没有错误
*/
virtual void OnSubscribeAllMarketData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct* error_info) = 0;
```
### <a id="onunsubscribeallmarketdata">OnUnSubscribeAllMarketData</a>
```cpp
/**
*   退订全市场的股票快照行情应答
*   @attention                    需要快速返回
*   @param exchange_id            表示当前全退订的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*    @param error_info            取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*/
virtual void OnUnSubscribeAllMarketData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct* error_info) = 0;
```
### <a id="onsubmarketdata">OnSubMarketData</a>
```cpp
/**
*   订阅快照行情应答，包括股票，基金，债券，权证和质押式回购 沪市港股通 深市港股通，股转数据
*   @attention                    每条订阅的合约对应一条订阅应答，需要快速返回，否则会阻塞后续消息，当阻塞严重时，会触发断线
*   @param ticker                 表示当前全阅的市场，EMQ_EXCHANGE_SH表示为上海全市场，EMQ_EXCHANGE_SZ表示为深圳全市场,EMQ_EXCHANGE_SHHK表示为沪市港股通，EMQ_EXCHANGE_SZHK表示为深市港股通，EMQ_EXCHANGE_BJGZK表示北交所股转市场
*   @param error_info             订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*    @param is_last               是否此次订阅的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*/
virtual void OnSubMarketData(EMTSpecificTickerStruct* ticker, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onunsubmarketdata">OnUnSubMarketData</a>
```cpp
/**
*   退订快照行情应答，包括股票，基金，债券，权证和质押式回购 沪市港股通 深市港股通
*   @attention                    每条订阅的合约对应一条订阅应答，需要快速返回，否则会阻塞后续消息，当阻塞严重时，会触发断线
*   @param ticker                 订阅的合约编号代码
*   @param error_info             取消订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*    @param is_last               是否此次取消订阅的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*/
virtual void OnUnSubMarketData(EMTSpecificTickerStruct* ticker, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onqueryalltickers">OnQueryAllTickers</a>
```cpp
/**
*   查询全市场合约部分静态信息的应答
*   @attention                    需要使用合约部分静态信息结构体EMTQuoteStaticInfo，用户自行传入一个空的结构体
*   @param qsi                    合约部分静态信息结构体
*   @param error_info             请求发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*   @param is_last                是否此次请求的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*   @return                       该函数返回为void
*/
virtual void OnQueryAllTickers(EMTQuoteStaticInfo* qsi, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onqueryalltickersfullinfo">OnQueryAllTickersFullInfo</a>
```cpp
/**
*   查询全市场合约完整静态信息的应答
*   @attention                    需要使用合约完整静态信息结构体EMTQutoFullInfo，用户自行传入一个空的结构体
*   @param qfi                    合约完整静态信息结构体
*   @param error_info             请求发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*   @param is_last                是否此次请求的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*   @return                       该函数返回为void
*/
virtual void OnQueryAllTickersFullInfo(EMTQuoteFullInfo* qfi, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onquerylatestindexdata">OnQueryLatestIndexData</a>
```cpp
/**
*   查询合约最新指数的应答
*   @attention                    需要使用最新数据结构体EMTIndexDataStruct
*   @param index_data             最新指数数据
*   @param error_info             请求发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*   @param is_last                是否此次请求的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*   @return                       该函数返回为void
*/
virtual void OnQueryLatestIndexData(EMTIndexDataStruct* index_data, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onquerylatestmarketdata">OnQueryLatestMarketData</a>
```cpp
/**
*   查询合约最新现货快照的应答
*   @attention                    需要使用最新数据结构体EMTMarketDataStruct
*   @param market_data            最新现货快照数据
*   @param error_info             请求发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
*   @param is_last                是否此次请求的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*   @return                       该函数返回为void
*/
virtual void OnQueryLatestMarketData(EMTMarketDataStruct* market_data, EMTRspInfoStruct* error_info, bool is_last) = 0;
```
### <a id="onquerytickerspriceinfo">OnQueryTickersPriceInfo</a>

```cpp
/**
* 查询最新价
* @attention        
* @param price_info               价格信息
* @param error_info               请求发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
* @param is_last                  是否此次请求的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
* @return                         该函数返回为void
*/
virtual void OnQueryTickersPriceInfo(EMTTickerPriceInfo* price_info, EMTRspInfoStruct* error_info, bool is_last) = 0;
```

### <a id="OnQueryHkStaticInfo">OnQueryHkStaticInfo</a>

```cpp
/**
* @brief  查询港股静态信息
* @attention                     需要使用港股静态信息结构体EMTHKStaticInfo                    
* @param  static_info            港股静态信息数据
* @param  error_info             订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
* @param  is_last                是否此次退订的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*/
virtual void OnQueryHkStaticInfo(EMTHKStaticInfo* static_info, EMTRspInfoStruct* error_info, bool is_last) {};
```

### <a id="OnQueryBJGZStaticInfo">OnQueryBJGZStaticInfo</a>

```cpp
/**
* @brief  查询股转静态信息
* @attention                     需要使用港股静态信息结构体EMTBJGZStaticInfo
* @param  static_info            港股静态信息数据
* @param  error_info             订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
* @param  is_last                是否此次退订的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
*/
virtual void OnQueryBJGZStaticInfo(EMTBJGZStaticInfo* static_info, EMTRspInfoStruct* error_info, bool is_last) {};
```

### <a id="OnRebuildSzData">OnRebuildSzData</a>

```cpp
/**
 * @brief  查询重建回补深市逐笔数据应答
 * 
 * @param  rebuild_data           重建数据响应信息
 * @param  error_info             错误信息，服务器响应发生错误的具体错误内容
 * @param  is_last                是否此次查询的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
 */
virtual void OnRebuildSzData(const EMTRebuildRespData* rebuild_data, const EMTRspInfoStruct* error_info, const bool is_last) {};

```



# 参数结构体

## 参数结构体索引

| 方法                                                      | 描述                 |
| :-------------------------------------------------------- | :------------------- |
| [EMTSpecificTickerStruct](#emtspecifictickerstruct)       | 指定的合约           |
| [EMTRspInfoStruct](#emtrspinfostruct)                     | 响应信息             |
| [EMTIndexDataStruct](#emtindexdatastruct)                 | 指数行情结构体       |
| [EMTMarketDataFundExData](#emtmarketdatafundexdata)       | 基金额外数据结构体   |
| [EMTMarketDataBondExData](#emtmarketdatabondexdata)       | 债券额外数据结构体   |
| [EMTMarketDataWarrantExData](#emtmarketdatawarrantexdata) | 权证额外数据         |
| [EMTMarketDataOptionExData](#emtmarketdataoptionexdata)   | 期权额外数据         |
| [EMTMarketDataHkExData](#EMTMarketDataHkExData)           | 港股通额外数据       |
| [EMTMarketDataBJGZExData](#EMTMarketDataBJGZExData)       | 北交所股转额外数据   |
| [EMTMarketDataStruct](#emtmarketdatastruct)               | 快照行情结构体       |
| [EMTQuoteStaticInfo](#emtquotestaticinfo)                 | 股票行情静态信息     |
| [EMTQuoteFullInfo](#emtquotefullinfo)                     | 股票行情全量静态信息 |
| [EMTMarketDataFullStruct](#emtmarketdatafullstruct)       | 现货快照             |
| [EMTHKStaticInfo](#EMTHKStaticInfo)                       | 港股基础静态信息     |
| [EMTBJGZStaticInfo](#EMTBJGZStaticInfo)                   | 北交所股转静态信息   |

## 参数结构体描述

### <a id="emtspecifictickerstruct">EMTSpecificTickerStruct</a>
```cpp
/// 指定的合约
using EMTSpecificTickerStruct = struct _EMTSpecificTickerStruct
{
    /// 交易所代码 
    EMQ_EXCHANGE_TYPE exchange_id;
    /// 合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾 
    char ticker[EMQ_TICKER_LEN];
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|exchange_id|[EMQ_EXCHANGE_TYPE](#emt_exchange_type)|SH、SZ|交易所代码 |
|ticker|char[16]|SH、SZ|合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾 |
### <a id="emtrspinfostruct">EMTRspInfoStruct</a>
```cpp
using EMTRspInfoStruct = struct _EMTRspInfoStruct
{
    /// 错误代码
    int32_t error_id;
    /// 错误信息
    char    error_msg[EMQ_ERR_MSG_LEN];
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|error_id|int32_t|SH、SZ|错误代码|
|error_msg|char[124]|SH、SZ|错误信息|
### <a id="emtindexdatastruct">EMTIndexDataStruct</a>
```cpp
/// 指数行情结构体
using EMTIndexDataStruct = struct _EMTIndexDataStruct
{
    /// 交易所代码
    EMQ_EXCHANGE_TYPE exchange_id;
    /// 合约代码(不包含交易所信息)，不带空格，以"\0"结尾
    char ticker[EMQ_TICKER_LEN];
    /// 行情时间，格式为YYYYMMDDHHMMSSssss
    int64_t data_time;
    /// 昨日收盘价
    double pre_close_price;
    /// 今日开盘价
    double open_price;
    /// 最新价
    double last_price;
    /// 最高价
    double high_price;
    /// 最低价
    double low_price;
    /// 数量，为总成交量(单位股，与交易所一致)
    int64_t qty;
    /// 成交金额，为总成交金额(单位元，与交易所一致)
    double turnover;

};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|exchange_id|[EMQ_EXCHANGE_TYPE](#emt_exchange_type)|SH、SZ|交易所代码|
|ticker|char[16]|SH、SZ|合约代码(不包含交易所信息)，不带空格，以"\0"结尾|
|data_time|int64_t|SH、SZ|行情时间，格式为YYYYMMDDHHMMSSssss|
|pre_close_price|double|SH、SZ|昨日收盘价|
|open_price|double|SH、SZ|今日开盘价|
|last_price|double|SH、SZ|最新价|
|high_price|double|SH、SZ|最高价|
|low_price|double|SH、SZ|最低价|
|qty|int64_t|SH、SZ|数量，为总成交量(单位股，与交易所一致)|
|turnover|double|SH、SZ|成交金额，为总成交金额(单位元，与交易所一致)|
### <a id="emtmarketdatafundexdata">EMTMarketDataFundExData</a>
```cpp
/// 基金额外数据结构体
using EMTMarketDataFundExData = struct _EMTMarketDataFundExData
{
    /// 基金实时参考净值(SH,SZ) 
    double iopv;
    /// ETF申购笔数(SH)    
    int32_t etf_buy_count;
    /// ETF赎回笔数(SH)
    int32_t etf_sell_count;
    /// ETF申购数量(SH)    
    double etf_buy_qty;
    /// ETF申购金额(SH)
    double etf_buy_money;
    /// ETF赎回数量(SH)
    double etf_sell_qty;
    /// ETF赎回金额(SH)
    double etf_sell_money;
    /// 基金T-1日净值(SZ) 
    double pre_iopv;
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|iopv|double|SH、SZ|基金实时参考净值(SH,SZ) |
|etf_buy_count|int32_t|SH|ETF申购笔数(SH)|
|etf_sell_count|int32_t|SH|ETF赎回笔数(SH)|
|etf_buy_qty|double|SH|ETF申购数量(SH)|
|etf_buy_money|double|SH|ETF申购金额(SH)|
|etf_sell_qty|double|SH|ETF赎回数量(SH)|
|etf_sell_money|double|SH|ETF赎回金额(SH)|
|pre_iopv|double|SZ|基金T-1日净值(SZ) |
### <a id="emtmarketdatabondexdata">EMTMarketDataBondExData</a>
```cpp
/// 债券额外数据结构体
using EMTMarketDataBondExData = struct _EMTMarketDataBondExData
{
    /// 债券加权平均委买价格(SH)
    double ma_bond_bid_price;
    /// 债券加权平均委卖价格(SH)
    double ma_bond_ask_price;
    /// 债券到期收益率(SH)
    double yield_to_maturity;
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|ma_bond_bid_price|double|SH|债券加权平均委买价格(SH)|
|ma_bond_ask_price|double|SH|债券加权平均委卖价格(SH)|
|yield_to_maturity|double|SH|债券到期收益率(SH)|
### <a id="emtmarketdatawarrantexdata">EMTMarketDataWarrantExData</a>
```cpp
/// 权证额外数据
using EMTMarketDataWarrantExData = struct _EMTMarketDataWarrantExData
{
    /// 权证执行的总数量(SH)
    double total_warrant_exec_qty;
    /// 权证跌停价格（元）(SH)
    double warrant_lower_price;
    /// 权证涨停价格（元）(SH)
    double warrant_upper_price;
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|total_warrant_exec_qty|double|SH|权证执行的总数量(SH) |
|warrant_lower_price|double|SH|权证跌停价格（元）(SH)|
|warrant_upper_price|double|SH|权证涨停价格（元）(SH) |
### <a id="emtmarketdataoptionexdata">EMTMarketDataOptionExData</a>
```cpp
/// 期权额外数据
using EMTMarketDataOptionExData = struct _EMTMarketDataOptionExData
{
    /// 波段性中断参考价(SH、SZ)
    double auction_price;
    /// 波段性中断集合竞价虚拟匹配量(SH、SZ)
    int64_t auction_qty;
    /// 最近询价时间(SH)
    int64_t last_enquiry_time;

    // 期权数据
    /// 昨日持仓量(张)(目前未填写)
    int64_t pre_total_long_positon;
    /// 持仓量(张)(目前未填写)
    int64_t total_long_positon;
    /// 昨日结算价(目前未填写)
    double pre_settl_price;
    /// 今日结算价(目前未填写)
    double settl_price;
};

```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|auction_price|double|SH SZ|波段性中断参考价 |
|auction_qty|int64_t|SH SZ|波段性中断集合竞价虚拟匹配量 |
|last_enquiry_time|int64_t|SH|最近询价时间(SH) |
|pre_total_long_positon|int64_t|SH、SZ|昨日持仓量(张)(目前未填写)|
|total_long_positon|int64_t|SH、SZ|持仓量(张)(目前未填写)|
|pre_settl_price|double|SH、SZ|昨日结算价(目前未填写)|
|settl_price|double|SH、SZ|今日结算价(目前未填写)|
### <a id="EMTMarketDataHkExData">EMTMarketDataHkExData</a>

```cpp
///港股通额外扩展数据
using EMTMarketDataHkExData = struct _EMTMarketDataHkExData
{
     
    ///参考价
    double refer_price;


    ///按盘价
    double nominal_price;

    ///买盘上限价
    double buyside_up_limit_price;

    ///买盘下限价
    double buyside_low_limit_price;

    ///卖盘上限价
    double sellside_up_limit_price;

    ///卖盘下限价
    double sellside_low_limit_price;

    ///冷静期开始时间  HHmmssSSS
    int32_t complex_event_start_time;

    ///冷静期结束时间 HHmmssSSS
    int32_t complex_event_end_time;

};
```

| 标识                  | 类型及长度 | 交易市场  | 描述           |
| :-------------------- | :--------- | :-------- | :------------- |
| referPrice            | double     | SHHK/SZHK | 参考价         |
| nominalPrice          | double     | SHHK/SZHK | 按盘价         |
| buySideUpLimitPrice   | double     | SHHK/SZHK | 买盘上限价     |
| buySideLowLimitPrice  | double     | SHHK/SZHK | 买盘下限价     |
| sellSideUpLimitPrice  | double     | SHHK/SZHK | 卖盘上限价     |
| sellSideLowLimitPrice | double     | SHHK/SZHK | 卖盘下限价     |
| complexEventStartTime | int32      | SHHK/SZHK | 冷静期开始时间 |
| complexEventEndTime   | int32      | SHHK/SZHK | 冷静期结束时间 |

### <a id="EMTMarketDataBJGZExData">EMTMarketDataBJGZExData</a>

```cpp
/// 北交所股转额外扩展数据
using EMTMarketDataBJGZExData = struct _EMTMarketDataBJGZExData
{
    ///静态市盈率
    double LYR;

    ///动态市盈率
    double TTM;

    ///价格升跌一 较昨日收盘价涨跌
    double price_change1;

    ///价格涨跌二 较上一笔成交涨跌
    double price_change2;

    ///合约持仓量
    long open_interest;

    ///第一位 char 交易状态‘N’表示正常状态；‘Y’ 表示首日挂牌/上市；‘D’表示新增股票交易；‘I’表示询价；‘F’表示申购
    ///第二位 char 证券级别 ‘T’表示对应证券是挂牌/上市公司股票；‘B’表示对应证券是两网公司及退市公司股票；‘O’表示对应证券是仅提供行权功能的期权；‘P’表示对应证券是持有人数存在 200 人限制的证券;(优先股)‘R’表示对应证券是其他类型的业务；
    ///   ‘F’表示是发行业务;’C’表示对应证券是提供协议转让的可转换公司债券。
    ///第三位 char 交易类型  ‘T’表示协议交易方式； * ‘M’表示做市交易方式；‘B’表示集合竞价+连续竞价交易方式； * ‘C’表示集合竞价交易方式；‘P’表示发行方式，‘O’表示其他类型。
    ///第四位 char  * 停牌标志  * ‘F’表示正常交易；‘T’表示停牌，不接受申报；‘H’表示停牌，接受申报。
    ///第五位 char ‘0’-基础层，‘1’-创新层，‘2’-北交所。对于优先股、可转债、指数等无市场层级的，本字段填空格。(预留)
    char ticker_relevant_info[8];
};

```

| 标识                 | 类型及长度 | 交易市场 | 描述                        |
| :------------------- | :--------- | :------- | :-------------------------- |
| LYR;                 | double     | BJGZ     | 静态市盈率                  |
| TTM                  | double     | BJGZ     | 动态市盈率                  |
| price_change1        | double     | BJGZ     | 价格升跌一 较昨日收盘价涨跌 |
| price_change2        | double     | BJGZ     | 价格升跌二 较上一笔成交涨跌 |
| open_interest        | long       | BJGZ     | 合约持仓量                  |
| ticker_relevant_info | char[8]    | BJGZ     | 合约相关信息                |

### <a id="emtmarketdatastruct">EMTMarketDataStruct</a>

```cpp
/// 快照行情结构体
using EMTMarketDataStruct = struct _EMTMarketDataStruct
{
    /// 交易所代码
    EMQ_EXCHANGE_TYPE exchange_id;
    /// 合约代码(不包含交易所信息)，不带空格。以"\0"结尾
    char ticker[EMQ_TICKER_LEN];

    // 价格
    /// 最新价
    double last_price;
    /// 昨日收盘价
    double pre_close_price;
    /// 今开盘价
    double open_price;
    /// 最高价
    double high_price;
    /// 最低价
    double low_price;
    /// 今收盘价(SH)
    double close_price;

    // 涨跌停
    /// 涨停价(SZ)
    double upper_limit_price;
    /// 跌停价(SZ)
    double lower_limit_price;
    /// 行情时间，格式为YYYYMMDDHHMMSSssss
    int64_t data_time;

    // 量额数据
    /// 数量，为总成交量(单位股，与交易所一致)
    int64_t qty;
    /// 成交金额，为总成交金额(单位元，与交易所一致)
    double turnover;
    /// 当日均价=(turnover/qty)    
    double avg_price;

    // 买卖盘
    /// 十档申买价
    double bid[10];
    /// 十档申卖价
    double ask[10];
    /// 十档申买量
    int64_t bid_qty[10];
    /// 十档申卖量
    int64_t ask_qty[10];

    /// 成交笔数
    int64_t trades_count;
    /// 当前交易状态说明(SH)
    char ticker_status[8];

    /// 委托买入总量(SH,SZ)
    int64_t total_bid_qty;
    /// 委托卖出总量(SH,SZ)
    int64_t total_ask_qty;
    /// 加权平均委买价格(SH,SZ)
    double ma_bid_price;
    /// 加权平均委卖价格(SH,SZ)
    double ma_ask_price;

    /// 买入撤单笔数(SH)
    int32_t cancel_buy_count;
    /// 卖出撤单笔数(SH)
    int32_t cancel_sell_count;
    /// 买入撤单数量(SH)
    double cancel_buy_qty;
    /// 卖出撤单数量(SH)
    double cancel_sell_qty;
    /// 买入撤单金额(SH)
    double cancel_buy_money;
    /// 卖出撤单金额(SH)
    double cancel_sell_money;
    /// 买入总笔数(SH)
    int64_t total_buy_count;
    /// 卖出总笔数(SH) 
    int64_t total_sell_count;
    /// 买入委托成交最大等待时间(SH)
    int32_t duration_after_buy;
    /// 卖出委托成交最大等待时间(SH)
    int32_t duration_after_sell;
    /// 买方委托价位数(SH) 
    int32_t num_bid_orders;
    /// 卖方委托价位数(SH)
    int32_t num_ask_orders;
    ///  额外数据联合体
    union {
        /// 债券额外数据
        EMTMarketDataBondExData bond;
        /// 基金额外数据
        EMTMarketDataFundExData fund;
        /// 权证额外数据
        EMTMarketDataWarrantExData warrant;
        /// 期权额外数据
        EMTMarketDataOptionExData opt;
        /// 港股额外数据
        EMTMarketDataHkExData hk;
        /// 北交所股转额外数据
        EMTMarketDataBJGZExData bjgz;
    };
    /// 证券类别；1：股票,基金,债券,权证,质押式回购；2：期权；决定了union是哪种数据类型
    EMQ_TICKER_TYPE data_type;

};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|exchange_id|[EMQ_EXCHANGE_TYPE](#emt_exchange_type)|ALL|交易所代码|
|ticker|char[16]|ALL|合约代码(不包含交易所信息)，不带空格。以"\0"结尾|
|last_price|double|ALL|最新价|
|pre_close_price|double| ALL       |昨日收盘价|
|open_price|double|ALL|今开盘价|
|high_price|double|ALL|最高价|
|low_price|double|ALL|最低价|
|close_price|double|SH|今收盘价(SH)|
|upper_limit_price|double|SZ|涨停价(SZ)|
|lower_limit_price|double|SZ|跌停价(SZ)|
|data_time|int64_t|SH、SZ|行情时间，格式为YYYYMMDDHHMMSSssss|
|qty|int64_t|ALL|数量，为总成交量(单位股，与交易所一致)|
|turnover|double|ALL|成交金额，为总成交金额(单位元，与交易所一致)|
|avg_price|double|ALL|当日均价=(turnover/qty)|
|bid|double[10]|ALL|十档申买价|
|ask|double[10]|ALL|十档申卖价|
|bid_qty|int64_t[10]|ALL|十档申买量|
|ask_qty|int64_t[10]|ALL|十档申卖量|
|trades_count|int64_t|ALL|成交笔数|
|ticker_status|char[8]|SH|当前交易状态说明(SH)|
|total_bid_qty|int64_t|SH、SZ|委托买入总量(SH,SZ)|
|total_ask_qty|int64_t|SH、SZ|委托卖出总量(SH,SZ)|
|ma_bid_price|double|SH、SZ|加权平均委买价格(SH,SZ)|
|ma_ask_price|double|SH、SZ|加权平均委卖价格(SH,SZ)|
|cancel_buy_count|int32_t|SH|买入撤单笔数(SH) |
|cancel_sell_count|int32_t|SH|卖出撤单笔数(SH)|
|cancel_buy_qty|double|SH|买入撤单数量(SH)|
|cancel_sell_qty|double|SH|卖出撤单数量(SH)|
|cancel_buy_money|double|SH|买入撤单金额(SH)|
|cancel_sell_money|double|SH|卖出撤单金额(SH)|
|total_buy_count|int64_t|SH|买入总笔数(SH)|
|total_sell_count|int64_t|SH|卖出总笔数(SH) |
|duration_after_buy|int32_t|SH|买入委托成交最大等待时间(SH)|
|duration_after_sell|int32_t|SH|卖出委托成交最大等待时间(SH)|
|num_bid_orders|int32_t|SH|买方委托价位数(SH) |
|num_ask_orders|int32_t|SH|卖方委托价位数(SH)|
|bond|[EMTMarketDataBondExData](#emtmarketdatabondexdata)|SH、SZ|债券额外数据|
|fund|[EMTMarketDataFundExData](#emtmarketdatafundexdata)|SH、SZ|基金额外数据|
|warrant|[EMTMarketDataWarrantExData](#emtmarketdatawarrantexdata)|SH、SZ|权证额外数据|
|opt|[EMTMarketDataOptionExData](#emtmarketdataoptionexdata)|SH、SZ|期权额外数据|
|hk|[EMTMarketDataHkExData](#EMTMarketDataHkExData)|SZHK/SZHK|港股通额外数据|
|bjgz|[EMTMarketDataBJGZExData](#EMTMarketDataBJGZExData)|BJGZ|股转额外数据|
|data_type|[EMQ_TICKER_TYPE](#emt_ticker_type)|ALL|证券类别：决定了union是哪种数据类型|
### <a id="emtquotestaticinfo">EMTQuoteStaticInfo</a>
```cpp
/// 股票行情静态信息
using EMTQuoteStaticInfo = struct _EMTQuoteStaticInfo {
    /// 交易所代码
    EMQ_EXCHANGE_TYPE exchange_id;
    /// 合约代码（不包含交易所信息），不带空格，以'\0'结尾
    char ticker[EMQ_TICKER_LEN];
    /// 合约名称
    char ticker_name[EMQ_TICKER_NAME_LEN];
    /// 合约类型
    EMQ_TICKER_TYPE ticker_type;
    /// 昨收盘 
    double  pre_close_price;
    /// 涨停板价
    double  upper_limit_price;
    /// 跌停板价
    double  lower_limit_price;
    /// 最小变动价位
    double  price_tick;
    /// 合约最小交易量(买)
    int32_t  buy_qty_unit;
    /// 合约最小交易量(卖) 
    int32_t sell_qty_unit;
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|ticker|char[16]|SH、SZ|合约代码（不包含交易所信息），不带空格，以'\0'结尾|
|ticker_name|char[64]|SH、SZ|合约名称|
|ticker_type|[EMQ_TICKER_TYPE](#emt_ticker_type)|SH、SZ|合约类型|
|pre_close_price|double|SH、SZ|昨收盘 |
|upper_limit_price|double|SH、SZ|涨停板价|
|lower_limit_price|double|SH、SZ|跌停板价|
|price_tick|double|SH、SZ|最小变动价位|
|buy_qty_unit|int32_t|SH、SZ|合约最小交易量(买)|
|sell_qty_unit|int32_t|SH、SZ|合约最小交易量(卖) |
### <a id="emtquotefullinfo">EMTQuoteFullInfo</a>
```cpp
/// 股票行情全量静态信息
using EMTQuoteFullInfo = struct _EMTQuoteFullInfo {
    /// 交易所代码 
    EMQ_EXCHANGE_TYPE exchange_id;
    /// 证券代码
    char ticker[EMQ_TICKER_LEN];
    /// 证券名称
    char ticker_name[EMQ_TICKER_NAME_LEN];
    /// 合约详细类型
    EMQ_SECURITY_TYPE security_type;
    /// 合约适当性类别
    EMQ_QUALIFICATION_TYPE ticker_qualification_class;
    /// 是否注册制(仅适用创业板股票，创新企业股票及存托凭证)
    bool is_registration;
    /// 是否具有协议控制架构(仅适用创业板股票，创新企业股票及存托凭证)
    bool is_VIE;
    /// 是否尚未盈利(仅适用创业板股票，创新企业股票及存托凭证)
    bool is_noprofit;
    /// 是否存在投票权差异(仅适用创业板股票，创新企业股票及存托凭证)
    bool is_weighted_voting_rights;
    /// 是否有涨跌幅限制(注：不提供具体幅度，可通过涨跌停价和昨收价来计算幅度)
    bool is_have_price_limit;
    /// 涨停价（仅在有涨跌幅限制时有效）
    double upper_limit_price;
    /// 跌停价（仅在有涨跌幅限制时有效）
    double lower_limit_price;
    /// 昨收价
    double pre_close_price;
    /// 价格最小变动价位
    double price_tick;
    /// 限价买委托数量上限
    int32_t bid_qty_upper_limit;
    /// 限价买委托数量下限
    int32_t bid_qty_lower_limit;
    /// 限价买数量单位
    int32_t bid_qty_unit;
    /// 限价卖委托数量上限
    int32_t ask_qty_upper_limit;
    /// 限价卖委托数量下限
    int32_t ask_qty_lower_limit;
    /// 限价卖数量单位
    int32_t ask_qty_unit;
    /// 市价买委托数量上限
    int32_t market_bid_qty_upper_limit;
    /// 市价买委托数量下限 
    int32_t market_bid_qty_lower_limit;
    /// 市价买数量单位
    int32_t market_bid_qty_unit;
    /// 市价卖委托数量上限
    int32_t market_ask_qty_upper_limit;
    /// 市价卖委托数量下限
    int32_t market_ask_qty_lower_limit;
    /// 市价卖数量单位
    int32_t market_ask_qty_unit;
    /// 保留字段
    uint64_t unknown[4];
};
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|ticker|char[16]|SH、SZ|证券代码|
|ticker_name|char[64]|SH、SZ|证券名称|
|security_type|[EMQ_SECURITY_TYPE](#emt_security_type)|SH、SZ|合约详细类型|
|ticker_qualification_class|[EMQ_QUALIFICATION_TYPE](#emt_qualification_type)|SH、SZ|合约适当性类别|
|is_registration|bool|SH、SZ|是否注册制(仅适用创业板股票，创新企业股票及存托凭证)|
|is_VIE|bool|SH、SZ|是否具有协议控制架构(仅适用创业板股票，创新企业股票及存托凭证) |
|is_noprofit|bool|SH、SZ|是否尚未盈利(仅适用创业板股票，创新企业股票及存托凭证)|
|is_weighted_voting_rights|bool|SH、SZ|是否存在投票权差异(仅适用创业板股票，创新企业股票及存托凭证)|
|is_have_price_limit|bool|SH、SZ|是否有涨跌幅限制(注：不提供具体幅度，可通过涨跌停价和昨收价来计算幅度)|
|upper_limit_price|double|SH、SZ|涨停价（仅在有涨跌幅限制时有效）|
|lower_limit_price|double|SH、SZ|跌停价（仅在有涨跌幅限制时有效）|
|pre_close_price|double|SH、SZ|昨收价|
|price_tick|double|SH、SZ|价格最小变动价位|
|bid_qty_upper_limit|int32_t|SH、SZ|限价买委托数量上限|
|bid_qty_lower_limit|int32_t|SH、SZ|限价买委托数量下限|
|bid_qty_unit|int32_t|SH、SZ|限价买数量单位|
|ask_qty_upper_limit|int32_t|SH、SZ|限价卖委托数量上限|
|ask_qty_lower_limit|int32_t|SH、SZ|限价卖委托数量下限|
|ask_qty_unit|int32_t|SH、SZ|限价卖数量单位|
|market_bid_qty_upper_limit|int32_t|SH、SZ|市价买委托数量上限|
|market_bid_qty_lower_limit|int32_t|SH、SZ|市价买委托数量下限 |
|market_bid_qty_unit|int32_t|SH、SZ|市价买数量单位|
|market_ask_qty_upper_limit|int32_t|SH、SZ|市价卖委托数量上限|
|market_ask_qty_lower_limit|int32_t|SH、SZ|市价卖委托数量下限|
|market_ask_qty_unit|int32_t|SH、SZ|市价卖数量单位|
|unknown|uint64_t[4]|SH、SZ|保留字段|
### <a id="emtmarketdatafullstruct">EMTMarketDataFullStruct</a>
```cpp
/// 现货快照
typedef struct _EMTMarketDataFullStruct
{
    /// 现货快照
    EMTMarketDataStruct marketData;                                        
    /// 买一档委托总笔数
    uint32_t max_bid1_count;                                            
    /// 买一档委托数
    uint32_t bid1_count;                                                
    /// 买一档委托详情
    int64_t bid1_qty[50];                                                
    /// 卖一档委托总笔数
    uint32_t max_ask1_count;                                            
    /// 卖一档委托数
    uint32_t ask1_count;                                                
    /// 卖一档委托详情
    int64_t ask1_qty[50];                                                
}EMTMarketDataFullStruct;
```
|标识|类型及长度|交易市场|描述|
|:---|:---|:---|:---|
|marketData|[EMTMarketDataStruct](#emtmarketdatastruct)|ALL|现货快照|
|max_bid1_count|uint32_t|ALL|买一档委托总笔数|
|bid1_count|uint32_t|ALL|买一档委托数|
|bid1_qty|int64_t[50]| ALL      |买一档委托详情|
|max_ask1_count|uint32_t|ALL|卖一档委托总笔数|
|ask1_count|uint32_t| ALL      |卖一档委托数|
|ask1_qty|int64_t[50]|ALL|卖一档委托详情|

### <a id="EMTHKStaticInfo">EMTHKStaticInfo</a>

```cpp
/// 港股静态信息数据
using EMTHKStaticInfo = struct _EMTHKStaticInfo
{
    
    /// 交易所代码
    EMQ_EXCHANGE_TYPE exchange_type;

    /// 证券类型
    EMQ_TICKER_TYPE ticker_type;

    /// 合约代码（不包含交易所信息），不带空格，以'\0'结尾
    char ticker[EMQ_HK_TICKER_LEN];

    /// ISIN 代码
    char isin_ticker[EMQ_TICKER_LEN];

    /// 英文证券名简称
    char short_form_ticke_en[EMQ_TICKER_LEN];

    /// 市场种类 
    //MAIN- 主板 GEM-创业板
    //ETS-扩充交易证券 NASD-Nasdaq AMX 市场
    char market_type[4];

    /// 证券类别
    ///BOND    Bond 债券
    ///BWRT    Basket Warrant 一篮子权证
    ///EQTY    Equity  股本
    ///TRST    Trust  信托
    ///WRNT    Warrant  权证
    char symbol_type[4];


    /// 辅助证券代码  权证的正股代码
    char assist_symbol[5];

    /// 货币种类
    ///港币：HKD； 美元：USD；人民币：CNY；澳币：AUD；加币：CAD ; 日圆：JPY;新加坡币：SGD ; 英镑 : GBP ; 欧元：EUR
    char currency_type[3];

    /// 货币单位
    ///货币单位表示前收盘价格字段的单位，当货币单位取值为 0 时，前收盘价格=前收盘价格字段取值×10 的 0 次幂，当货币单位取值为 1 时，前收盘价格=前收盘价格字段取值×10 的 1 次幂，依次类推。
    char currency_unit;

    /// 面值货币种类 港币：HKD； 美元：USD；人民币：CNY
    char currency_denomination_type[3];

    /// 保留字段
    char reserved1[4];

    /// 上市日期 YYYYMMDD
    char issueDate[8];

    /// 买卖单位
    ///整手数
    ///买卖申报数量必须是该字段的整数倍。
    int32_t round_lot;

    /// 保留字段
    char reserved2[4];

    /// 面值 股票面值 单位：元
    double denomination;

    /// 利息  对于债券填写交收日利息 仅针对利息通过 CCASS 系统来结算的债券
    double interest;

    /// 前收盘价 当日除权（息）后收盘价格
    double pre_close_price;

    ///  保留字段
    char reserverd3[16];

    ///第一位有效Y表示停牌 N表示非停牌
    ///第二位有效 Y表示参与市场波动调节机制 N表示不参与
    ///第三位有效 Y表示参与收市竞价交易时段 N表示不参与
    ///第四位有效 Y表示参与开市前时段优化 N表示不参与
    ///第五至六位有效 价差代码,不足两位左补零。
    ///第七至八位有效 市场波动调节机制类别
    char text[64];

    ///中文证券简称 GBK编码
    char ticker_name_zh[64];

    ///英文证券全称
    char ticker_name_en[64];

};

```

| 标识                       | 类型及长度                              | 交易市场   | 描述                                                         |
| :------------------------- | :-------------------------------------- | :--------- | :----------------------------------------------------------- |
| exchange_type              | [EMQ_EXCHANGE_TYPE](#emt_exchange_type) | SHHK、SZHK | 交易所代码                                                   |
| ticker_type                | [EMQ_TICKER_TYPE](#emt_ticker_type)     | SHHK、SZHK | 证券类型                                                     |
| ticker                     | char[8];                                | SHHK、SZHK | 合约代码（不包含交易所信息），不带空格，以'\0'结尾           |
| isin_ticker                | char[16]                                | SHHK、SZHK | ISIN 代码                                                    |
| short_form_ticke_en        | char[16]                                | SHHK、SZHK | 英文证券名简称                                               |
| market_type                | char[4]                                 | SHHK、SZHK | 市场种类 MAIN- 主板 GEM-创业板   //ETS-扩充交易证券  NASD-Nasdaq AMX 市场 |
| symbol_type                | char[4]                                 | SHHK、SZHK | 证券类别<br/>BOND    Bond 债券<br/> BWRT    Basket Warrant 一篮子权证<br/> EQTY    Equity  股本<br/> TRST    Trust  信托<br/> WRNT    Warrant  权证 |
| assist_symbol              | char[5]                                 | SHHK、SZHK | 辅助证券代码  权证的正股代码                                 |
| currency_type              | char[3]                                 | SHHK、SZHK | 货币种类  港币：HKD； 美元：USD；人民币：CNY；澳币：AUD；加币：CAD ; 日圆：JPY;新加坡币：SGD ; 英镑 : GBP ; 欧元：EUR |
| currency_unit              | char                                    | SHHK、SZHK | 货币单位                                                     |
| currency_denomination_type | char[3]                                 | SHHK、SZHK | 面值货币种类 港币：HKD； 美元：USD；人民币：CNY              |
| reserved1                  | char[4]                                 | SHHK、SZHK | 保留字段                                                     |
| issueDate                  | char[8]                                 | SHHK、SZHK | 上市日期 YYYYMMDD                                            |
| round_lot                  | int32_t                                 | SHHK、SZHK | 买卖单位   //整手数限价买委托数量上限  //买卖申报数量必须是该字段的整数倍。限价买委托数量下限 |
| reserved2                  | char[4]                                 | SHHK、SZHK | 保留字段                                                     |
| denomination               | double                                  | SHHK、SZHK | 面值 股票面值 单位：元                                       |
| interest                   | double                                  | SHHK、SZHK | 利息  对于债券填写交收日利息 仅针对利息通过 CCASS 系统来结算的债券 |
| pre_close_price            | double                                  | SHHK、SZHK | 前收盘价 当日除权（息）后收盘价格                            |
| reserverd3                 | char[16]                                | SHHK、SZHK | 保留字段                                                     |
| text                       | char[64]                                | SHHK、SZHK | 备注                                                         |
| ticker_name_zh             | char[64]                                | SHHK、SZHK | 中文证券简称 GBK编码                                         |
| ticker_name_en             | char[64]                                | SHHK、SZHK | 英文证券全称                                                 |

### <a id="EMTBJGZStaticInfo">EMTBJGZStaticInfo</a>

```cpp
/// 股转静态信息数据
using EMTBJGZStaticInfo = struct _EMTBJGZStaticInfo
{
    /// 交易所代码
    EMQ_EXCHANGE_TYPE exchange_type;
    /// 证券类型
    EMQ_TICKER_TYPE ticker_type;
    /// 合约代码（不包含交易所信息），不带空格，以'\0'结尾
    char ticker[8];
    ///char[8] 证券简称 GBK
    char ticker_name[8];
    /// char[6]  行业种类 
    /// 对于股票：左边第一位为字母，表示门类；左边第二、三位为数字，表示大类；其他位为空。
    /// 具体定义请参见中国证监会颁布的《上市公司行业分类指引》
    char industry_type[6];
    /// char[2] 货币种类
    /// ‘00’人民币，‘02’美元
    char currency_type[2];
    /// double 面值
    /// 对于股票，每股面值为 1元；对于可转债，每张面值为 100 元
    double denomination;
    /// int64 总股本
    int64_t total_share_capital;
    /// int64 非限售股本
    int64_t nonrestricted_share_capital;
    /// double 上年每股收益
      /// 对于股票，存放上年每股利润
    double last_year_eps;
    ///  double 今年每股收益  预留
    double this_year_eps;
    /// double 经手费率
    /// 为全国股转公司及北交所本身收取或向投资者代收的交易费用费率，该费率仅为参考，实际费率以相关收费管理规定为准
    double transaction_rate;
    /// double 印花税率
    double stamp_duty_rate;
    /// double 过户费率
    double transfer_rate;
    /// int32 挂牌日期 CCYYMMDD
    int32_t list_date;
    /// int32  起息日 CCYYMMDD
    /// 对于优先股和可转债，该字段存放其起息日；对于要约业务，该字段存放其要约开始日期；对于发行业务，该字段存放其询价开始日期
    int32_t start_date;
    /// int32 到期日 CCYYMMDD
    /// 对于要约业务，该字段存放其要约结束日期；对于发行业务，该字段存放其询价结束日期
    int32_t end_date;
    /// int32  每笔限量 申报数量上限
    int32_t declare_upper_limit;
    /// int32 买数量单位
    int32_t buy_quantity_unit;
    /// int32 卖数量单位
    int32_t sell_quantity_unit;
    /// int32 最小申报数量
    /// 存放除做市商外其他投资者在正常交易时段的每笔最小申报数量
    int32_t min_declare_count;
    /// int32 限价参数类型
    /// 0 表示 限价参数的值为价格；1 为比例数
    int32_t param_type;
    /// double 价格挡位
    /// 申报价格的最小变动单位，例如 0.01 元 RMB。
    double tick_size;
    /// double 首笔交易限价参数
    /// 具体参考北交所接口文档
    double first_trade_limit_price_param;
    /// double 后续交易限价参数
    /// 具体参考北交所接口文档
    double sub_trade_limit_price_param;
    /// double 涨停价格
    /// 为申报有效价格范围的价格上限，如果 =99999.99（币种为人民币）或 99999.999（币种为美元），则相应的证券没有涨幅限制。对于要约业务，存放其收购/回购价格
    double upper_limit_price;
    /// double 跌停价格
    /// 为申报有效价格范围的价格下限 如果=0.000，则相应的证券没有跌幅限制。对于要约业务，存放其收购/回购价格
    double lower_limit_price;
    /// double 折合比例
    /// 对于优先股，该字段存放其票面股息率（%）；对于可转债，该字段存放票面利率（%）。
    double conv_ratio;
    /// char 成分股标志 预留
    char constituent_stock_flag;
    /// char 交易状态
    /// ‘N’表示正常状态；‘Y’表示首日挂牌/上市；‘D’表示新增股票交易；‘I’表示询价；‘F’表示申购
    char trade_status;
    /// char 证券级别
    /// ‘T’表示对应证券是挂牌/上市公司股票；‘B’表示对应证券是两网公司及退市公司股票；
    /// ‘O’表示对应证券是仅提供行权功能的期权；‘P’表示对应证券是持有人数存在 200 人限制的证券;(优先股)
    /// ‘R’表示对应证券是其他类型的业务；
    /// ‘F’表示是发行业务;’C’表示对应证券是提供协议转让的可转换公司债券。
    char symbol_type;
    ///  char 交易类型
    /// ‘T’表示协议交易方式；
    /// ‘M’表示做市交易方式；‘B’表示集合竞价+连续竞价交易方式；
    /// ‘C’表示集合竞价交易方式；‘P’表示发行方式，‘O’表示其他类型。
    char trade_type;
    /// char 停牌标志
    /// ‘F’表示正常交易；‘T’表示停牌，不接受申报；‘H’表示停牌，接受申报。
    char suspend_flag;
    ///  char 除权除息标志 
    /// ‘N’表示正常状态；‘E’表示除权；‘D’表示除息；‘A’表示除权除息。
    char ex_divide_flag;
    /// char 网络投票标志 预留
    char network_note_flag;
    /// char 市场层级
    /// ‘0’-基础层，‘1’-创新层，‘2’-北交所。对于优先股、可转债、指数等无市场层级的，本字段填空格。(预留字段)
    char market_level;
    ///  char[2] 交易场所 预留
    char trading_ring[2];
    /// char 是否为融资标的 (预留字段)
    /// ‘Y’-是，‘N’-否
    char is_financing;
    /// char 是否为融券标的 (预留字段)
    /// ‘Y’-是，‘N’-否
    char is_securities_loan;
    /// char 是否当日可融资 (预留字段)
    /// ‘Y’-是，‘N’-否
    char is_financing_today;
    /// char 是否当日可融券 (预留字段)
    /// ‘Y’-是，‘N’-否
    char is_securities_loan_today;
    /// 预留字段
    char reserved[2];
    /// int32 交易单位 申报数量的单位
    int32_t trade_unit;
    /// char[4] 其他业务状态
    /// 详见北交所NQXX接口文档
    char other_status[4];
    
    /// int32 做市商数量
    /// 对于有做市商提供做市服务的证券揭示其做市商数量，对于没有做市商提供做市服务的证券该字段的值默认为 0
    int32_t market_maker_count;
    /// int32
    /// 记录更新时间 HHMMSS
    int32_t update_time;
    /// double 大宗交易价格上限 预留
    double block_trade_upper_limit_price;
    /// double 大宗交易价格下限 预留
    double block_trade_lower_limit_price;

};
```

| 标识                           | 类型及长度                              | 交易市场 | 描述                                               |
| :----------------------------- | :-------------------------------------- | :------- | :------------------------------------------------- |
| exchange_type                  | [EMQ_EXCHANGE_TYPE](#emt_exchange_type) | BJGZ     | 交易所代码                                         |
| ticker_type                    | [EMQ_TICKER_TYPE](#emt_ticker_type)     | BJGZ     | 证券类型                                           |
| ticker                         | char[8];                                | BJGZ     | 合约代码（不包含交易所信息），不带空格，以'\0'结尾 |
| ticker_name                    | char[8]                                 | BJGZ     | 证券简称 GBK                                       |
| industry_type                  | char[6]                                 | BJGZ     | 行业种类                                           |
| currency_type                  | char[2]                                 | BJGZ     | 货币种类                                           |
| denomination                   | double                                  | BJGZ     | 面值                                               |
| total_share_capital            | double                                  | BJGZ     | 总股本                                             |
| nonrestricted_share_capital    | double                                  | BJGZ     | 非限售股本                                         |
| last_year_eps                  | double                                  | BJGZ     | 上年每股收益                                       |
| this_year_eps                  | double                                  | BJGZ     | 今年每股收益（预留）                               |
| transaction_rate               | double                                  | BJGZ     | 经手费率                                           |
| stamp_duty_rate                | double                                  | BJGZ     | 印花税率                                           |
| transfer_rate                  | double                                  | BJGZ     | 过户费率                                           |
| list_date                      | int32                                   | BJGZ     | 挂牌日期 CCYYMMDD                                  |
| start_date                     | int32                                   | BJGZ     | 起息日 CCYYMMDD                                    |
| end_date                       | int32                                   | BJGZ     | 到期日 CCYYMMDD                                    |
| declare_upper_limit            | int32                                   | BJGZ     | 每笔限量 申报数量上限                              |
| buy_quantity_unit              | int32                                   | BJGZ     | 买数量单位                                         |
| sell_quantity_unit             | int32                                   | BJGZ     | 卖数量单位                                         |
| min_declare_count              | int32                                   | BJGZ     | 最小申报数量                                       |
| param_type                     | int32                                   | BJGZ     | 限价参数类型                                       |
| tick_size                      | double                                  | BJGZ     | 价格挡位                                           |
| first_trade_limit_price_param  | double                                  | BJGZ     | 首笔交易限价参数                                   |
| sub_trade_limit_price_param    | double                                  | BJGZ     | 后续交易限价参数                                   |
| upper_limit_price              | double                                  | BJGZ     | 涨停价格                                           |
| lower_limit_price              | double                                  | BJGZ     | 跌停价格                                           |
| conv_ratio                     | double                                  | BJGZ     | 折合比例                                           |
| constituent_stock_flag;        | char                                    | BJGZ     | 成分股标志（预留）                                 |
| trade_status                   | char                                    | BJGZ     | 交易状态                                           |
| symbol_type                    | char                                    | BJGZ     | 证券类型                                           |
| trade_type                     | char                                    | BJGZ     | 交易类型                                           |
| suspend_flag                   | char                                    | BJGZ     | 停牌标志                                           |
| ex_divide_flag                 | char                                    | BJGZ     | 除息除权标志                                       |
| network_note_flag              | char                                    | BJGZ     | 网络投票标志 （预留）                              |
| market_level                   | char                                    | BJGZ     | 市场层级 （预留）                                  |
| trading_ring                   | char[2]                                 | BJGZ     | 交易场所（预留）                                   |
| is_financing                   | char                                    | BJGZ     | 是否为融资标的 (预留)                              |
| is_securities_loan             | char                                    | BJGZ     | 是否为融券标的 (预留)                              |
| is_financing_today             | char                                    | BJGZ     | 是否当日可融资 (预留)                              |
| is_securities_loan_today       | char                                    | BJGZ     | 是否当日可融券 (预留)                              |
| reserved                       | char[2]                                 | BJGZ     | 预留字段                                           |
| trade_unit                     | int32                                   | BJGZ     | 交易单位                                           |
| other_status                   | char[4]                                 | BJGZ     | 其他业务状态                                       |
| market_maker_count             | int32                                   | BJGZ     | 做市商数量                                         |
| update_time                    | int32                                   | BJGZ     | 记录更新时间 HHMMSS                                |
| block_trade_upper_limit_price  | double                                  | BJGZ     | 大宗交易价格上限 （预留）                          |
| block_trade_lower_limit_price; | double                                  | BJGZ     | 大宗交易价格下限 （预留）                          |



# 附录

## 字典定义

### <a id="emt_ticker_type">证券类别-EMQ_TICKER_TYPE</a>

| 枚举                       | 取值 | 定义说明           |
| :------------------------- | :--- | :----------------- |
| EMQ_TICKER_TYPE_STOCK      | 0    | 普通股票           |
| EMQ_TICKER_TYPE_INDEX      | 1    | 指数               |
| EMQ_TICKER_TYPE_FUND       | 2    | 基金               |
| EMQ_TICKER_TYPE_BOND       | 3    | 债券               |
| EMQ_TICKER_TYPE_OPTION     | 4    | 期权               |
| EMQ_TICKER_TYPE_TECH_STOCK | 5    | 科创板股票（上海） |
| EMQ_TICKER_TYPE_REPURCHASE | 6    | 质押式回购         |
| EMQ_TICKER_TYPE_WARRANT    | 7    | 权证               |
| EMQ_TICKER_TYPE_HK_STOCK   | 8    | 港股通             |
| EMQ_TICKER_TYPE_BJGZ       | 9    | 北交所股转         |
| EMQ_TICKER_TYPE_UNKNOWN    | 100  | 未知类型           |


### <a id="emt_exchange_type">交易所类型-EMQ_EXCHANGE_TYPE</a>

| 枚举                 | 取值 | 定义说明           |
| :------------------- | :--- | :----------------- |
| EMQ_EXCHANGE_SH      | 1    | 上证               |
| EMQ_EXCHANGE_SZ      | 2    | 深证               |
| EMQ_EXCHANGE_SHHK    | 3    | 沪市港股通         |
| EMQ_EXCHANGE_SZHK    | 4    | 深市港股通         |
| EMQ_EXCHANGE_BJGZ    | 5    | 北交所股转行情     |
| EMQ_EXCHANGE_UNKNOWN | 100  | 不存在的交易所类型 |

### <a id="emt_qualification_type">合约适当性类别-EMQ_QUALIFICATION_TYPE</a>

| 枚举                           | 取值 | 定义说明                               |
| :----------------------------- | :--- | :------------------------------------- |
| EMQ_QUALIFICATION_PUBLIC       | 0    | 公众投资者，合格投资者与机构投资者均可 |
| EMQ_QUALIFICATION_COMMON       | 1    | 仅合格投资者与公众投资者               |
| EMQ_QUALIFICATION_ORGANIZATION | 2    | 仅限机构投资者                         |
| EMQ_QUALIFICATION_UNKNOWN      | 3    | 未知，期权等可能为此种类型             |
###  <a id="emt_security_type">证券适当性枚举类型-EMQ_SECURITY_TYPE</a>

| 枚举                                    | 取值 | 定义说明             |
| :-------------------------------------- | :--- | :------------------- |
| EMQ_SECURITY_MAIN_BOARD                 | 0    | 主板股票             |
| EMQ_SECURITY_SECOND_BOARD               | 1    | 中小板股票           |
| EMQ_SECURITY_STARTUP_BOARD              | 2    | 创业板股票           |
| EMQ_SECURITY_INDEX                      | 3    | 指数                 |
| EMQ_SECURITY_TECH_BOARD                 | 4    | 科创板股票(上海)     |
| EMQ_SECURITY_STATE_BOND                 | 5    | 国债                 |
| EMQ_SECURITY_ENTERPRICE_BOND            | 6    | 企业债               |
| EMQ_SECURITY_COMPANEY_BOND              | 7    | 公司债               |
| EMQ_SECURITY_CONVERTABLE_BOND           | 8    | 转换债券             |
| EMQ_SECURITY_NATIONAL_BOND_REVERSE_REPO | 12   | 国债逆回购           |
| EMQ_SECURITY_ETF_SINGLE_MARKET_STOCK    | 14   | 本市场股票           |
| EMQ_SECURITY_ETF_INTER_MARKET_STOCK     | 15   | 跨市场股票           |
| EMQ_SECURITY_ETF_SINGLE_MARKET_BOND     | 17   | 本市场实物债券       |
| EMQ_SECURITY_ETF_GOLD                   | 19   | 黄金                 |
| EMQ_SECURITY_STRUCTURED_FUND_CHILD      | 24   | 分级基金子基金       |
| EMQ_SECURITY_SZSE_RECREATION_FUND       | 26   | 深交所仅申赎基金     |
| EMQ_SECURITY_STOCK_OPTION               | 29   | 个股期权             |
| EMQ_SECURITY_ETF_OPTION                 | 30   | ETF期权              |
| EMQ_SECURITY_ALLOTMENT                  | 100  | 配股                 |
| EMQ_SECURITY_MONETARY_FUND_SHCR         | 110  | 上交所申赎型货币基金 |
| EMQ_SECURITY_MONETARY_FUND_SHTR         | 111  | 上交所交易型货币基金 |
| EMQ_SECURITY_MONETARY_FUND_SZ           | 112  | 深交所货币基金       |
| EMQ_SECURITY_OTHERS                     | 255  | 其他                 |

