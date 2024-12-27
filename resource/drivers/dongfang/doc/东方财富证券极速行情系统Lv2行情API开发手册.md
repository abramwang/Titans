<font size="6">**东方财富证券极速行情系统Lv2行情API开发手册(Ver2.22.0)**</font><br>



<font size="6">文档说明</font>

<table><tbody>
    <tr>
        <th>日期</th><th>api 版本</th><th>修订摘要</th>
    </tr>
    <tr>
        <td>2024.09.25</td><td>2.22.0</td><td>API初版</td>
    </tr>
</table>
<font size="6">前言</font>

本接口规范用以指导开发通过 EMQAPILv2对接的方式访问东财快速行情系统，获取指数、现货的相关数据。

本接口规范描述内容包括可开展的业务、必要的运行指导以及详细的数据交换格式



#  1. EMQAPILv2介绍

本部分主要介绍极速行情系统的接口，包括：

* EMQAPILv2简介。
* EMQAPILv2运行模式。
* EMQAPILv2编程接口

## 1.1. EMQAPILv2简介

### 1.1.1. 背景

 为帮助客户实现快速交易并获取实时行情，东方财富证券推出了极速行情系统。客户端系统只需调用 EMQAPILv2，即可与极速行情系统进行对接。在接收到极速行情系统的行情数据后，EMQAPILv2 将主动回调客户端系统。

### 1.1.2. 简介

&emsp;&emsp;EMQAPILv2 是一个基于C++的类库，通过使用和扩展类库提供的接口来实现全部的行情功能，支持指数、现货快照相关数据。支持最新快照，逐笔行情等数据。

&emsp;&emsp;该类库包含以下文件：

|文件路径        |文件名                    |文件描述                  |
|:--------------|:------------------------|:-------------------------|
|demo           |CMakeLists.txt           |示例编译文件                |
|               |src/main.cpp         |示例主函数                 |
|               |src/quote_demo_lv2.cpp           |示例接口方法封装            |
|               |src/quote_demo_lv2.h             |示例接口方法头文件           |
|doc            |东方财富证券极速行情系统行情API Lv2开发手册               |                          |
| include |quote_api_lv2.h          |定义行情相关接口            |
|               |quote_struct_lv2.h       |定义L2行情相关数据结构        |
| |quote_sze_define.h |定义L2 深市相关数据结构 |
| |quote_sse_define.h |定义L2 沪市相关数据结构 |
|lib          |libemt_quote_api_lv2.so        |Linux行情接口动态链接库     |

### 1.1.3. 发行的平台

目前发布了以下操作系统平台的版本：

* Intel Linux：包括.h文件和.so文件。

如果需要其他操作系统版本，请联系行情专线。

## 1.2. 运行模式

### 1.2.1. 工作线程

​		EMQAPILv2 使用 UDP 模式接收行情。客户系统应用程序根据配置的行情类别创建多个线程，即应用程序主线程与 n 个数据处理线程（负责接收上游服务的行情数据并处理相应的回调）。

​		EMQAPILv2 提供的接口回调由数据处理线程驱动。通过实现 SPI 中的接口方法，客户系统能够从极速行情系统接收所需数据。

### 1.2.2. 运行流程

客户系统和极速行情系统的交互过程分为2个阶段：

* 初始化阶段。
* 功能调用阶段。

#### 1.2.2.1. 初始化阶段

&emsp;&emsp;在初始化阶段，客户系统的程序必须完成如下步骤（具体代码请参考开发实例）：

|顺序    |客户系统    |调用方法|
|:------|:-----------|:-------|
|1     |创建一个QuoteApi实例             |QuoteApiLv2::CreateQuoteApiLv2|
|2     |产生一个事件处理的实例；（步骤1返回）|                        |
|3     |注册一个事件处理的实例；           |QuoteApiLv2::RegisterSpi   |
|4 |登录服务 |QuoteApiLv2::Login |
|5     |配置需要接收的行情流                       |QuoteApiLv2::SetChannelConfig|
|6 |启动工作线程 |QuoteApiLv2::Start()|
|6     |设置开发代码     |相关业务回调函数实现 |

&emsp;&emsp;<font color=red>*开发代码由我司人员提供，需客户申请。*</font>

### 1.2.3. 时序示例

&emsp;&emsp;EMQAPILv2 提供了两个接口，分别为QuoteApiLv2和QuoteSpiLv2。

&emsp;&emsp;客户系统可以通过QuoteApiLv2发出操作请求，通过继承QuoteSpiLv2并重载回调函数来处理极速行情系统的回复或响应。

### 1.2.4. 连接方式

#### 1.2.4.1. 单账户登录
&emsp;&emsp;目前一个QuoteApi的示例，仅第一次成功登录有效。

&emsp;&emsp;API中登录Login()接口为同步的，返回值判断为0后，可以视为已经登录成功。



# 2. API公共接口

