## EMT API常见问题

##### Q1 API是否支持自动重连？

* API支持自动重连。当发生断线时，API将回调`OnDisconnected()`断线通知；自动重连成功后，API将回调`OnConnected()`连接成功通知。

##### Q2 如发生断线，回调中应做何处理？

* 当客户端与交易后台的通信连接断开时，`OnDisconnected()`方法将被调用；API内部支持自动重连机制，此接口仅用做断线通知，需要快速返回，用户无需在此函数中做特殊处理。

##### Q3 API是否支持多账号登录？

* API支持多账户登录，不同账户使用登录返回的sessionid进行区分。

##### Q4 同一个账号是否支持使用多个客户端进行多点登录？

* TCP交易模式支持单账号多点登录；需注意多点登录时，同一账号所使用的不同客户端的client_id不可相同，即同一账号同一client_id只可建立一个会话，在该会话退出前，不可重复登录。
* UDP交易模式不允许单账号UDP多点登录，UDP与TCP可多点登录。

##### Q5 client_id可以为任意数吗？

* client_id请在[1-127]范围区间内取值。

##### Q6 订阅公共流SubscribePublicTopic的Restart方式和Quick方式是什么区别？

* Restart方式将在Login时推送当交易日全部交易流水；Quick方式只会推送本次登录后所执行的的交易回报；注意该方法需在Login前调用生效。

##### Q7 Linux系统下调用Login接口非root用户会报警告，且采集不到终端信息？

* 系统告警信息如下：

  ```
  WARNING: you should run this program as super-user.
  WARNING: output may be incomplete or inaccurate, you should run this program as super-user.
  ```

* 此问题原因是您当前使用的用户无lshw命令权限，请首先使用root用户安装lshw命令，再visudo配置sudo权限（或修改/etc/sudoers），如下以test用户为例进行配置：

  ```
  > whereis lshw
  lshw: /usr/sbin/lshw /usr/share/man/man1/lshw.1.gz
  
  > visudo
  ## Allow root to run any commands anywhere
  root    ALL=(ALL)       ALL
  test    ALL=(ALL)       /usr/sbin/lshw
  
  ## Allows people in group wheel to run all commands
  %wheel  ALL=(ALL)       ALL
  test    ALL=(ALL)       NOPASSWD:ALL
  ```

##### Q8 调用InserOrder接口返回的订单号order_emt_id是唯一的吗？

* 在当前交易日内，保证全局唯一；跨交易日不保证唯一性。

##### Q9 获取到的错误信息为何出现乱码？

* API错误信息中文编码统一为UTF-8，烦请检查编码格式。

##### Q10 启动程序报错“无法加载接口驱动，错误编号126”，如何处理？

* 该问题可能由于您windows系统中vc环境不完整，请下载`msvcp140.dll`、`vcruntime140.dll`、`vcruntime140_1.dll`，放置于emt_api.dll同级目录下后再次运行程序；该vc运行库也可联系EMT技术支持人员获取。

##### Q11 登录后交易和查询均正常，但过一小段时间后交易和查询报错1003（消息发送失败）怎么回事？

* 该问题为心跳超时，服务端主动断开此连接导致；请检查设置心跳间隔接口`SetHeartBeatInterval()`，可选择不调用，或设置为5s到15s内任意值。

##### Q12 实盘EMT系统什么时间启动？

* 早8点35分系统开始提供服务。

##### Q13 若干分页查询方法该如何使用？

* EMT API提供的若干分页查询方法使用逻辑一致；
* 请求时入参`req_count`为单页请求记录数，限制为200条之内，`reference`为上一次收到的分页查询结果中带回来的索引号，如您本次是从头查询，请传0；
* 在回调函数中，当`sequence`为0，表明当次查询没有查到任何记录；当`is_last`为true时，如果`sequence`等于`req_count`，表示可能还有后续结果，可以进行下一次分页查询请求，如果不等，表示所有结果已经查询完毕；一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息。

##### Q14 订单推送状态有哪些？

* 订单推送会调用`OnOrderEvent()`方法，该方法可能收到以下6种订单状态：订单初始化、已报未成交、全部成交、部分撤单、全部撤单、拒单；订单的部分成交状态仅通过`OnTradeEvent()`方法回调。

##### Q15 委托下单同步返回的订单编号和委托推送回调有先后时序关系吗？

* `InsertOrder()`下单接口同步返回的订单编号在先，`OnOrderEvent()`订单推送在后。

##### Q16 仿真测试环境UDP交易模式无法登录？

* 实盘支持UDP/TCP两种API交易模式，仿真测试请使用TCP接入。

##### Q17 进行市价委托时，可用资金高于当前市价价格但报错资金不足？

