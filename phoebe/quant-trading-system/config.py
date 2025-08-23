# 数据库配置文件
# 请根据实际情况修改以下配置

DATABASE_CONFIG = {
    'host': 'localhost',
    'port': 3307,
    'user': 'root',
    'password': 'MySecureRootPassword123!',
    'database': 'ti_trading_performance',
    'charset': 'utf8mb4'
}

# 日志配置
LOGGING_CONFIG = {
    'level': 'INFO',
    'format': '%(asctime)s - %(levelname)s - %(message)s',
    'file': 'import.log'
}

# 数据映射配置
BUSINESS_TYPE_MAPPING = {
    '证券': 'securities',
    '股票': 'securities',
    '期货': 'futures',
    '期权': 'options'
}

EXCEL_COLUMNS_MAPPING = {
    '股票代码': 'symbol',
    '证券代码': 'symbol',
    '证券名称': 'name',
    '股票名称': 'name',
    '业务标识': 'business_type',
    '买卖标志': 'action_type',
    '成交数量': 'volume',
    '成交金额': 'amount',
    '成交价格': 'price',
    '委托时间': 'order_time',
    '成交时间': 'execution_time',
    '佣金费用': 'commission',
    '过户费': 'transfer_fee',
    '印花税': 'stamp_duty',
    '其他费用': 'other_fees',
    '净收付金额': 'net_amount',
    '剩余数量': 'remaining_quantity',
    '当前持仓': 'current_position',
    '持仓成本': 'cost_basis',
    '持仓市值': 'market_value',
    '浮动盈亏': 'unrealized_pnl',
    '实现盈亏': 'realized_pnl'
}

# Web应用配置
WEB_CONFIG = {
    'host': '0.0.0.0',
    'port': 5000,
    'debug': True
}