## 2.1. 业务支持索引
|方法|描述|
|:----|:----|
|[CreateQuoteApiLv2](#createquoteapilv2)|创建一个Quote API接口类实例|
|[Login](#login)|用户登录请求|
|[RegisterSpi](#registerspi)|注册回调接口|
|[SetChannelConfig](#SetChannelConfig)|设置通道接收配置|
|[Start](#Start)|行情接收启动接口|
|[Stop](#Stop)| 行情接收停止接口            |
|[Release](#Release)| 关闭接口，释放资源          |
|[GetApiVersion](#GetApiVersion)|获取版本信息|

## 2.2. API接口描述

### 2.2.1. <a id="createquoteapilv2">CreateQuoteApiLv2</a>

```cpp
/**
 * @brief  创建API接口类实例
 * @param  save_file_path             API日志保存路径
 * @param  log_level                  API日志输出级别
 *
 * @return                            EMQ API类的实例
 */
static QuoteApiLv2 *CreateQuoteApiLv2(const char *save_file_path, EMQ_LOG_LEVEL log_level = EMQ_LOG_LEVEL_DEBUG);
```

### 2.2.2. <a id="login">Login</a>
```cpp
/**
* @brief  发送登录消息到网关服务器
* 
* @attention                          此函数为同步阻塞模式，不需要异步等待登录成功，当函数返回即可进行后续操作，此api只能有一个连接
* @param  ip                          网关IP地址
* @param  port                        网关端口号
* @param  user_name                   登录用户账号
* @param  user_pwd                    登录用户密码
* @return                             登录是否成功  登录是否成功，"0"表示登录成功，非“0”登录失败，具体原因可查看本地日志。    
*/
virtual int32_t Login(const char *ip, const uint16_t port, const char *user_name, const char *user_pwd) = 0;
```
###  2.2.3. <a id="registerspi">RegisterSpi</a>
```cpp
/**
* @brief  注册回调接口类spi实例
* 
* @param  spi                        派生自回调接口类的实例
*/
virtual void RegisterSpi(QuoteSpiLv2* spi) = 0;
```
### 2.2.4. <a id="SetChannelConfig">SetChannelConfig</a>
```cpp
/**
* @brief 设置通道接收配置, 同时进行内置系统配置最优检查
* 
* @param  config						行情接收通道配置
* @param  num							配置数量
* @return                               配置是否成功，"0" 表示配置成功，非"0"表示配置失败
*/
virtual int32_t SetChannelConfig(EMQConfigLv2 *config, uint32_t num) = 0;
```

### 2.2.5. <a id="Start">Start</a>

```cpp
/**
 * @brief  								行情接收启动接口
 * 
 * @return 								启动是否成功，"0" 表示启动成功，非"0"表示启动失败
 */ 
virtual int32_t Start() = 0;
```

### 2.2.6. <a id="Stop">Stop</a>

```cpp
/**
 * @brief  								行情接收停止接口
 * 
 * @return 								停止是否成功，"0" 表示停止成功，非"0"表示停止失败
 */ 
virtual int32_t Stop() = 0;
```

### 2.2.7. <a id="Release">Release</a>

```cpp
/**
 * @brief  								关闭接口，释放资源
 */ 
virtual void Release() = 0;
```

### 2.2.8. <a id="GetApiVersion">GetApiVersion</a>

```cpp
/**
 * @brief  								获取版本信息
 * 
 * @return 								API版本号
 */ 
virtual const char *GetApiVersion() = 0;
```



# 3. API业务接口

## 3.1. 业务支持索引

|方法API|回调SPI|描述|
|:----|:----|-----|
| [GetPacketHardwareRXTs](#GetPacketHardwareRXTs) | 无      | 获取网卡收到行情包的硬件时间戳；仅可在对应的行情SPI回调内调用 |

## 3.2. API接口描述

### 3.2.1. <a id="GetPacketHardwareRXTs">GetPacketHardwareRXTs</a>

```cpp
/**
 * @brief  								获取网卡收到行情包的硬件时间戳；仅可在对应的行情SPI回调内调用
 * 
 * @return 								网卡收到行情包的硬件时间戳，单位ns
 */ 
virtual uint64_t GetPacketHardwareRXTs(void *packet) = 0;
```



# 4. SPI接口

## 4.1. 业务支持索引

| 方法                                  | 描述                   |
| ------------------------------------- | ---------------------- |
| [OnLv2SnapSze](#onlv2snapsze)         | 深交所快照行情         |
| [OnLv2TickSze](#onlv2ticksze)         | 深交所逐笔合并行情     |
| [OnLv2IndexSze](#onlv2indexsze)       | 深交所指数行情         |
| [OnLv2BondSnapSze](#onlv2bondsnapsze) | 深交所债券快照行情     |
| [OnLv2BondTickSze](#onlv2bondticksze) | 深交所债券逐笔合并行情 |
| [OnLv2TreeSze](#onlv2treesze)         | 深交所建树行情         |
| [OnLv2SnapSse](#onlv2snapsse)         | 上交所快照行情         |
| [OnLv2TickSse](#onlv2ticksse)         | 上交所逐笔合并行情     |
| [OnLv2IndexSse](#onlv2indexsse)       | 上交所指数行情         |
| [OnLv2BondSnapSse](#onlv2bondsnapsse) | 上交所债券快照行情     |
| [OnLv2BondTickSse](#onlv2bondticksse) | 上交所债券逐笔行情     |
| [OnLv2TreeSse](#onlv2treesse)         | 上交所建树行情         |

## 4.2. SPI接口描述

### 4.2.1. <a id="onlv2snapsze">OnLv2SnapSze</a>

```cpp
/**
 *   深交所快照行情
 *   @param snap   深交所快照行情数据
 */
virtual void OnLv2SnapSze(EMQSzeSnap *snap) {}
```

### 4.2.2. <a id="onlv2ticksze">OnLv2TickSze</a>

```cpp
/**
 *   深交所逐笔合并行情
 *   @param tick   深交所逐笔合并行情数据
 */
virtual void OnLv2TickSze(EMQSzeTick *tick) {}
```

### 4.2.3. <a id="onlv2indexsze">OnLv2IndexSze</a>

```cpp
/**
 *   深交所指数行情
 *   @param idx   深交所指数行情数据
 */
virtual void OnLv2IndexSze(EMQSzeIdx *idx) {}
```

### 4.2.4. <a id="onlv2bondsnapsze">OnLv2BondSnapSze</a>

```cpp
/**
 *   深交所债券快照行情
 *   @param bond_snap   深交所债券快照行情数据
 */
virtual void OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap) {}
```

### 4.2.5. <a id="onlv2bondticksze">OnLv2BondTickSze</a>

```cpp
/**
 *   深交所债券逐笔合并行情
 *   @param bond_tick   深交所债券逐笔合并行情数据
 */
virtual void OnLv2BondTickSze(EMQSzeBondTick *bond_tick) {}
```

### 4.2.6. <a id="onlv2treesze">OnLv2TreeSze</a>

```cpp
/**
 *   深交所建树行情
 *   @param tree   深交所建树行情数据
 */
virtual void OnLv2TreeSze(EMQSzeTree *tree) {}
```

### 4.2.7. <a id="onlv2snapsse">OnLv2SnapSse</a>

```cpp
/**
 *   上交所快照行情
 *   @param snap   上交所快照行情数据
 */
virtual void OnLv2SnapSse(EMQSseSnap *snap) {}
```

### 4.2.8. <a id="onlv2ticksse">OnLv2TickSse</a>

```cpp
/**
 *   上交所逐笔合并行情
 *   @param tick   上交所逐笔合并行情数据
 */
virtual void OnLv2TickSse(EMQSseTick *tick) {}
```

### 4.2.9. <a id="onlv2indexsse">OnLv2IndexSse</a>

```cpp
/**
 *   上交所指数行情
 *   @param idx   上交所指数行情数据
 */
virtual void OnLv2IndexSse(EMQSseIdx *idx) {}
```

### 4.2.10. <a id="onlv2bondsnapsse">OnLv2BondSnapSse</a>

```cpp
/**
 *   上交所债券快照行情
 *   @param bond_snap   上交所债券快照行情数据
 */
virtual void OnLv2BondSnapSse(EMQSseBondSnap *bond_snap) {}
```

### 4.2.11. <a id="onlv2bondticksse">OnLv2BondTickSse</a>

```cpp
/**
 *   上交所债券逐笔行情
 *   @param bond_tick   上交所债券逐笔行情数据
 */
virtual void OnLv2BondTickSse(EMQSseBondTick *bond_tick) {}
```

### 4.2.12. <a id="onlv2treesse">OnLv2TreeSse</a>

```cpp
/**
 *   上交所建树行情
 *   @param tree   上交所建树行情数据
 */
virtual void OnLv2TreeSse(EMQSseTree *tree) {}
```



# 5. 参数结构体

## 5.1. 参数结构体索引

#### 5.1.1. 公共结构体

| 结构体                        | 描述            |
| ----------------------------- | --------------- |
| [EMQConfigLv2](#EMQConfigLv2) | Lv2行情接收配置 |

#### 5.1.1.1. 深交所相关结构体

| 结构体                                                   | 描述               |
| -------------------------------------------------------- | ------------------ |
| [EMQSzePriceQuantityUnit](#emtquoteszepricequantityunit) | 深交所快照档位单元 |
| [EMQSzeHeartbeat](#emtquoteszeheartbeat)                 | 深交所心跳消息     |
| [EMQSzeHead](#emtquoteszehead)                           | 深交所消息协议头   |
| [EMQSzeSnap](#emtquoteszesnap)                           | 深交所快照消息     |
| [EMQSzeIdx](#emtquoteszeidx)                             | 深交所指数消息     |
| [EMQSzeTickOrder](#emtquoteszetickorder)                 | 深交所订单消息     |
| [EMQSzeTickExe](#emtquoteszetickexe)                     | 深交所成交消息     |
| [EMQSzeTick](#emtquoteszetick)                           | 深交所逐笔消息     |
| [EMQSzeTree](#emtquoteszetree)                           | 深交所建树消息     |
| [EMQSzeBondSnap](#emtquoteszebondsnap)                   | 深交债券快照消息   |
| [EMQSzeBondOrder](#EMQSzeBondOrder)                      | 深交所债券逐笔委托 |
| [EMQSzeBondExe](#EMQSzeBondExe)                          | 深交所债券逐笔成交 |
| [EMQSzeBondTick](#EMQSzeBondTick)                        | 深交所债券逐笔合并 |

#### 5.1.1.2. 上交所相关结构体

| 结构体                                                       | 描述                       |
| ------------------------------------------------------------ | -------------------------- |
| [EMQSseL2PriceQuantityUnit](#EMQSseL2PriceQuantityUnit)      | 上交所lev2快照档位单元     |
| [EMQSseHeartbeat](#EMQSseL2PriceQuantityUnit)                | 上交所心跳消息             |
| [EMQSseHead](#EMQSseHead)                                    | 上交所消息协议头           |
| [EMQSseSnap](#EMQSseSnap)                                    | 上交所lev2快照消息         |
| [EMQSseIdx](#EMQSseIdx)                                      | 上交所lev2指数消息         |
| [EMQSseL2TreePriceQuantityUnit](#EMQSseL2TreePriceQuantityUnit) | 上交所lev2建树档位单元     |
| [EMQSseTree](#EMQSseTree)                                    | 上交所lev2 建树消息        |
| [EMQSseL2BondPriceQuantityUnit](#EMQSseL2BondPriceQuantityUnit) | 上交所lev2债券快照档位单元 |
| [EMQSseBondSnap](#EMQSseBondSnap)                            | 上交所lev2债券快照         |
| [EMQSseBondTick](#EMQSseBondTick)                            | 债券逐笔                   |
| [EMQSseTick](#EMQSseTick)                                    | 逐笔合并                   |



## 5.2. 参数结构体描述

### 5.2.1. <a id="EMQConfigLv2">EMQConfigLv2</a>

```cpp
// Lv2行情接收配置
struct EMQConfigLv2 {
  bool enable;                 // 是否启用
  EMQRecvMode mode;            // 接收模式
  EMQType quote_type;          // 行情类型
  char eth_name[ETH_NAME_LEN]; // 网卡名
  char multicast_ip[IP_LEN];   // 组播地址
  uint16_t multicast_port;     // 组播端口
  int32_t rx_cpu_id;           // 用于接收的cpu id，-1表示不绑定
  int32_t handle_cpu_id;       // 用于处理的cpu id，-1表示不绑定
  int32_t rx_pkt_num;          // 接收内存大小 单位为4MB
  int32_t spsc_size;           // 缓存队列长度，单位K
};
```

| 标识           | 类型及长度                          | 描述                           |
| -------------- | ----------------------------------- | ------------------------------ |
| enable         | bool                                | 是否启用                       |
| mode           | [EMQRecvMode](#EMQRecvMode)         | 接收模式                       |
| quote_type     | [EMQType](#EMQType)                 | 行情类型                       |
| eth_name       | char[[ETH_NAME_LEN](#ETH_NAME_LEN)] | 网卡名                         |
| multicast_ip   | char[[IP_LEN](#IP_LEN)]             | 组播地址                       |
| multicast_port | uint16_t                            | 组播端口                       |
| rx_cpu_id      | int32_t                             | 用于接收的CPU ID，-1表示不绑定 |
| handle_cpu_id  | int32_t                             | 用于处理的CPU ID，-1表示不绑定 |
| rx_pkt_num     | int32_t                             | 接收内存大小 单位为4MB         |
| spsc_size      | int32_t                             | 缓存队列长度，单位K            |



### 5.2.2. <a id="emtquoteszepricequantityunit">EMQSzePriceQuantityUnit</a>

```cpp
// 深交所快照档位单元
struct EMQSzePriceQuantityUnit
{
    // 限价单和市价单都参与建树,对于建树行情,订单价格取值方法如下表：
    //  ****************************************************************************************************************
    //  |订单类型 \ 买卖方向 |                    买（1）          |                           卖（2）                  |
    //  |     市价（1）      |                    卖一价           |                           买一价                   |
    //  |     限价（2）      |   如果小于 3 的取为 3，否则取原值   |   如果大于 999999997，取为 999999997，否则取原值   |
    //  |    本方最优(U）    |                    买一价           |                           卖一价                   |
    //  *****************************************************************************************************************
    unsigned int                      m_price;                           // 价格，实际值需要除以 10000
    unsigned long long                m_quantity;                        // 数量，实际值需要除以 100
};
```

| 标识       | 类型及长度         | 描述                       |
| ---------- | ------------------ | -------------------------- |
| m_price    | unsigned int       | 价格，实际值需要除以 10000 |
| m_quantity | unsigned long long | 数量，实际值需要除以 100   |

### 5.2.3. <a id="emtquoteszeheartbeat">EMQSzeHeartbeat</a>

```cpp
// 深交所心跳消息
struct EMQSzeHeartbeat
{
    unsigned int                      m_sequence;                        // 行情序号
    unsigned int                      m_reserved_1;                      // 保留字段
    unsigned char                     m_message_type;                    // 行情类型
    unsigned char                     m_reserved_2[7];                   // 保留字段
};
```

| 标识           | 类型及长度       | 描述     |
| -------------- | ---------------- | -------- |
| m_sequence     | unsigned int     | 行情序号 |
| m_reserved_1   | unsigned int     | 保留字段 |
| m_message_type | unsigned char    | 行情类型 |
| m_reserved_2   | unsigned char[7] | 保留字段 |

### 5.2.4. <a id="emtquoteszehead">EMQSzeHead</a>

```cpp
// 深交所消息协议头
struct EMQSzeHead
{
    unsigned int                      m_sequence;                        // 行情序号
    unsigned int                      m_reserved_1;                      // 保留字段
    unsigned char                     m_message_type;                    // 行情类型
    //******************************************************************************************
    // 0:指数 1:股票 2:基金 3:债券 4:回购 5:权证 6:期货 7:外汇 8:利率 9:期权 10:其他
    //******************************************************************************************
    unsigned char                     m_security_type;                   // 证券类型
    //******************************************************************************************
    // 当 security_type == 0 时, 恒为0
    // 当 security_type == 1 时，0：主板 1：中小板 2：创业板 3：B股 4：H股 5：科创版 255:未分类
    // 当 security_type == 2 时，248：LOF基金 249：ETF基金 255：未分类
    // 当 security_type == 3 时，248：国债 249：可转债 255：未分类
    // 当 security_type == 4 时，248：国债回购 255：未分类
    // 当 security_type == 5/6/7/8/10时，255:没有意义
    // 当 security_type == 9 时，248：股票期权 249：ETF期权 255：未分类
    //******************************************************************************************
    unsigned char                     m_sub_security_type;               // 证券子类型
    unsigned char                     m_symbol[SZE_SYMBOL_LEN];          // 证券代码
    unsigned char                     m_exchange_id;                     // 交易所编号
    unsigned long long                m_quote_update_time;               // 行情更新时间
    //******************************************************************************************
    //格式如：YYYYMMDDHHMMSSsss（毫秒）
    //YYYY=0000-9999, MM=01-12,    DD=01-31, HH=00-23, MM=00-59, SS=00-60(秒)，sss=000-999(毫秒)。
    //******************************************************************************************
    unsigned short                    m_channel_num;                     // 频道号
    unsigned long long                m_sequence_num;                    // 包序号
    unsigned int                      m_md_stream_id;                    // 行情类别
};
```

| 标识                | 类型及长度                                       | 描述         |
| ------------------- | ------------------------------------------------ | ------------ |
| m_sequence          | unsigned int                                     | 行情序号     |
| m_reserved_1        | unsigned int                                     | 保留字段     |
| m_message_type      | unsigned char                                    | 行情类型     |
| m_security_type     | unsigned char                                    | 证券类型     |
| m_sub_security_type | unsigned char                                    | 证券子类型   |
| m_symbol            | unsigned char[[SZE_SYMBOL_LEN](#SZE_SYMBOL_LEN)] | 证券代码     |
| m_exchange_id       | unsigned char                                    | 交易所编号   |
| m_quote_update_time | unsigned long long                               | 行情更新时间 |
| m_channel_num       | unsigned short                                   | 频道号       |
| m_sequence_num      | unsigned long long                               | 包序号       |
| m_md_stream_id      | unsigned int                                     | 行情类别     |

### 5.2.5. <a id="emtquoteszesnap">EMQSzeSnap</a>

```cpp
// 深交所快照消息
struct EMQSzeSnap
{
    EMQSzeSnap()
    {
        memset(this, 0, sizeof(EMQSzeSnap));
    }

    EMQSzeHead                        m_head;                            // 包头
    unsigned char                     m_trading_status;                  // 交易状态码
    /******************************************************************************************
     * | Bit 位    | 返回值 | 交易所值 | 描述                                 |
     * | --------- | ------ | -------- | ------------------------------------ |
     * | Bid7-Bid4 | 0      | 'S'      | 表示启动(开市前)                     |
     * |           | 1      | 'O'      | 表示开盘集合竞价                     |
     * |           | 2      | 'T'      | 表示连续交易                         |
     * |           | 3      | 'B'      | 表示休市                             |
     * |           | 4      | 'C'      | 表示收盘集合竞价                     |
     * |           | 5      | 'E'      | 表示已闭市                           |
     * |           | 6      | 'H'      | 表示临时停牌                         |
     * |           | 7      | 'A'      | 表示盘后交易                         |
     * |           | 8      | 'V'      | 表示波动性中断                       |
     * |           | 15     | other    | 表示无意义                           |
     * | Bit3      | 0      | '0'      | 表示此正常状态                       |
     * |           | 1      | '1'      | 表示全天停牌                         |
     * |           | 0      | other    | 表示无意义，当成此产品可正常交易处理 |
     * | Bit2      | 0      |          | 预留位，值为 0                       |
     * | Bit1      | 0      |          | 预留位，值为 0                       |
     * | Bit0      | 0      |          | 预留位，值为 0                       |
    *******************************************************************************************/
    unsigned long long                m_total_trade_num;                 // 成交笔数
    unsigned long long                m_total_quantity;                  // 总成交量，          实际值需要除以 100
    unsigned long long                m_total_value;                     // 总成交额，          实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                 // 昨收价，            实际值需要除以 10000
    unsigned int                      m_last_price;                      // 最新价，            实际值需要除以 10000
    unsigned int                      m_open_price;                      // 开盘价，            实际值需要除以 10000
    unsigned int                      m_day_high_price;                  // 最高价，            实际值需要除以 10000
    unsigned int                      m_day_low_price;                   // 最低价，            实际值需要除以 10000
    unsigned int                      m_today_close_price;               // 收盘价，            实际值需要除以 10000
    unsigned int                      m_total_bid_weighted_avg_price;    // 买方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;              // 买总量，            实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;    // 卖方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;              // 卖总量，            实际值需要除以 100
    unsigned int                      m_lpv;                             // 基金 T-1 日净值，   实际值需要除以 10000
    unsigned int                      m_iopv;                            // 基金实时参考净值，  实际值需要除以 10000
    unsigned int                      m_upper_limit_price;               // 涨停价，            实际值需要除以 10000
    unsigned int                      m_low_limit_price;                 // 跌停价，            实际值需要除以 10000
    unsigned int                      m_open_interest;                   // 合约持仓量，        实际值需要除以 10000
    EMQSzePriceQuantityUnit      m_bid_unit[SZE_LEV2_DEPTH];        // 买方十档
    EMQSzePriceQuantityUnit      m_ask_unit[SZE_LEV2_DEPTH];        // 卖方十档

    /// 当 Ask_1_Qty=0 时，TotalAskQty 重置为 0；当 Bid_1_Qty=0 时，TotalBidQty 重置为 0。
};
```

| 标识                           | 类型及长度                                                   | 描述                                   |
| ------------------------------ | ------------------------------------------------------------ | -------------------------------------- |
| m_head                         | [EMQSzeHead](#EMQSzeHead)                                    | 包头                                   |
| m_trading_status               | unsigned char                                                | 交易状态码                             |
| m_total_trade_num              | unsigned long long                                           | 成交笔数                               |
| m_total_quantity               | unsigned long long                                           | 总成交量，实际值需要除以 100           |
| m_total_value                  | unsigned long long                                           | 总成交额，实际值需要除以 1000000       |
| m_pre_close_price              | unsigned int                                                 | 昨收价，实际值需要除以 10000           |
| m_last_price                   | unsigned int                                                 | 最新价，实际值需要除以 10000           |
| m_open_price                   | unsigned int                                                 | 开盘价，实际值需要除以 10000           |
| m_day_high_price               | unsigned int                                                 | 最高价，实际值需要除以 10000           |
| m_day_low_price                | unsigned int                                                 | 最低价，实际值需要除以 10000           |
| m_today_close_price            | unsigned int                                                 | 收盘价，实际值需要除以 10000           |
| m_total_bid_weighted_avg_price | unsigned int                                                 | 买方挂单均价，实际值需要除以 10000     |
| m_total_bid_quantity           | unsigned long long                                           | 买总量，实际值需要除以 100             |
| m_total_ask_weighted_avg_price | unsigned int                                                 | 卖方挂单均价，实际值需要除以 10000     |
| m_total_ask_quantity           | unsigned long long                                           | 卖总量，实际值需要除以 100             |
| m_lpv                          | unsigned int                                                 | 基金 T-1 日净值，实际值需要除以 10000  |
| m_iopv                         | unsigned int                                                 | 基金实时参考净值，实际值需要除以 10000 |
| m_upper_limit_price            | unsigned int                                                 | 涨停价，实际值需要除以 10000           |
| m_low_limit_price              | unsigned int                                                 | 跌停价，实际值需要除以 10000           |
| m_open_interest                | unsigned int                                                 | 合约持仓量，实际值需要除以 10000       |
| m_bid_unit                     | [EMQSzePriceQuantityUnit](#EMQSzePriceQuantityUnit)[[SZE_LEV2_DEPTH](#SZE_LEV2_DEPTH)] | 买方十档                               |
| m_ask_unit                     | [EMQSzePriceQuantityUnit](#EMQSzePriceQuantityUnit)[[SZE_LEV2_DEPTH](#SZE_LEV2_DEPTH)] | 卖方十档                               |

### 5.2.6. <a id="emtquoteszeidx">EMQSzeIdx</a>

```cpp
// 深交所指数消息
struct EMQSzeIdx
{
    EMQSzeIdx()
    {
        memset(this, 0, sizeof(EMQSzeIdx));
    }

    EMQSzeHead                   m_head;                            // 包头
    unsigned long long                m_total_trade_num;                 // 成交笔数
    unsigned long long                m_total_quantity;                  // 总成交量，  实际值需要除以 100
    unsigned long long                m_total_value;                     // 总成交额，  实际值需要除以 1000000
    unsigned int                      m_last_price;                      // 最新价，    实际值需要除以 10000
    unsigned int                      m_pre_close_price;                 // 昨收价，    实际值需要除以 10000
    unsigned int                      m_open_price;                      // 开盘价，    实际值需要除以 10000
    unsigned int                      m_day_high_price;                  // 最高价，    实际值需要除以 10000
    unsigned int                      m_day_low_price;                   // 最低价，    实际值需要除以 10000
    unsigned int                      m_today_close_price;               // 收盘价，    实际值需要除以 10000
    char                              m_reserved[5];                     // 保留字段
};
```

| 标识                | 类型及长度                | 描述                               |
| ------------------- | ------------------------- | ---------------------------------- |
| m_head              | [EMQSzeHead](#EMQSzeHead) | 包头                               |
| m_total_trade_num   | unsigned long long        | 成交笔数                           |
| m_total_quantity    | unsigned long long        | 总成交量，  实际值需要除以 100     |
| m_total_value       | unsigned long long        | 总成交额，  实际值需要除以 1000000 |
| m_last_price        | unsigned int              | 最新价，    实际值需要除以 10000   |
| m_pre_close_price   | unsigned int              | 昨收价，    实际值需要除以 10000   |
| m_open_price        | unsigned int              | 开盘价，    实际值需要除以 10000   |
| m_day_high_price    | unsigned int              | 最高价，    实际值需要除以 10000   |
| m_day_low_price     | unsigned int              | 最低价，    实际值需要除以 10000   |
| m_today_close_price | unsigned int              | 收盘价，    实际值需要除以 10000   |
| m_reserved          | char [5]                  | 保留字段                           |

### 5.2.7. <a id="emtquoteszetickorder">EMQSzeTickOrder</a>

```cpp
// 深交所订单消息
struct EMQSzeTickOrder
{
    EMQSzeTickOrder()
    {
        memset(this, 0, sizeof(EMQSzeTickOrder));
    }

    EMQSzeHead                   m_head;                            // 包头
    /// 对逐笔委托价格转换说明如下：
    //  ****************************************************************************************************************
    //  |订单类型 \ 买卖方向 |                    买（1）            |                           卖（2）                    |
    //  |     市价（1）      |                    直接透传           |                           直接透传                   |
    //  |    本方最优(U）    |                    直接透传           |                           直接透传                   |
    //  |     限价（2）      |   如果小于 3 的取为 3，否则取原值     |   如果大于 999999997，取为 999999997，否则取原值     |
    //  *****************************************************************************************************************
    unsigned int                      m_order_price;                     // 价格，  实际值需要除以 10000
    unsigned long long                m_order_quantity;                  // 数量，  实际值需要除以 100
    char                              m_side_flag;                       // 方向，‘1’=买 ‘2’=卖
    char                              m_order_type;                      // 订单类型
    //******************************************************************************************
    // '1'=市价  '2'=限价  'U'=本方最优
    //******************************************************************************************
    char                              m_reserved[15];                     // 保留字段
};
```

| 标识             | 类型及长度                | 描述                         |
| ---------------- | ------------------------- | ---------------------------- |
| m_head           | [EMQSzeHead](#EMQSzeHead) | 包头                         |
| m_order_price    | unsigned int              | 价格，  实际值需要除以 10000 |
| m_order_quantity | unsigned long long        | 数量，  实际值需要除以 100   |
| m_side_flag      | char                      | 方向，‘1’=买 ‘2’=卖          |
| m_order_type     | char                      | 订单类型                     |
| m_reserved       | char[15]                  | 保留字段                     |

### 5.2.8. <a id="emtquoteszetickexe">EMQSzeTickExe</a>

```cpp
// 深交所成交消息
struct EMQSzeTickExe
{
    EMQSzeTickExe()
    {
        memset(this, 0, sizeof(EMQSzeTickExe));
    }

    EMQSzeHead                        m_head;                           // 包头
    long long                         m_trade_buy_num;                  // 买方包序号
    long long                         m_trade_sell_num;                 // 卖方包序号
    unsigned int                      m_trade_price;                    // 价格，   实际值需要除以 10000
    long long                         m_trade_quantity;                 // 数量，   实际值需要除以 100
    char                              m_trade_type;                     // 成交类型
    //******************************************************************************************
    // '4'=撤销  'F'=成交
    //******************************************************************************************
};
```

| 标识             | 类型及长度                | 描述                        |
| ---------------- | ------------------------- | --------------------------- |
| m_head           | [EMQSzeHead](#EMQSzeHead) | 包头                        |
| m_trade_buy_num  | long long                 | 买方包序号                  |
| m_trade_sell_num | long long                 | 卖方包序号                  |
| m_trade_price    | unsigned int              | 价格，实际值需要除以 10000  |
| m_trade_quantity | long long                 | 数量，实际值需要除以 100    |
| m_trade_type     | char                      | 成交类型，'4'=撤销 'F'=成交 |

### 5.2.9. <a id="emtquoteszetick">EMQSzeTick</a>

```cpp
// 深交所逐笔消息
struct EMQSzeTick
{
    EMQSzeTickType m_tick_type;
    union {
        EMQSzeTickOrder *m_tick_order;
        EMQSzeTickExe *m_tick_exe;
    };
};
```

| 标识         | 类型及长度                            | 描述               |
| ------------ | ------------------------------------- | ------------------ |
| m_tick_type  | [EMQSzeTickType](#EMQSzeTickType)     | 深交所逐笔消息类型 |
| m_tick_order | [EMQSzeTickOrder](#EMQSzeTickOrder) * | 深交所订单消息     |
| m_tick_exe   | [EMQSzeTickExe](#EMQSzeTickExe) *     | 深交所成交消息     |

### 5.2.10. <a id="emtquoteszetree">EMQSzeTree</a>

```cpp
// 深交所建树消息
struct EMQSzeTree
{
    EMQSzeTree()
    {
        memset(this, 0, sizeof(EMQSzeTree));
    }

    EMQSzeHead                        m_head;                           // 包头
    long long                         m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，               实际值需要除以 100
    long long                         m_total_value;                    // 总成交额，               实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                // 昨收价，                 实际值需要除以 10000
    unsigned int                      m_last_price;                     // 最新价，                 实际值需要除以 10000
    unsigned int                      m_open_price;                     // 开盘价，                 实际值需要除以 10000
    unsigned int                      m_day_high_price;                 // 最高价，                 实际值需要除以 10000
    unsigned int                      m_day_low_price;                  // 最低价，                 实际值需要除以 10000
    unsigned int                      m_today_close_price;              // 收盘价，                 实际值需要除以 10000
    unsigned int                      m_total_bid_weighted_avg_price;   // 买方挂单均价，           实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;             // 买总量，                 实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;   // 卖方挂单均价，           实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;             // 卖总量，                 实际值需要除以 100
    unsigned int                      m_upper_limit_price;              // 涨停价，                 实际值需要除以 10000
    unsigned int                      m_low_limit_price;                // 跌停价，                 实际值需要除以 10000
    unsigned long long                m_market_open_total_bid;          // Bid 边市价单挂单总量，   实际值需要除以 100
    unsigned long long                m_market_open_total_ask;          // Ask 边市价单挂单总量，   实际值需要除以 100
    unsigned int                      m_bid_depth;                      // Bid 边总档位数
    unsigned int                      m_ask_depth;                      // Ask 边总档位数
    EMQSzePriceQuantityUnit      m_bid_unit[SZE_LEV2_DEPTH];       // 买方十档
    EMQSzePriceQuantityUnit      m_ask_unit[SZE_LEV2_DEPTH];       // 卖方十档
    char                              m_ch_gap_flag;                    // 0x01=此消息前逐笔消息有断号发生；0x00=逐笔消息序号连续，正常；
    char                              m_reserved[4];                    // 保留字段
};
```

| 标识                           | 类型及长度                                                   | 描述                                                         |
| ------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| m_head                         | EMQSzeHead                                                   | 包头                                                         |
| m_total_trade_num              | long long                                                    | 成交笔数                                                     |
| m_total_quantity               | unsigned long long                                           | 总成交量，实际值需要除以 100                                 |
| m_total_value                  | long long                                                    | 总成交额，实际值需要除以 1000000                             |
| m_pre_close_price              | unsigned int                                                 | 昨收价，实际值需要除以 10000                                 |
| m_last_price                   | unsigned int                                                 | 最新价，实际值需要除以 10000                                 |
| m_open_price                   | unsigned int                                                 | 开盘价，实际值需要除以 10000                                 |
| m_day_high_price               | unsigned int                                                 | 最高价，实际值需要除以 10000                                 |
| m_day_low_price                | unsigned int                                                 | 最低价，实际值需要除以 10000                                 |
| m_today_close_price            | unsigned int                                                 | 收盘价，实际值需要除以 10000                                 |
| m_total_bid_weighted_avg_price | unsigned int                                                 | 买方挂单均价，实际值需要除以 10000                           |
| m_total_bid_quantity           | unsigned long long                                           | 买总量，实际值需要除以 100                                   |
| m_total_ask_weighted_avg_price | unsigned int                                                 | 卖方挂单均价，实际值需要除以 10000                           |
| m_total_ask_quantity           | unsigned long long                                           | 卖总量，实际值需要除以 100                                   |
| m_upper_limit_price            | unsigned int                                                 | 涨停价，实际值需要除以 10000                                 |
| m_low_limit_price              | unsigned int                                                 | 跌停价，实际值需要除以 10000                                 |
| m_market_open_total_bid        | unsigned long long                                           | Bid 边市价单挂单总量，实际值需要除以 100                     |
| m_market_open_total_ask        | unsigned long long                                           | Ask 边市价单挂单总量，实际值需要除以 100                     |
| m_bid_depth                    | unsigned int                                                 | Bid 边总档位数                                               |
| m_ask_depth                    | unsigned int                                                 | Ask 边总档位数                                               |
| m_bid_unit[SZE_LEV2_DEPTH]     | [EMQSzePriceQuantityUnit](#EMQSzePriceQuantityUnit)[[SZE_LEV2_DEPTH](#SZE_LEV2_DEPTH)] | 买方十档                                                     |
| m_ask_unit[SZE_LEV2_DEPTH]     | [EMQSzePriceQuantityUnit](#EMQSzePriceQuantityUnit)[[SZE_LEV2_DEPTH](#SZE_LEV2_DEPTH)] | 卖方十档                                                     |
| m_ch_gap_flag                  | char                                                         | 0x01=此消息前逐笔消息有断号发生；0x00=逐笔消息序号连续，正常 |
| m_reserved[4]                  | char[4]                                                      | 保留字段                                                     |

### 5.2.11. <a id="emtquoteszebondsnap">EMQSzeBondSnap</a>

```cpp
// 深交债券快照消息
struct EMQSzeBondSnap
{
    EMQSzeBondSnap()
    {
        memset( this , 0 , sizeof( EMQSzeBondSnap ) );
    }
    EMQSzeHead                        m_head;                            // 包头
    unsigned char                     m_trading_status;                  // 交易状态码
    /******************************************************************************************
     * | Bit 位    | 返回值 | 交易所值 | 描述                                 |
     * | --------- | ------ | -------- | ------------------------------------ |
     * | Bid7-Bid4 | 0      | 'S'      | 表示启动(开市前)                     |
     * |           | 1      | 'O'      | 表示开盘集合竞价                     |
     * |           | 2      | 'T'      | 表示连续交易                         |
     * |           | 3      | 'B'      | 表示休市                             |
     * |           | 4      | 'C'      | 表示收盘集合竞价                     |
     * |           | 5      | 'E'      | 表示已闭市                           |
     * |           | 6      | 'H'      | 表示临时停牌                         |
     * |           | 7      | 'A'      | 表示盘后交易                         |
     * |           | 8      | 'V'      | 表示波动性中断                       |
     * |           | 15     | other    | 表示无意义                           |
     * | Bit3      | 0      | '0'      | 表示此正常状态                       |
     * |           | 1      | '1'      | 表示全天停牌                         |
     * |           | 0      | other    | 表示无意义，当成此产品可正常交易处理 |
     * | Bit2      | 0      |          | 预留位，值为 0                       |
     * | Bit1      | 0      |          | 预留位，值为 0                       |
     * | Bit0      | 0      |          | 预留位，值为 0                       |
    *******************************************************************************************/
    unsigned long long                m_total_trade_num;                 // 成交笔数
    unsigned long long                m_total_quantity;                  // 总成交量，          实际值需要除以 100
    unsigned long long                m_total_value;                     // 总成交额，          实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                 // 昨收价，            实际值需要除以 10000
    unsigned int                      m_last_price;                      // 最新价，            实际值需要除以 10000
    unsigned int                      m_open_price;                      // 开盘价，            实际值需要除以 10000
    unsigned int                      m_day_high_price;                  // 最高价，            实际值需要除以 10000
    unsigned int                      m_day_low_price;                   // 最低价，            实际值需要除以 10000
    unsigned int                      m_today_close_price;               // 收盘价，            实际值需要除以 10000
    unsigned int                      m_total_trade_weighted_avg_price;  // 加权平均价，        实际值需要除以 10000
    int                               m_fluctuation_1;                   // 升跌 1,             实际值需要除以 10000
    int                               m_fluctuation_2;                   // 升跌 2,             实际值需要除以 10000
    unsigned int                      m_total_bid_weighted_avg_price;    // 买方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;              // 买总量，            实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;    // 卖方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;              // 卖总量，            实际值需要除以 100

    int                               m_weighted_avg_price_BP;           // 加权平均涨跌 BP     实际值需要除以 10000
    unsigned int                      m_weighted_avg_pre_close_price;    // 昨收盘加权平均价    实际值需要除以 10000
    unsigned int                      m_auction_trade_last_price;        // 匹配成交最新价      实际值需要除以 10000
    unsigned char                     m_reserved[4];                     // 保留字段
    unsigned long long                m_auction_volume_trade;            // 匹配成交成交量      实际值需要除以 100
    unsigned long long                m_auction_value_trade;             // 匹配成交成交金额    实际值需要除以 1000000

    EMQSzePriceQuantityUnit      m_bid_unit[SZE_LEV2_DEPTH];        // 买方十档
    EMQSzePriceQuantityUnit      m_ask_unit[SZE_LEV2_DEPTH];        // 卖方十档
};
```

| 标识                             | 类型及长度                                                 | 描述                                     |
| -------------------------------- | ---------------------------------------------------------- | ---------------------------------------- |
| m_head                           | [EMQSzeHead](#EMQSzeHead)                                  | 包头                                     |
| m_trading_status                 | unsigned char                                              | 交易状态码                               |
| m_total_trade_num                | unsigned long long                                         | 成交笔数                                 |
| m_total_quantity                 | unsigned long long                                         | 总成交量，实际值需要除以 100             |
| m_total_value                    | unsigned long long                                         | 总成交额，实际值需要除以 1000000         |
| m_pre_close_price                | unsigned int                                               | 昨收价，实际值需要除以 10000             |
| m_last_price                     | unsigned int                                               | 最新价，实际值需要除以 10000             |
| m_open_price                     | unsigned int                                               | 开盘价，实际值需要除以 10000             |
| m_day_high_price                 | unsigned int                                               | 最高价，实际值需要除以 10000             |
| m_day_low_price                  | unsigned int                                               | 最低价，实际值需要除以 10000             |
| m_today_close_price              | unsigned int                                               | 收盘价，实际值需要除以 10000             |
| m_total_trade_weighted_avg_price | unsigned int                                               | 加权平均价，实际值需要除以 10000         |
| m_fluctuation_1                  | int                                                        | 升跌 1，实际值需要除以 10000             |
| m_fluctuation_2                  | int                                                        | 升跌 2，实际值需要除以 10000             |
| m_total_bid_weighted_avg_price   | unsigned int                                               | 买方挂单均价，实际值需要除以 10000       |
| m_total_bid_quantity             | unsigned long long                                         | 买总量，实际值需要除以 100               |
| m_total_ask_weighted_avg_price   | unsigned int                                               | 卖方挂单均价，实际值需要除以 10000       |
| m_total_ask_quantity             | unsigned long long                                         | 卖总量，实际值需要除以 100               |
| m_weighted_avg_price_BP          | int                                                        | 加权平均涨跌 BP，实际值需要除以 10000    |
| m_weighted_avg_pre_close_price   | unsigned int                                               | 昨收盘加权平均价，实际值需要除以 10000   |
| m_auction_trade_last_price       | unsigned int                                               | 匹配成交最新价，实际值需要除以 10000     |
| m_reserved[4]                    | unsigned char[4]                                           | 保留字段                                 |
| m_auction_volume_trade           | unsigned long long                                         | 匹配成交成交量，实际值需要除以 100       |
| m_auction_value_trade            | unsigned long long                                         | 匹配成交成交金额，实际值需要除以 1000000 |
| m_bid_unit[SZE_LEV2_DEPTH]       | EMQSzePriceQuantityUnit[[SZE_LEV2_DEPTH](#SZE_LEV2_DEPTH)] | 买方十档                                 |
| m_ask_unit[SZE_LEV2_DEPTH]       | EMQSzePriceQuantityUnit[[SZE_LEV2_DEPTH](#SZE_LEV2_DEPTH)] | 卖方十档                                 |

### 5.2.12. <a id="EMQSzeBondOrder">EMQSzeBondOrder</a>

```cpp

// 深交所债券逐笔委托
struct EMQSzeBondOrder
{
    EMQSzeBondOrder()
    {
        memset( this , 0 , sizeof( EMQSzeBondOrder ) );
    }

    EMQSzeHead                   m_head;                            // 包头
    /// 对逐笔委托价格转换说明如下：
    //  ****************************************************************************************************************
    //  |订单类型 \ 买卖方向 |                    买（1）            |                           卖（2）                    |
    //  |     市价（1）      |                    直接透传           |                           直接透传                   |
    //  |    本方最优(U）    |                    直接透传           |                           直接透传                   |
    //  |     限价（2）      |   如果小于 3 的取为 3，否则取原值     |   如果大于 999999997，取为 999999997，否则取原值     |
    //  *****************************************************************************************************************
    unsigned int                      m_price;                           // 价格，  实际值需要除以 10000
    unsigned long long                m_quantity;                        // 数量，  实际值需要除以 100
    char                              m_side;                            // 方向，‘1’=买 ‘2’=卖
    char                              m_order_type;                      // 订单类型
    //******************************************************************************************
    // '1'=市价  '2'=限价  'U'=本方最优
    //******************************************************************************************
    char                              m_reserved[15];                     // 保留字段
};
```

| 标识         | 类型及长度                | 描述                       |
| ------------ | ------------------------- | -------------------------- |
| m_head       | [EMQSzeHead](#EMQSzeHead) | 包头                       |
| m_price      | unsigned int              | 价格，实际值需要除以 10000 |
| m_quantity   | unsigned long long        | 数量，实际值需要除以 100   |
| m_side       | char                      | 方向，‘1’=买 ‘2’=卖        |
| m_order_type | char                      | 订单类型                   |
| m_reserved   | char[15]                  | 保留字段                   |

### 5.2.13. <a id="EMQSzeBondExe">EMQSzeBondExe</a>

``` cpp
// 深交所债券逐笔成交
struct EMQSzeBondExe
{
    EMQSzeBondExe()
    {
        memset( this , 0 , sizeof( EMQSzeBondExe ) );
    }
    EMQSzeHead                   m_head;                           // 包头
    long long                         m_buy_num;                        // 买方包序号
    long long                         m_sell_num;                       // 卖方包序号
    unsigned int                      m_price;                          // 价格，   实际值需要除以 10000
    long long                         m_quantity;                       // 数量，   实际值需要除以 100
    char                              m_type;                           // 成交类型
    //******************************************************************************************
    // '4'=撤销  'F'=成交
    //******************************************************************************************
};
```

| 标识       | 类型及长度                | 描述                       |
| ---------- | ------------------------- | -------------------------- |
| m_head     | [EMQSzeHead](#EMQSzeHead) | 包头                       |
| m_buy_num  | long long                 | 买方包序号                 |
| m_sell_num | long long                 | 卖方包序号                 |
| m_price    | unsigned int              | 价格，实际值需要除以 10000 |
| m_quantity | long long                 | 数量，实际值需要除以 100   |
| m_type     | char                      | 成交类型                   |

### 5.2.14. <a id="EMQSzeBondTick">EMQSzeBondTick</a>

``` cpp
struct EMQSzeBondTick
{
    EMQSzeTickType m_tick_type;
    union {
        EMQSzeBondOrder *m_bond_order;
        EMQSzeBondExe *m_bond_exe;
    };
};
```

| 标识         | 类型及长度                            | 描述                   |
| ------------ | ------------------------------------- | ---------------------- |
| m_tick_type  | [EMQSzeTickType](#EMQSzeTickType)     | 逐笔消息类型           |
| m_bond_order | [EMQSzeBondOrder](#EMQSzeBondOrder) * | 指向债券逐笔委托的指针 |
| m_bond_exe   | [EMQSzeBondExe](#EMQSzeBondExe) *     | 指向债券逐笔成交的指针 |



### 5.2.15. <a id="EMQSseL2PriceQuantityUnit">EMQSseL2PriceQuantityUnit</a>

```cpp
// 上交所lev2快照档位单元
struct EMQSseL2PriceQuantityUnit
{
    char                              m_reserved[4];                    // 保留字段
    unsigned int                      m_price;                          // 价格，   实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量，   实际值需要除以 1000
};
```

| 标识       | 类型及长度         | 描述                      |
| ---------- | ------------------ | ------------------------- |
| m_reserved | char[4]            | 保留字段                  |
| m_price    | unsigned int       | 价格，实际值需要除以 1000 |
| m_quantity | unsigned long long | 数量，实际值需要除以 1000 |

### 5.2.16. <a id="EMQSseHeartbeat">EMQSseHeartbeat</a>

``` cpp
// 上交所心跳消息
struct EMQSseHeartbeat
{
    unsigned int                      m_sequence;                       // 行情序号
    unsigned int                      m_reserved_1;                     // 保留字段
    unsigned char                     m_message_type;                   // 行情类型
    unsigned char                     m_reserved_2[7];                  // 保留字段
};
```

| 标识           | 类型及长度       | 描述     |
| -------------- | ---------------- | -------- |
| m_sequence     | unsigned int     | 行情序号 |
| m_reserved_1   | unsigned int     | 保留字段 |
| m_message_type | unsigned char    | 行情类型 |
| m_reserved_2   | unsigned char[7] | 保留字段 |

### 5.2.17. <a id="EMQSseHead">EMQSseHead</a>

``` cpp
// 上交所消息协议头
struct EMQSseHead
{
    unsigned int                      m_sequence;                       // 行情序号
    unsigned int                      m_reserved_1;                     // 保留字段
    unsigned char                     m_message_type;                   // 行情类型
    unsigned short                    m_message_len;                    // 包括此消息头的长度
    unsigned char                     m_exchange_id;                    // 交易所编号
    unsigned short                    m_quote_date_year;                // 行情更新年份
    unsigned char                     m_quote_date_month;               // 行情更新月份
    unsigned char                     m_quote_date_day;                 // 行情更新日期
    unsigned int                      m_send_time;                      // 交易所发送时间
    unsigned char                     m_category_id;                    // 数据产品类别
    unsigned int                      m_msg_seq_id;                     // 行情包序号
    unsigned char                     m_seq_lost_flag;                  // 1=有丢包，0=没有丢包
};
```

| 标识               | 类型及长度     | 描述                 |
| ------------------ | -------------- | -------------------- |
| m_sequence         | unsigned int   | 行情序号             |
| m_reserved_1       | unsigned int   | 保留字段             |
| m_message_type     | unsigned char  | 行情类型             |
| m_message_len      | unsigned short | 包括此消息头的长度   |
| m_exchange_id      | unsigned char  | 交易所编号           |
| m_quote_date_year  | unsigned short | 行情更新年份         |
| m_quote_date_month | unsigned char  | 行情更新月份         |
| m_quote_date_day   | unsigned char  | 行情更新日期         |
| m_send_time        | unsigned int   | 交易所发送时间       |
| m_category_id      | unsigned char  | 数据产品类别         |
| m_msg_seq_id       | unsigned int   | 行情包序号           |
| m_seq_lost_flag    | unsigned char  | 1=有丢包，0=没有丢包 |

### 5.2.18. <a id="EMQSseSnap">EMQSseSnap</a>

``` cpp
// 上交所lev2快照消息
struct EMQSseSnap
{
    EMQSseSnap()
    {
        memset(this, 0, sizeof(EMQSseSnap));                          // 初始化结构体
    }

    EMQSseHead                        m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSS（秒）HH=00-23, MM=00-59, SS=00-59(秒)
    char                              m_symbol[SSE_SYMBOL_LEN];         // 证券代码
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_image_status;                   // 快照类型，1=全量，2=更新
    unsigned char                     m_reserved_1;                     // 保留字段
    unsigned int                      m_pre_close_price;                // 昨收价，    实际值需要除以 1000
    unsigned int                      m_open_price;                     // 开盘价，    实际值需要除以 1000
    unsigned int                      m_day_high_price;                 // 最高价，    实际值需要除以 1000
    unsigned int                      m_day_low_price;                  // 最低价，    实际值需要除以 1000
    unsigned int                      m_last_price;                     // 最新价，    实际值需要除以 1000
    unsigned int                      m_today_close_price;              // 收盘价，    实际值需要除以 1000
    unsigned char                     m_instrument_status;              // 当前品种交易状态
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌 5:收盘集合竞价
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_trading_status;                 // 当前产品状态
    //******************************************************************************************
    // Bit7-Bit4(m_trading_status&0xF0):
    // 0x00:表示启动（开市前)
    // 0x10:表示开盘集合竞价
    // 0x20:表示连续交易
    // 0x30:表示闭市
    // 0x40:表示合约停牌
    // 0x50:表示可恢复交易的熔断时段（盘中集合竞价）
    // 0x60:表示不可恢复交易的熔断时段（暂停交易至闭市）
    // 0x70:表示收盘集合竞价时段
    // 0x80:表示波动性中断（适用于股票期权)
    // 0x15:表示无意义
    // Bit3(m_trading_status&0x08):
    // 0x08:‟表示此合约可正常交易 0x00:表示此合约不可正常交易
    // Bit2(m_trading_status&0x04):
    // 0x04:表示已上市 0x00:表示未上市
    // Bit1(m_trading_status&0x02):
    // 0x02:表示此合约在当前时段接受订单申报 0x00:表示此合约在当前时段不接受订单申报
    // Bit0:预留
    //******************************************************************************************
    unsigned short                    m_reserved_2;                     // 保留字段
    unsigned int                      m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 成交总量，               实际值需要除以 1000
    unsigned long long                m_total_value;                    // 成交总额，               实际值需要除以 100000
    unsigned long long                m_total_bid_quantity;             // 委托买入总量，           实际值需要除以 1000
    unsigned int                      m_total_bid_weighted_avg_price;   // 买方加权平均委买价格，   实际值需要除以 1000
    unsigned long long                m_total_ask_quantity;             // 委托卖出总量，           实际值需要除以 1000
    unsigned int                      m_total_ask_weighted_avg_price;   // 卖方加权平均委买价格，   实际值需要除以 1000
    unsigned int                      m_yield_to_maturity;              // 债券到期收益率
    unsigned char                     m_bid_depth;                      // 买盘价位数量
    unsigned char                     m_ask_depth;                      // 卖盘价位数量
    EMQSseL2PriceQuantityUnit    m_bid_unit[SSE_LEV2_DEPTH];       // 买边十档
    EMQSseL2PriceQuantityUnit    m_ask_unit[SSE_LEV2_DEPTH];       // 卖边十档
};
```

| 标识                           | 类型及长度                                                   | 描述                                      |
| ------------------------------ | ------------------------------------------------------------ | ----------------------------------------- |
| m_head                         | [EMQSseHead](#EMQSseHead)                                    | 包头                                      |
| m_quote_update_time            | unsigned int                                                 | 行情更新时间,格式如：HHMMSS（秒）         |
| m_symbol                       | char[[SSE_SYMBOL_LEN](#SSE_SYMBOL_LEN)]                      | 证券代码                                  |
| m_security_type                | unsigned char                                                | 证券类型 (0：指数，1：股票，... 10：其他) |
| m_image_status                 | unsigned char                                                | 快照类型，1=全量，2=更新                  |
| m_reserved_1                   | unsigned char                                                | 保留字段                                  |
| m_pre_close_price              | unsigned int                                                 | 昨收价，实际值需要除以 1000               |
| m_open_price                   | unsigned int                                                 | 开盘价，实际值需要除以 1000               |
| m_day_high_price               | unsigned int                                                 | 最高价，实际值需要除以 1000               |
| m_day_low_price                | unsigned int                                                 | 最低价，实际值需要除以 1000               |
| m_last_price                   | unsigned int                                                 | 最新价，实际值需要除以 1000               |
| m_today_close_price            | unsigned int                                                 | 收盘价，实际值需要除以 1000               |
| m_instrument_status            | unsigned char                                                | 当前品种交易状态                          |
| m_trading_status               | unsigned char                                                | 当前产品状态                              |
| m_reserved_2                   | unsigned short                                               | 保留字段                                  |
| m_total_trade_num              | unsigned int                                                 | 成交笔数                                  |
| m_total_quantity               | unsigned long long                                           | 成交总量，实际值需要除以 1000             |
| m_total_value                  | unsigned long long                                           | 成交总额，实际值需要除以 100000           |
| m_total_bid_quantity           | unsigned long long                                           | 委托买入总量，实际值需要除以 1000         |
| m_total_bid_weighted_avg_price | unsigned int                                                 | 买方加权平均委买价格，实际值需要除以 1000 |
| m_total_ask_quantity           | unsigned long long                                           | 委托卖出总量，实际值需要除以 1000         |
| m_total_ask_weighted_avg_price | unsigned int                                                 | 卖方加权平均委买价格，实际值需要除以 1000 |
| m_yield_to_maturity            | unsigned int                                                 | 债券到期收益率                            |
| m_bid_depth                    | unsigned char                                                | 买盘价位数量                              |
| m_ask_depth                    | unsigned char                                                | 卖盘价位数量                              |
| m_bid_unit                     | [EMQSseL2PriceQuantityUnit](#EMQSseL2PriceQuantityUnit)[[SSE_LEV2_DEPTH](#SSE_LEV2_DEPTH)] | 买边十档                                  |
| m_ask_unit                     | [EMQSseL2PriceQuantityUnit](#EMQSseL2PriceQuantityUnit)[[SSE_LEV2_DEPTH](#SSE_LEV2_DEPTH)] | 卖边十档                                  |

### 5.2.19. <a id="EMQSseIdx">EMQSseIdx</a>

``` cpp
// 上交所lev2指数消息
struct EMQSseIdx
{
    EMQSseIdx()
    {
        memset(this, 0, sizeof(EMQSseIdx));                           // 初始化结构体
    }

    EMQSseHead                        m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSS（秒）HH=00-23, MM=00-59, SS=00-59(秒)
    char                              m_symbol[SSE_SYMBOL_LEN];         // 证券代码
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 0=指数， 10=其他
    unsigned int                      m_pre_close_price;                // 昨收价，     实际值需要除以 100000
    unsigned int                      m_open_price;                     // 开盘价，     实际值需要除以 100000
    unsigned long long                m_total_value;                    // 成交额，     实际值需要除以 10
    unsigned int                      m_day_high_price;                 // 最高价，     实际值需要除以 100000
    unsigned int                      m_day_low_price;                  // 最低价，     实际值需要除以 100000
    unsigned int                      m_last_price;                     // 最新价，     实际值需要除以 100000
    unsigned long long                m_total_quantity;                 // 成交量，     实际值需要除以 100000
    unsigned int                      m_today_close_price;              // 今收盘价，   实际值需要除以 100000
};
```

| 标识                | 类型及长度                              | 描述                              |
| ------------------- | --------------------------------------- | --------------------------------- |
| m_head              | [EMQSseHead](#EMQSseHead)               | 包头                              |
| m_quote_update_time | unsigned int                            | 行情更新时间,格式如：HHMMSS（秒） |
| m_symbol            | char[[SSE_SYMBOL_LEN](#SSE_SYMBOL_LEN)] | 证券代码                          |
| m_security_type     | unsigned char                           | 证券类型 (0：指数，10：其他)      |
| m_pre_close_price   | unsigned int                            | 昨收价，实际值需要除以 100000     |
| m_open_price        | unsigned int                            | 开盘价，实际值需要除以 100000     |
| m_total_value       | unsigned long long                      | 成交额，实际值需要除以 10         |
| m_day_high_price    | unsigned int                            | 最高价，实际值需要除以 100000     |
| m_day_low_price     | unsigned int                            | 最低价，实际值需要除以 100000     |
| m_last_price        | unsigned int                            | 最新价，实际值需要除以 100000     |
| m_total_quantity    | unsigned long long                      | 成交量，实际值需要除以 100000     |
| m_today_close_price | unsigned int                            | 今收盘价，实际值需要除以 100000   |

### 5.2.20. <a id="EMQSseL2TreePriceQuantityUnit">EMQSseL2TreePriceQuantityUnit</a>

``` cpp
// 上交所lev2建树档位单元
struct EMQSseL2TreePriceQuantityUnit
{
    unsigned int                      m_price;                          // 价格，   实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量，   实际值需要除以 1000
};
```

| 标识       | 类型               | 描述                      |
| ---------- | ------------------ | ------------------------- |
| m_price    | unsigned int       | 价格，实际值需要除以 1000 |
| m_quantity | unsigned long long | 数量，实际值需要除以 1000 |

### 5.2.21. <a id="EMQSseTree">EMQSseTree</a>

``` cpp

// 上交所lev2 建树消息
struct EMQSseTree
{
    EMQSseTree()
    {
        memset(this, 0, sizeof(EMQSseTree));
    }

    EMQSseHead                   m_head;
    unsigned int                      m_channel_num;                    // 通道号
    unsigned int                      m_quote_update_time;              // 行情时间
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 股票/合约类型
    unsigned long long                m_biz_index;                      // 业务序列号
    unsigned int                      m_pre_close_price;                // 昨收价，                 实际值需要除以 1000
    unsigned int                      m_open_price;                     // 开盘价，                 实际值需要除以 1000
    unsigned int                      m_day_high_price;                 // 最高价，                 实际值需要除以 1000
    unsigned int                      m_day_low_price;                  // 最低价，                 实际值需要除以 1000
    unsigned int                      m_last_price;                     // 最新价，                 实际值需要除以 1000
    unsigned int                      m_today_close_price;              // 今收盘价，               实际值需要除以 1000
    unsigned char                     m_instrument_status;              // 当前品种交易状态
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌 5:收盘集合竞价
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_trading_status;                 // 当前产品状态
    //******************************************************************************************
    // Bit7-Bit4(m_trading_status&0xF0):
    // 0x00:表示启动（开市前)
    // 0x10:表示开盘集合竞价
    // 0x20:表示连续交易
    // 0x30:表示闭市
    // 0x40:表示合约停牌
    // 0x50:表示可恢复交易的熔断时段（盘中集合竞价）
    // 0x60:表示不可恢复交易的熔断时段（暂停交易至闭市）
    // 0x70:表示收盘集合竞价时段
    // 0x80:表示波动性中断（适用于股票期权)
    // 0x15:表示无意义
    // Bit3(m_trading_status&0x08):
    // 0x08:‟表示此合约可正常交易 0x00:表示此合约不可正常交易
    // Bit2(m_trading_status&0x04):
    // 0x04:表示已上市 0x00:表示未上市
    // Bit1(m_trading_status&0x02):
    // 0x02:表示此合约在当前时段接受订单申报 0x00:表示此合约在当前时段不接受订单申报
    // Bit0:预留
    //******************************************************************************************
    unsigned char                     m_reserved_0[6];                  // 6个字节的保留字段
    unsigned int                      m_total_trade_num;                // 总成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，               实际值需要除以 1000
    unsigned long long                m_total_value;                    // 总成交金额，             实际值需要除以 100000
    unsigned long long                m_total_bid_quantity;             // 委托买入数量，           实际值需要除以 1000
    unsigned int                      m_total_bid_weighted_avg_price;   // 加权平均委托买入价格，   实际值需要除以 1000
    unsigned long long                m_total_ask_quantity;             // 委托卖出总量，           实际值需要除以 1000
    int                               m_total_ask_weighted_avg_price;   // 加权平均委托卖价格，     实际值需要除以 1000
    int                               m_yield_to_maturity;              // 债券到期收益率
    unsigned char                     m_bid_depth;                      // 申买档位数
    unsigned char                     m_ask_depth;                      // 申卖档位数
    unsigned char                     m_reserved_1[2];                  // 2字节的保留字段
    EMQSseL2TreePriceQuantityUnit m_bid_unit[SSE_LEV2_DEPTH];       // 申买档位信息
    EMQSseL2TreePriceQuantityUnit m_ask_unit[SSE_LEV2_DEPTH];       // 申卖档位信息
};
```

| 标识                           | 类型                                                         | 描述                                      |
| ------------------------------ | ------------------------------------------------------------ | ----------------------------------------- |
| m_head                         | [EMQSseHead](#EMQSseHead)                                    | 包头                                      |
| m_channel_num                  | unsigned int                                                 | 通道号                                    |
| m_quote_update_time            | unsigned int                                                 | 行情时间                                  |
| m_symbol                       | char[[SSE_SYMBOL_LEN](#SSE_SYMBOL_LEN)]                      | 标的名称                                  |
| m_security_type                | unsigned char                                                | 股票/合约类型                             |
| m_biz_index                    | unsigned long long                                           | 业务序列号                                |
| m_pre_close_price              | unsigned int                                                 | 昨收价，实际值需要除以 1000               |
| m_open_price                   | unsigned int                                                 | 开盘价，实际值需要除以 1000               |
| m_day_high_price               | unsigned int                                                 | 最高价，实际值需要除以 1000               |
| m_day_low_price                | unsigned int                                                 | 最低价，实际值需要除以 1000               |
| m_last_price                   | unsigned int                                                 | 最新价，实际值需要除以 1000               |
| m_today_close_price            | unsigned int                                                 | 今收盘价，实际值需要除以 1000             |
| m_instrument_status            | unsigned char                                                | 当前品种交易状态                          |
| m_trading_status               | unsigned char                                                | 当前产品状态                              |
| m_reserved_0                   | unsigned char[6]                                             | 6个字节的保留字段                         |
| m_total_trade_num              | unsigned int                                                 | 总成交笔数                                |
| m_total_quantity               | unsigned long long                                           | 总成交量，实际值需要除以 1000             |
| m_total_value                  | unsigned long long                                           | 总成交金额，实际值需要除以 100000         |
| m_total_bid_quantity           | unsigned long long                                           | 委托买入数量，实际值需要除以 1000         |
| m_total_bid_weighted_avg_price | unsigned int                                                 | 加权平均委托买入价格，实际值需要除以 1000 |
| m_total_ask_quantity           | unsigned long long                                           | 委托卖出总量，实际值需要除以 1000         |
| m_total_ask_weighted_avg_price | int                                                          | 加权平均委托卖价格，实际值需要除以 1000   |
| m_yield_to_maturity            | int                                                          | 债券到期收益率                            |
| m_bid_depth                    | unsigned char                                                | 申买档位数                                |
| m_ask_depth                    | unsigned char                                                | 申卖档位数                                |
| m_reserved_1                   | unsigned char[2]                                             | 2字节的保留字段                           |
| m_bid_unit                     | EMQSseL2TreePriceQuantityUnit[[SSE_LEV2_DEPTH](#SSE_LEV2_DEPTH)] | 申买档位信息                              |
| m_ask_unit                     | EMQSseL2TreePriceQuantityUnit[[SSE_LEV2_DEPTH](#SSE_LEV2_DEPTH)] | 申卖档位信息                              |

### 5.2.22. <a id="EMQSseL2BondPriceQuantityUnit">EMQSseL2BondPriceQuantityUnit</a>

``` cpp
// 上交所lev2债券快照档位单元
struct EMQSseL2BondPriceQuantityUnit
{
    unsigned int                      m_price;                          // 价格，   实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量，   实际值需要除以 1000
};
```

| 标识       | 类型               | 描述                      |
| ---------- | ------------------ | ------------------------- |
| m_price    | unsigned int       | 价格，实际值需要除以 1000 |
| m_quantity | unsigned long long | 数量，实际值需要除以 1000 |

### 5.2.23. <a id="EMQSseBondSnap">EMQSseBondSnap</a>

``` cpp
// 上交所lev2债券快照
struct EMQSseBondSnap
{
    EMQSseBondSnap()
    {
        memset(this, 0, sizeof(EMQSseBondSnap));                  // 初始化结构体
    }

    EMQSseHead                   m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSSsss（千分之一秒）HH=00-23, MM=00-59, SS=00-59(秒) sss=000-999(千分之一秒) 如 143025002 表示 14:30:25.002
    unsigned char                     m_reserved_0[4];                  // 保留字段
    char                              m_symbol[SSE_SYMBOL_LEN];         // 合约
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_sub_security_type;              // 证券子类型
    unsigned char                     m_reserved_1[3];                  // 保留字段
    unsigned int                      m_pre_close_price;                // 昨收盘价，       实际值需要除以 1000
    unsigned int                      m_open_price;                     // 开盘价，         实际值需要除以 1000
    unsigned int                      m_day_high_price;                 // 最高价，         实际值需要除以 1000
    unsigned int                      m_day_low_price;                  // 最低价 ，        实际值需要除以 1000
    unsigned int                      m_last_price;                     // 最新价，         实际值需要除以 1000
    unsigned int                      m_today_close_price;              // 收盘价，         实际值需要除以 1000
    unsigned char                     m_instrument_status;              // 当前品种交易状态
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_reserved_2[3];                  // 保留字段
    unsigned int                      m_total_trade_num;                // 总成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，               实际值需要除以 1000
    unsigned long long                m_total_value;                    // 总成交金额，             实际值需要除以 100000
    unsigned long long                m_total_bid_quantity;             // 委托买入数量，           实际值需要除以 1000
    unsigned int                      m_total_bid_weighted_avg_price;   // 加权平均委托买入价格，   实际值需要除以 1000
    unsigned long long                m_total_ask_quantity;             // 委托卖出总量，           实际值需要除以 1000
    unsigned int                      m_total_ask_weighted_avg_price;   // 加权平均委托卖价格，     实际值需要除以 1000

    unsigned int                      m_withdraw_bid_num;               // 买入撤单笔数
    unsigned long long                m_withdraw_bid_amount;            // 买入撤单数量             实际值需要除以 1000
    unsigned long long                m_withdraw_bid_price;             // 买入撤单金额             实际值需要除以 100000
    unsigned int                      m_withdraw_ask_num;               // 卖出撤单笔数
    unsigned long long                m_withdraw_ask_amount;            // 卖出撤单数量             实际值需要除以 1000
    unsigned long long                m_withdraw_ask_price;             // 卖出撤单金额             实际值需要除以 100000
    unsigned int                      m_total_bid_num;                  // 买入总笔数
    unsigned int                      m_total_ask_num;                  // 卖出总笔数
    unsigned int                      m_bid_trade_max_duration;         // 买入委托成交最大等待时间
    unsigned int                      m_ask_trade_max_duration;         // 卖出委托成交最大等待时间

    unsigned char                     m_bid_depth;                      // 申买档位数
    unsigned char                     m_ask_depth;                      // 申卖档位数
    unsigned char                     m_reserved_3[6];                  // 保留字段
    EMQSseL2BondPriceQuantityUnit   m_bid_unit[SSE_LEV2_DEPTH];     // 申买信息
    EMQSseL2BondPriceQuantityUnit   m_ask_unit[SSE_LEV2_DEPTH];     // 申卖信息
};
```

| 标识                           | 类型                                                         | 描述                                                         |
| ------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| m_head                         | [EMQSseHead](#EMQSseHead)                                    | 包头                                                         |
| m_quote_update_time            | unsigned int                                                 | 行情更新时间, 格式如：HHMMSSsss（千分之一秒）                |
| m_reserved_0                   | unsigned char[4]                                             | 保留字段                                                     |
| m_symbol                       | char[[SSE_SYMBOL_LEN](#SSE_SYMBOL_LEN)]                      | 合约                                                         |
| m_security_type                | unsigned char                                                | 证券类型                                                     |
| m_sub_security_type            | unsigned char                                                | 证券子类型                                                   |
| m_reserved_1                   | unsigned char[3]                                             | 保留字段                                                     |
| m_pre_close_price              | unsigned int                                                 | 昨收盘价，实际值需要除以 1000                                |
| m_open_price                   | unsigned int                                                 | 开盘价，实际值需要除以 1000                                  |
| m_day_high_price               | unsigned int                                                 | 最高价，实际值需要除以 1000                                  |
| m_day_low_price                | unsigned int                                                 | 最低价，实际值需要除以 1000                                  |
| m_last_price                   | unsigned int                                                 | 最新价，实际值需要除以 1000                                  |
| m_today_close_price            | unsigned int                                                 | 收盘价，实际值需要除以 1000                                  |
| m_instrument_status            | unsigned char                                                | 当前品种交易状态<br />0:其他 1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌  6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市 |
| m_reserved_2                   | unsigned char[3]                                             | 保留字段                                                     |
| m_total_trade_num              | unsigned int                                                 | 总成交笔数                                                   |
| m_total_quantity               | unsigned long long                                           | 总成交量，实际值需要除以 1000                                |
| m_total_value                  | unsigned long long                                           | 总成交金额，实际值需要除以 100000                            |
| m_total_bid_quantity           | unsigned long long                                           | 委托买入数量，实际值需要除以 1000                            |
| m_total_bid_weighted_avg_price | unsigned int                                                 | 加权平均委托买入价格，实际值需要除以 1000                    |
| m_total_ask_quantity           | unsigned long long                                           | 委托卖出总量，实际值需要除以 1000                            |
| m_total_ask_weighted_avg_price | unsigned int                                                 | 加权平均委托卖价格，实际值需要除以 1000                      |
| m_withdraw_bid_num             | unsigned int                                                 | 买入撤单笔数                                                 |
| m_withdraw_bid_amount          | unsigned long long                                           | 买入撤单数量，实际值需要除以 1000                            |
| m_withdraw_bid_price           | unsigned long long                                           | 买入撤单金额，实际值需要除以 100000                          |
| m_withdraw_ask_num             | unsigned int                                                 | 卖出撤单笔数                                                 |
| m_withdraw_ask_amount          | unsigned long long                                           | 卖出撤单数量，实际值需要除以 1000                            |
| m_withdraw_ask_price           | unsigned long long                                           | 卖出撤单金额，实际值需要除以 100000                          |
| m_total_bid_num                | unsigned int                                                 | 买入总笔数                                                   |
| m_total_ask_num                | unsigned int                                                 | 卖出总笔数                                                   |
| m_bid_trade_max_duration       | unsigned int                                                 | 买入委托成交最大等待时间                                     |
| m_ask_trade_max_duration       | unsigned int                                                 | 卖出委托成交最大等待时间                                     |
| m_bid_depth                    | unsigned char                                                | 申买档位数                                                   |
| m_ask_depth                    | unsigned char                                                | 申卖档位数                                                   |
| m_reserved_3                   | unsigned char[6]                                             | 保留字段                                                     |
| m_bid_unit                     | EMQSseL2BondPriceQuantityUnit[[SSE_LEV2_DEPTH](#SSE_LEV2_DEPTH)] | 申买信息                                                     |
| m_ask_unit                     | EMQSseL2BondPriceQuantityUnit[[SSE_LEV2_DEPTH](#SSE_LEV2_DEPTH)] | 申卖信息                                                     |

### 5.2.24. <a id="EMQSseBondTick">EMQSseBondTick</a>

``` cpp

// 债券逐笔
struct EMQSseBondTick
{
    EMQSseBondTick()
    {
        memset(this, 0, sizeof(EMQSseBondTick));                  // 初始化结构体
    }

    EMQSseHead                   m_head;                           // 包头
    unsigned int                      m_tick_index;                     // 序号从1开始,按通道连续
    unsigned int                      m_channel_num;                    // 通道号
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_sub_security_type;              // 证券子类型
    unsigned int                      m_tick_time;                      // 订单或成交时间,格式如：HHMMSSsss（千分之一秒）HH=00-23, MM=00-59, SS=00-59(秒) sss=000-999(千分之一秒) 如 143025002 表示 14:30:25.002
    unsigned char                     m_tick_type;                      // 类型， A新增订单， D删除订单， 删除订单， S产品状态订单， T成交
    unsigned long long                m_buy_num;                        // 买方订单号
    unsigned long long                m_sell_num;                       // 卖方订单号
    unsigned int                      m_price;                          // 价格，对产品状态订单无意义       实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量（手），对产品状态订单无意义 实际值需要除以 1000
    unsigned long long                m_trade_value;                    // 成交额，仅适用于成交消息         实际值需要除以 100000
    char                              m_side_flag;                      // 买卖标志，若为订单： B - 买单， S - 卖单；若为成交： B - 外盘， 主动买， S - 内盘， 主动卖 N - 未知 。
    unsigned char                     m_instrument_status;              // 当前品种交易状态，仅适用于产品状态订单
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_reserved[8];                    // 保留字段
};

```

| 标识                | 类型                                    | 描述                                                         |
| ------------------- | --------------------------------------- | ------------------------------------------------------------ |
| m_head              | [EMQSseHead](#EMQSseHead)               | 包头                                                         |
| m_tick_index        | unsigned int                            | 序号从1开始,按通道连续                                       |
| m_channel_num       | unsigned int                            | 通道号                                                       |
| m_symbol            | char[[SSE_SYMBOL_LEN](#SSE_SYMBOL_LEN)] | 标的名称                                                     |
| m_security_type     | unsigned char                           | 证券类型                                                     |
| m_sub_security_type | unsigned char                           | 证券子类型                                                   |
| m_tick_time         | unsigned int                            | 订单或成交时间，格式如：HHMMSSsss（千分之一秒）              |
| m_tick_type         | unsigned char                           | 类型：A新增订单，D删除订单，S产品状态订单，T成交             |
| m_buy_num           | unsigned long long                      | 买方订单号                                                   |
| m_sell_num          | unsigned long long                      | 卖方订单号                                                   |
| m_price             | unsigned int                            | 价格，对产品状态订单无意义，实际值需要除以 1000              |
| m_quantity          | unsigned long long                      | 数量（手），对产品状态订单无意义，实际值需要除以 1000        |
| m_trade_value       | unsigned long long                      | 成交额，仅适用于成交消息，实际值需要除以 100000              |
| m_side_flag         | char                                    | 买卖标志，B - 买单，S - 卖单；成交时，B - 外盘，S - 内盘，N - 未知 |
| m_instrument_status | unsigned char                           | 当前品种交易状态，仅适用于产品状态订单                       |
| m_reserved          | unsigned char[8]                        | 保留字段                                                     |

### 5.2.25. <a id="EMQSseTick">EMQSseTick</a>

``` cpp
// 逐笔合并
struct EMQSseTick
{
    EMQSseTick()
    {
        memset(this, 0, sizeof(EMQSseTick));                  // 初始化结构体
    }

    unsigned int                      m_sequence;                       // 行情序号
    unsigned int                      m_reserved_1;                     // 保留字段
    unsigned char                     m_message_type;                   // 行情类型
    unsigned long long                m_tick_index;                     // 序号从1开始,按通道连续
    unsigned short                    m_channel_num;                    // 通道号
    unsigned short                    m_reserved_2;                     // 保留字段
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    unsigned int                      m_tick_time;                      // 订单或成交时间,格式如：HHMMSSss（百分之一秒）HH=00-23, MM=00-59, SS=00-59(秒) ss=00-99(百分之一秒) 如 14302502 表示 14:30:25.02
    unsigned char                     m_tick_type;                      // 类型， A新增订单， D删除订单， S产品状态订单， T成交
    unsigned long long                m_buy_num;                        // 买方订单号
    unsigned long long                m_sell_num;                       // 卖方订单号
    unsigned int                      m_price;                          // 价格，对产品状态订单无意义       实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量（手），对产品状态订单无意义 实际值需要除以 1000
    unsigned long long                m_trade_value;
    // 对于新增委托，表示已成交的委托数量， 字段类型为 uInt64_1000, 实际值需要除以 1000；
    // 对于成交，表示成交金额 （ 元 ）， 字段类型为 uInt64_100000, 实际值需要除以 100000；
    // 其他无意义

    unsigned char                     m_side_flag;                      // TickType不同含义不同，参考附表证券交 易状态表
    /*******************************************************************************
    | TickType 类型 | m_tick_type 取值 |     SideFlag 含义      | m_side_flag 取值 |
    | ------------- | ---------------- | ---------------------- | ---------------- |
    |   新增订单    |       'A'        |          买单          |       0x00       |
    |               |                  |          卖单          |       0x01       |
    | ------------- | ---------------- | ---------------------- | ---------------- |
    |   删除订单    |       'D'        |          买单          |       0x00       |
    |               |                  |          卖单          |       0x01       |
    | ------------- | ---------------- | ---------------------- | ---------------- |
    | 产品状态订单  |       'S'        |          其他          |       0x00       |
    |               |                  |          启动          |       0x01       |
    |               |                  |      开市集合竞价      |       0x02       |
    |               |                  |      连续自动撮合      |       0x03       |
    |               |                  |          停牌          |       0x04       |
    |               |                  |      收盘集合竞价      |       0x05       |
    |               |                  | 闭市, 自动计算闭市价格 |       0x06       |
    |               |                  |        交易结束        |       0x07       |
    |               |                  |       产品未上市       |       0x08       |
    | ------------- | ---------------- | ---------------------- | ---------------- |
    |     成交      |       'T'        |      外盘 主动买       |       0x00       |
    |               |                  |      内盘 主动卖       |       0x01       |
    |               |                  |          未知          |       0x02       |
    *******************************************************************************/
};
```

| 标识           | 类型                                    | 描述                                                         |
| -------------- | --------------------------------------- | ------------------------------------------------------------ |
| m_sequence     | unsigned int                            | 行情序号                                                     |
| m_reserved_1   | unsigned int                            | 保留字段                                                     |
| m_message_type | unsigned char                           | 行情类型                                                     |
| m_tick_index   | unsigned long long                      | 序号从1开始,按通道连续                                       |
| m_channel_num  | unsigned short                          | 通道号                                                       |
| m_reserved_2   | unsigned short                          | 保留字段                                                     |
| m_symbol       | char[[SSE_SYMBOL_LEN](#SSE_SYMBOL_LEN)] | 标的名称                                                     |
| m_tick_time    | unsigned int                            | 订单或成交时间，格式如：HHMMSSss（百分之一秒）               |
| m_tick_type    | unsigned char                           | 类型：A新增订单，D删除订单，S产品状态订单，T成交             |
| m_buy_num      | unsigned long long                      | 买方订单号                                                   |
| m_sell_num     | unsigned long long                      | 卖方订单号                                                   |
| m_price        | unsigned int                            | 价格，对产品状态订单无意义，实际值需要除以 1000              |
| m_quantity     | unsigned long long                      | 数量（手），对产品状态订单无意义，实际值需要除以 1000        |
| m_trade_value  | unsigned long long                      | 对于新增委托，表示已成交的委托数量（实际值需要除以 1000）；对于成交，表示成交金额（实际值需要除以 100000） |
| m_side_flag    | unsigned char                           | m_tick_type取值不同该字段含义也不同，详情参考代码释义        |



# 6. 附录

## 6.1. 字典定义

### 6.1.1. <a id="EMQSzeDefine">深交所相关常量定义</a>

| 宏定义                                    | 值   | 描述                         |
| ----------------------------------------- | ---- | ---------------------------- |
| SZE_EXCHANGE_ID                           | 101  | 深交所 exchange_id           |
| <a id="SZE_SYMBOL_LEN">SZE_SYMBOL_LEN</a> | 9    | 深交所合约长度               |
| <a id="SZE_LEV2_DEPTH">SZE_LEV2_DEPTH</a> | 10   | 深交所快照深度               |
| SZE_IBR_DEPTH                             | 5    | 深交所 IBR 建树深度          |
| SZE_LEV2_SNAP_MSG_TYPE                    | 21   | 深交所快照消息类型           |
| SZE_LEV2_IDX_MSG_TYPE                     | 22   | 深交所指数消息类型           |
| SZE_LEV2_ORDER_MSG_TYPE                   | 23   | 深交所订单消息类型           |
| SZE_LEV2_EXE_MSG_TYPE                     | 24   | 深交所成交消息类型           |
| SZE_LEV2_AF_CLOSE_MSG_TYPE                | 25   | 深交所盘后定价消息类型       |
| SZE_LEV2_TREE_MSG_TYPE                    | 29   | 深交所建树消息类型           |
| SZE_LEV2_IBR_TREE_MSG_TYPE                | 28   | 深交所 IBR 建树快照          |
| SZE_LEV2_TURNOVER_MSG_TYPE                | 26   | 深交所成交量统计快照         |
| SZE_LEV2_BOND_SNAP_MSG_TYPE               | 80   | 深交所债券快照消息类型       |
| SZE_LEV2_BOND_ORDER_MSG_TYPE              | 81   | 深交所债券逐笔委托消息类型   |
| SZE_LEV2_BOND_EXE_MSG_TYPE                | 82   | 深交所债券逐笔成交消息类型   |
| SZE_LEV2_SNAP_HT_MSG_TYPE                 | 121  | 深交所快照心跳消息类型       |
| SZE_LEV2_IDX_HT_MSG_TYPE                  | 122  | 深交所指数心跳消息类型       |
| SZE_LEV2_TICK_HT_MSG_TYPE                 | 123  | 深交所逐笔消息心跳消息类型   |
| SZE_LEV2_AF_CLOSE_HT_MSG_TYPE             | 125  | 深交所盘后定价心跳消息类型   |
| SZE_LEV2_TREE_HT_MSG_TYPE                 | 129  | 深交所全新快照心跳消息类型   |
| SZE_LEV2_IBR_TREE_HT_MSG_TYPE             | 128  | 深交所 IBR 建树快照心跳消息  |
| SZE_LEV2_TURNOVER_HT_MSG_TYPE             | 126  | 深交所成交量统计快照心跳消息 |
| SZE_LEV2_BOND_SNAP_HT_MSG_TYPE            | 180  | 深交所债券快照心跳消息类型   |
| SZE_LEV2_BOND_TICK_HT_MSG_TYPE            | 181  | 深交所债券逐笔心跳消息类型   |

### 6.1.2. <a id="EMQSzeDefine">上交所相关常量定义</a>

| 宏定义                                    | 值   | 描述                             |
| ----------------------------------------- | ---- | -------------------------------- |
| SSE_EXCHANGE_ID                           | 100  | 上交所 exchange_id               |
| <a id="SSE_SYMBOL_LEN">SSE_SYMBOL_LEN</a> | 9    | 上交所合约长度                   |
| <a id=SSE_LEV2_DEPTH>SSE_LEV2_DEPTH</a>                            | 10   | 上交所 lev2 档位深度             |
| SSE_OPTION_DEPTH                          | 5    | 上交所期权档位深度               |
| SSE_SECURITY_NAME_LEN                     | 9    | 上交所证券名称长度               |
| SSE_PRODUCT_STATUS_LEN                    | 21   | 上交所产品状态长度               |
| DATE_LEN                                  | 9    | 日期长度                         |
| SSE_LEV2_IDX_MSG_TYPE                     | 33   | 上交所 lev2 指数消息类型         |
| SSE_LEV2_EXE_MSG_TYPE                     | 34   | 上交所 lev2 成交消息类型         |
| SSE_LEV2_OPT_MSG_TYPE                     | 35   | 上交所 lev2 期权消息类型         |
| SSE_LEV2_SNAP_MSG_TYPE                    | 39   | 上交所 lev2 快照消息类型         |
| SSE_LEV2_ORDER_MSG_TYPE                   | 32   | 上交所 lev2 委托信息类型         |
| SSE_LEV2_TREE_MSG_TYPE                    | 38   | 上交所 lev2 建树信息类型         |
| SSE_LEV2_BOND_SNAP_MSG_TYPE               | 60   | 上交所 lev2 债券快照信息类型     |
| SSE_LEV2_BOND_TICK_MSG_TYPE               | 61   | 上交所 lev2 债券逐笔信息类型     |
| SSE_LEV2_TICK_MERGE_MSG_TYPE              | 62   | 上交所 lev2 逐笔合并信息类型     |
| SSE_LEV2_ETF_MSG_TYPE                     | 63   | 上交所 lev2 ETF 信息类型         |
| SSE_LEV2_STATIC_INFO_MSG_TYPE             | 255  | 上交所 lev2 静态信息类型         |
| SSE_LEV2_IDX_HT_MSG_TYPE                  | 133  | 上交所 lev2 指数心跳消息类型     |
| SSE_LEV2_TICK_HT_MSG_TYPE                 | 132  | 上交所 lev2 逐笔心跳信息类型     |
| SSE_LEV2_OPT_HT_MSG_TYPE                  | 135  | 上交所 lev2 期权心跳信息类型     |
| SSE_LEV2_TREE_HT_MSG_TYPE                 | 138  | 上交所 lev2 建树心跳信息类型     |
| SSE_LEV2_SNAP_HT_MSG_TYPE                 | 139  | 上交所 lev2 快照心跳信息类型     |
| SSE_LEV2_BOND_SNAP_HT_MSG_TYPE            | 160  | 上交所 lev2 债券快照心跳信息类型 |
| SSE_LEV2_BOND_TICK_HT_MSG_TYPE            | 161  | 上交所 lev2 债券逐笔心跳信息类型 |
| SSE_LEV2_TICK_MERGE_HT_MSG_TYPE           | 162  | 上交所 lev2 逐笔合并心跳信息类型 |
| SSE_LEV2_ETF_HT_MSG_TYPE                  | 163  | 上交所 lev2 ETF 心跳信息类型     |

### 6.1.3. <a id="CommonDefine">通用常量定义</a>

| 枚举                                  | 取值 | 定义说明             |
| :------------------------------------ | :--- | :------------------- |
| <a id="IP_LEN">IP_LEN</a>             | 64   | IP字符串地址长度定义 |
| <a id="ETH_NAME_LEN">ETH_NAME_LEN</a> | 64   | Eth字符串长度定义    |

### 6.1.4. <a id="EMQSzeTickType">证券类别-EMQSzeTickType</a>

| 枚举               | 取值 | 定义说明     |
| :----------------- | :--- | :----------- |
| kTickTypeOrder     | 0    | 逐笔委托     |
| kTickTypeExe       | 1    | 逐笔成交     |
| kTickTypeBondOrder | 2    | 债券逐笔委托 |
| kTickTypeBondExe   | 3    | 债券逐笔成交 |

### 6.1.5. <a id="EMQRecvMode">接收模式-EMQRecvMode</a>

| 枚举    | 取值 | 定义说明            |
| :------ | :--- | :------------------ |
| kNormal | 0    |                     |
| kEFVI   | 1    | solarflare efvi接收 |

### 6.1.6. <a id="EMQType">行情类型-EMQType</a>

| 枚举         | 取值 | 定义说明       |
| ------------ | ---- | -------------- |
| kSzeSnap     | 0    | 深交所快照     |
| kSzeTick     | 1    | 深交所逐笔     |
| kSzeIndex    | 2    | 深交所指数     |
| kSzeBondSnap | 3    | 深交所债券快照 |
| kSzeBondTick | 4    | 深交所债券逐笔 |
| kSzeTree     | 5    | 深交所建树     |
| kSseSnap     | 6    | 上交所快照     |
| kSseTick     | 7    | 上交所逐笔     |
| kSseIndex    | 8    | 上交所指数     |
| kSseBondSnap | 9    | 上交所债券快照 |
| kSseBondTick | 10   | 上交所债券逐笔 |
| kSseTree     | 11   | 上交所建树     |

### 6.1.7. <a id="EMQExchangeType">交易所类型-EMQExchangeType</a>

| 枚举            | 取值 | 定义说明 |
| --------------- | ---- | -------- |
| EMQ_EXCHANGE_SH | 1    | 上交所   |
| EMQ_EXCHANGE_SZ | 2    | 深交所   |

### 6.1.8. <a id="EMQLogLevel">日志级别-EMQLogLevel</a>

| 枚举                | 取值 | 定义说明     |
| ------------------- | ---- | ------------ |
| EMQ_LOG_LEVEL_FATAL | 0    | 严重错误级别 |
| EMQ_LOG_LEVEL_ERROR | 1    | 错误级别 |
| EMQ_LOG_LEVEL_WARNING | 2    | 警告级别 |
| EMQ_LOG_LEVEL_INFO | 3    | 通知级别 |
| EMQ_LOG_LEVEL_DEBUG | 4    | 调试级别 |
| EMQ_LOG_LEVEL_TRACE | 5    | 跟踪级别 |