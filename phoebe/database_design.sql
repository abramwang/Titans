-- 华鑫证券业绩管理数据库设计
-- 数据库名称: trading_performance

CREATE DATABASE IF NOT EXISTS trading_performance 
DEFAULT CHARACTER SET utf8mb4 
DEFAULT COLLATE utf8mb4_unicode_ci;

USE trading_performance;

-- 1. 账户信息表
CREATE TABLE account_info (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account_id VARCHAR(50) NOT NULL COMMENT '账户编号，如314000045768',
    account_name VARCHAR(100) NOT NULL COMMENT '账户名称',
    broker VARCHAR(50) NOT NULL COMMENT '券商名称，如华鑫',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_account_id (account_id)
) COMMENT '账户基本信息表';

-- 2. 证券基础信息表
CREATE TABLE securities (
    id INT AUTO_INCREMENT PRIMARY KEY,
    security_code VARCHAR(20) NOT NULL COMMENT '证券代码，如159995.SZ',
    security_name VARCHAR(100) NOT NULL COMMENT '证券名称，如芯片ETF',
    exchange VARCHAR(10) NOT NULL COMMENT '交易所，SH/SZ',
    security_type VARCHAR(20) DEFAULT 'ETF' COMMENT '证券类型：ETF/股票等',
    is_active BOOLEAN DEFAULT TRUE COMMENT '是否有效',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_security_code (security_code),
    INDEX idx_security_name (security_name),
    INDEX idx_exchange (exchange)
) COMMENT '证券基础信息表';

-- 3. 日度账户资产表（来源于业绩统计文件）
CREATE TABLE daily_account_assets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account_id VARCHAR(50) NOT NULL COMMENT '账户编号',
    trade_date DATE NOT NULL COMMENT '交易日期',
    start_assets DECIMAL(15,2) COMMENT '日初账户资产(不含公募占款)',
    end_assets DECIMAL(15,2) COMMENT '日末账户资产(不含公募占款)',
    net_value DECIMAL(10,6) COMMENT '净值',
    custody_net_value DECIMAL(10,6) COMMENT '托管净值',
    estimated_profit DECIMAL(15,2) COMMENT '预估盈利',
    floating_pnl DECIMAL(15,2) COMMENT '浮动盈亏',
    capital_flow DECIMAL(15,2) COMMENT '出入金',
    cumulative_profit DECIMAL(15,2) COMMENT '累计盈利',
    profit_including_floating DECIMAL(15,2) COMMENT '包含浮动盈利',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_account_date (account_id, trade_date),
    INDEX idx_trade_date (trade_date),
    FOREIGN KEY (account_id) REFERENCES account_info(account_id)
) COMMENT '日度账户资产表';

-- 4. 日度交易明细表（来源于业绩估算文件）
CREATE TABLE daily_trading_details (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account_id VARCHAR(50) NOT NULL COMMENT '账户编号',
    trade_date DATE NOT NULL COMMENT '交易日期',
    security_code VARCHAR(20) NOT NULL COMMENT '证券代码',
    buy_price DECIMAL(10,4) COMMENT '买入价格',
    buy_quantity INT COMMENT '买入数量',
    sell_price DECIMAL(10,4) COMMENT '卖出价格',
    sell_quantity INT COMMENT '卖出数量',
    current_price DECIMAL(10,4) COMMENT '当前价格',
    current_market_value DECIMAL(15,2) COMMENT '当前市值',
    cost_amount DECIMAL(15,2) COMMENT '成本金额',
    profit DECIMAL(15,2) COMMENT '利润',
    trade_type VARCHAR(20) DEFAULT 'T0' COMMENT '交易类型：T0/T1等',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_account_date (account_id, trade_date),
    INDEX idx_security_code (security_code),
    INDEX idx_trade_date (trade_date),
    FOREIGN KEY (account_id) REFERENCES account_info(account_id),
    FOREIGN KEY (security_code) REFERENCES securities(security_code)
) COMMENT '日度交易明细表';

-- 5. 日度盈亏汇总表（来源于业绩估算文件的汇总数据）
CREATE TABLE daily_pnl_summary (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account_id VARCHAR(50) NOT NULL COMMENT '账户编号',
    trade_date DATE NOT NULL COMMENT '交易日期',
    settlement_pnl DECIMAL(15,2) COMMENT '结算盈亏',
    floating_pnl DECIMAL(15,2) COMMENT '浮动盈亏',
    total_pnl DECIMAL(15,2) COMMENT '总盈亏',
    total_trades INT DEFAULT 0 COMMENT '交易笔数',
    profitable_trades INT DEFAULT 0 COMMENT '盈利笔数',
    loss_trades INT DEFAULT 0 COMMENT '亏损笔数',
    max_single_profit DECIMAL(15,2) COMMENT '单笔最大盈利',
    max_single_loss DECIMAL(15,2) COMMENT '单笔最大亏损',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_account_date (account_id, trade_date),
    INDEX idx_trade_date (trade_date),
    FOREIGN KEY (account_id) REFERENCES account_info(account_id)
) COMMENT '日度盈亏汇总表';