* 深市市价委托时柜台将按涨停价预扣资金，沪市市价委托时柜台将按保护限价预扣资金。

##### Q18 调用绑核方法SetCpuAffinity后，为何使用top看不到？

* `SetCpuAffinity()`为将API内部处理线程绑核，可通过`top -Hp pid`查看线程绑核情况，并确保该方法在Login前调用。

##### Q19 实盘的报单流速是如何控制的？

* 报单流速由EMT交易系统控制，用户当前无法自行设置，超过流控阈值将报错211008（超过限定流速）。

##### Q20 两地分仓模式下的资金是如何分配的？

* 账户内可用资金每个交易日日初将按初始资金比例分配，初始资金比例将在切换两地分仓模式时指定，如需调整可联系对接人员。

##### Q21 InsertOrder接口是线程安全的吗？

* 是。

##### Q22 两融担保品划转持仓数量是实时增减吗？

* 担保品转出为实时扣减数量；担保品转入并不会实时增加持仓数量，需当日清算后确认数量。

##### Q23 Login登录报错4104（clientid重复）如何处理？

* clientid重复即当前账户已经有一个相同clientid的客户端登录过且仍未退出，请检查该账户的多点登录情况。

##### Q24 Login登录报错4102（白名单检测失败）如何处理？

* 实盘账户转入EMT系统时需绑定白名单IP，如使用未开设白名单的服务器执行API登录，将报此错误。

##### Q25 交易API回调方法是单线程串行的吗？

* 不是；查询类回调方法保证为串行回调，交易类回调方法如`OnOrderEvent()`、`OnTradeEvent()`、`OnCancelOrderError()`等可能由另一内部线程回调。

##### Q26 行情API调用登录接口成功后，订阅行情为什么会返回错误码“-10”，并且报错“no session”？

* 在调用`CreateQuoteApi()`创建行情API实例时要保证其参数data_type值与账号权限一致：
  * data_type=1：L1权限； data_type=2：L2权限；

##### Q27 行情查询接口调用频率是多少？

* `QueryAllTickers() / QueryAllTickersFullInfo()`调用频率为8s~10s一次；
* `QueryLatestInfo() / QueryTickersPriceInfo() / QueryMinuteInfo() / QueryMinHistoryInfo()`调用频率为1s~3s一次；
* 实盘请确保控制查询接口调用频率。

##### Q28 行情查询接口的回调方法应如何使用？

* 查询接口仅需调用一次，回调函数即不断接收查询数据，直至is_last标志为true，表示查询数据全部接收。回调函数接收数据时无需再次调用查询接口。

##### Q29 日志输出等级是否可选择？

* 在调用`CreateTraderApi()`创建交易API实例时可选择日志等级，交易API日志不会产生控制台输出。

* 在调用`CreateQuoteApi()`创建行情API实例时可选择日志等级，日志等级对控制台输出及日志文件均有影响。

##### Q30 为什么订阅逐笔行情时告知不支持该消息类型？

* 需确保使用的账号支持L2行情权限。

##### Q31 查询最新快照信息与订阅快照行情有什么区别？

* 查询接口为非实时行情，订阅接口返回实时行情。
* 查询回调方法调用次数与查询数据量相关，订阅回调方法在退订前始终活跃。

##### Q32 行情API回调方法之间是单线程串行的吗？

* 是。

##### Q33 行情API断线重连后需要重新订阅吗？

* 在断线重连前执行成功的订阅会被恢复，无需用户发起重新订阅。

##### Q34 分时行情与实时行情有何区别？

* 分时行情按分钟为周期推送，实时行情与交易所行情推送保持同步。建议积极使用实时行情。

##### Q35 仿真测试环境提供L2行情吗？

* 仿真测试环境仅提供L1行情，暂不提供L2行情。

##### Q36 订单委托中的order_client_id如何填写？

* 订单委托中入参order_client_id为客户报单编号，完全由客户自定义，订单推送中会将该字段带回。

##### Q37 OnOrderEvent如何区分该笔为委托还是撤单？

* 可使用`OnOrderEvent()`回调`EMTOrderInfo`中的`order_submit_status`提交状态来判断，也可使用`order_cancel_emt_id`非0来判断。

##### Q38 如何唯一标识一笔成交？

* 可使用`OnTradeEvent()`回调`EMTTradeReport`中的`exec_id`交易所成交编号来唯一标识一笔成交，当发现存在两笔成交回报`exec_id`相同，则为自成交。

##### Q39 请求接口中的request_id是什么规则？如果请求填写相同的request_id，会在应答中原样带回吗？

* 请求接口中的`request_id`完全由用户自定义，请求中填写的`request_id`会在对应的应答回调方法中原样带回。

