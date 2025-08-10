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
COLUMN_MAPPING = {
    # 业绩统计文件列名映射
    'summary_columns': {
        '日期': 'trade_date',
        '日初账户资产(不含公募占款)': 'start_assets',
        '日末账户资产(不含公募占款)': 'end_assets',
        '净值': 'net_value',
        '托管净值': 'custody_net_value',
        '预估盈利': 'estimated_profit',
        '浮动盈亏': 'floating_pnl',
        '出入金': 'capital_flow',
        '累计盈利': 'cumulative_profit',
        '包含浮动盈利': 'profit_including_floating'
    },
    
    # 业绩估算文件列位置映射
    'daily_columns': {
        0: 'security_code',
        1: 'security_name',
        2: 'buy_price',
        3: 'buy_quantity',
        4: 'sell_price',
        5: 'sell_quantity',
        6: 'current_price',
        7: 'current_market_value',
        8: 'cost_amount',
        9: 'profit'
    }
}

# 证券类型映射
SECURITY_TYPE_MAPPING = {
    'ETF': ['ETF'],
    '股票': ['股份', '科技', '重工', '钢', '材料'],
    '基金': ['基金']
}