-- 6. 持仓明细表（当前持仓情况）
CREATE TABLE position_details (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account_id VARCHAR(50) NOT NULL COMMENT '账户编号',
    security_code VARCHAR(20) NOT NULL COMMENT '证券代码',
    position_date DATE NOT NULL COMMENT '持仓日期',
    total_quantity INT NOT NULL DEFAULT 0 COMMENT '总持仓数量',
    available_quantity INT NOT NULL DEFAULT 0 COMMENT '可用数量',
    avg_cost_price DECIMAL(10,4) COMMENT '平均成本价',
    current_price DECIMAL(10,4) COMMENT '当前价格',
    market_value DECIMAL(15,2) COMMENT '市值',
    cost_value DECIMAL(15,2) COMMENT '成本价值',
    unrealized_pnl DECIMAL(15,2) COMMENT '未实现盈亏',
    unrealized_pnl_rate DECIMAL(8,4) COMMENT '未实现盈亏比例',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_account_security_date (account_id, security_code, position_date),
    INDEX idx_position_date (position_date),
    INDEX idx_security_code (security_code),
    FOREIGN KEY (account_id) REFERENCES account_info(account_id),
    FOREIGN KEY (security_code) REFERENCES securities(security_code)
) COMMENT '持仓明细表';

-- 7. 数据导入日志表
CREATE TABLE import_logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    file_name VARCHAR(255) NOT NULL COMMENT '导入文件名',
    file_type VARCHAR(50) NOT NULL COMMENT '文件类型：业绩估算/业绩统计',
    account_id VARCHAR(50) NOT NULL COMMENT '账户编号',
    trade_date DATE COMMENT '交易日期',
    import_status VARCHAR(20) DEFAULT 'SUCCESS' COMMENT '导入状态：SUCCESS/FAILED/PARTIAL',
    import_records INT DEFAULT 0 COMMENT '导入记录数',
    error_message TEXT COMMENT '错误信息',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_file_name (file_name),
    INDEX idx_import_status (import_status),
    INDEX idx_trade_date (trade_date)
) COMMENT '数据导入日志表';

-- 插入初始化数据
INSERT INTO account_info (account_id, account_name, broker) VALUES 
('314000045768', '华鑫证券账户', '华鑫证券');

-- 创建视图：日度绩效汇总视图
CREATE VIEW v_daily_performance AS
SELECT 
    daa.account_id,
    daa.trade_date,
    daa.start_assets,
    daa.end_assets,
    daa.estimated_profit,
    daa.floating_pnl,
    daa.cumulative_profit,
    dps.settlement_pnl,
    dps.total_trades,
    dps.profitable_trades,
    dps.loss_trades,
    CASE 
        WHEN dps.total_trades > 0 
        THEN ROUND(dps.profitable_trades * 100.0 / dps.total_trades, 2) 
        ELSE 0 
    END as win_rate,
    daa.end_assets - daa.start_assets as daily_asset_change
FROM daily_account_assets daa
LEFT JOIN daily_pnl_summary dps ON daa.account_id = dps.account_id AND daa.trade_date = dps.trade_date
ORDER BY daa.trade_date DESC;

-- 创建视图：证券交易统计视图
CREATE VIEW v_security_trading_stats AS
SELECT 
    dtd.security_code,
    s.security_name,
    COUNT(*) as total_trades,
    SUM(CASE WHEN dtd.profit > 0 THEN 1 ELSE 0 END) as profitable_trades,
    SUM(CASE WHEN dtd.profit < 0 THEN 1 ELSE 0 END) as loss_trades,
    SUM(dtd.profit) as total_profit,
    AVG(dtd.profit) as avg_profit,
    MAX(dtd.profit) as max_profit,
    MIN(dtd.profit) as min_profit,
    SUM(dtd.buy_quantity) as total_buy_quantity,
    SUM(dtd.sell_quantity) as total_sell_quantity
FROM daily_trading_details dtd
LEFT JOIN securities s ON dtd.security_code = s.security_code
WHERE dtd.profit IS NOT NULL
GROUP BY dtd.security_code, s.security_name
ORDER BY total_profit DESC;