##### Q40 一个账户在多点登录的情况下，订单推送多个端都会收到，如何过滤本端自己的订单？

* 推荐使用`GetClientIDByEMTID()`方法获取该笔订单下单的客户端id，由于不同端`client_id`各不相同，可用此方法过滤本端自己的订单。此方法为同步返回。

##### Q41 订单终态有哪些？

* 订单终态可能为全部成交、部分撤单、已撤单、已拒绝。

##### Q42 CancelOrder撤单时回调报错6084（无效撤单方式）是什么原因？

* `CancelOrder()`接口需传入`InsertOrder()`返回的系统订单编号`order_emt_id`（当日保证全局唯一），请检查撤单接口传入的订单号是否为该编号，并检查接收`InsertOrder()`返回值的字段类型定义应为`uint64_t`而不是`int32_t`或`uint32_t`，否则赋值将被截断。

##### Q43 两融委托拒单报错230101（委托类型不支持）是什么原因？

* 请检查`InsertOrder()`入参中的`position_effect`开平标志，两融委托时开平标志应填`EMT_POSITION_EFFECT_INIT`。

##### Q44 两融委托拒单报错230522（业务不支持）是什么原因？

* 请检查`InsertOrder()`入参中的`business_type`业务类型，两融委托时业务类型应填`EMT_BUSINESS_TYPE_MARGIN`。

##### Q45 一次ETF申赎交易都可以收到哪些成交回报？

* 一条成交类型为EMT_TRDT_PRIMARY（申赎成交，值为'3'）、证券代码为ETF二级市场交易代码、成交数量为申购数量的成交记录。
* 若干条成交类型为EMT_TRDT_COMPONENT（成分股成交，值为'2'）、证券代码为ETF成份证券代码、成交数量为过户数量成交记录。
* 一条成交类型为EMT_TRDT_CASH（现金替代成交，值为'1'）、证券代码为ETF二级市场交易代码、成交金额为现金替代总额、成交数量为1的成交记录。

##### Q46 订单推送的流程状态是怎么变化的？

* 柜台收到报单并前置校验通过后，会回调OnOrderEvent，order_status为EMT_ORDER_STATUS_INIT（初始化），order_submit_status为EMT_ORDER_SUBMIT_STATUS_INSERT_SUBMITTED（已提交）；
* 柜台报至交易所，交易所接受后，会回调OnOrderEvent，order_status为EMT_ORDER_STATUS_NOTRADEQUEUEING（未成交），order_submit_status为EMT_ORDER_SUBMIT_STATUS_INSERT_ACCEPTED（已接受）；
* 如果有部分成交情况，每笔成交后，都会回调OnTradeEvent，quantity为此次成交的数量。
* 全部成交后，会回调OnOrderEvent，order_status为EMT_ORDER_STATUS_ALLTRADED（全部成交），order_submit_status为EMT_ORDER_SUBMIT_STATUS_INSERT_ACCEPTED（已接受）；同时回调OnTradeEvent，quantity为此次成交的数量。
* 发生拒单后，会回调OnOrderEvent，order_status为EMT_ORDER_STATUS_REJECTED（拒单），order_submit_status为EMT_ORDER_SUBMIT_STATUS_INSERT_REJECTED（拒单）；

##### Q47 发起撤单后订单推送的流程状态是怎么变化的？

* 柜台收到撤单请求后，校验订单是否可撤，如果校验不通过则回调OnCancelOrderError，error_info有相应错误码；如果校验通过，进入下一步；
* 柜台校验通过，将撤单请求报至交易所，在收到交易所回报后，如果交易所报错，会回调OnCancelOrderError，error_info有相应错误码；如果交易所回报成功，会回调OnOrderEvent，order_status根据成交情况为EMT_ORDER_STATUS_PARTTRADEDNOTQUEUEING（部分撤单）或者EMT_ORDER_STATUS_CANCELED（全部撤单），order_submit_status为EMT_ORDER_SUBMIT_STATUS_CANCEL_ACCEPTED（撤单已被接受）；

##### Q48 API调用登录接口时需要填写密码吗？
* 当前柜台通过客户机ip白名单进行登录鉴权控制，客户通过API登录时不对密码字段进行校验，因而无需填写密码；
* 如果通过EMC客户端登录，需要正确填写注册时获取的密码。

##### Q49 QueryAsset接口和QueryFund接口有什么区别？
* QueryAsset接口查询客户资产情况，包括总资产和证券资产字段等，字段较全；下单后快速查询情况下部分字段更新可能有延时。
* QueryFund接口查询客户资金情况。本接口返回的字段保证是最新的资金信息。