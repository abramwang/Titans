# 华鑫证券Excel数据业务逻辑说明

最后更新: 2025-08-11 16:55:49

## 数据文件结构

### 汇总文件
- **华鑫_314000045768_业绩统计.xlsx**: 日度账户资产变化和绩效汇总

### 明细文件  
- **华鑫_314000045768_业绩估算_YYYY-MM-DD.xlsx**: 每日详细交易和持仓数据

## 每日Excel文件的4个数据部分

### 1. 【日内底仓T0】
- **数据类型**: 交易数据
- **业务说明**: 日内T0交易操作
- **分析用途**: 分析日内交易策略和执行效果

### 2. 【ETF日内申赎】
- **数据类型**: 交易数据
- **业务说明**: ETF申购赎回操作
- **分析用途**: 分析ETF申赎策略和套利效果

### 3. 【隔夜底仓】
- **数据类型**: 持仓数据
- **业务说明**: 隔夜持有的底仓
- **分析用途**: 分析隔夜持仓风险和收益

### 4. 【ETF赎回涨停票】
- **数据类型**: 持仓数据
- **业务说明**: ETF赎回获得的涨停股票
- **分析用途**: 分析涨停股票持仓价值

## 业务逻辑分组

### 🔄 交易数据组 (Trading Data)
- **组成**: 日内底仓T0 + ETF日内申赎
- **业务含义**: 当日的所有主动交易操作
- **分析价值**: 
  - 交易策略效果评估
  - 交易成本分析
  - T0和申赎策略对比
  - 日内交易损益计算

### 📦 持仓数据组 (Position Data)
- **组成**: 隔夜底仓 + ETF赎回涨停票
- **业务含义**: 当日收盘时的实际持仓状况
- **分析价值**:
  - 资产配置分析
  - 持仓风险评估
  - 隔夜持仓收益计算
  - 涨停股票价值分析

## 数据库设计建议

### 表结构建议
```sql
-- 交易明细表 (Trading Details)
CREATE TABLE trading_details (
    trade_date DATE,
    security_code VARCHAR(20),
    section_type ENUM('日内底仓T0', 'ETF日内申赎'),
    -- 其他交易字段...
);

-- 持仓明细表 (Position Details) 
CREATE TABLE position_details (
    trade_date DATE,
    security_code VARCHAR(20),
    section_type ENUM('隔夜底仓', 'ETF赎回涨停票'),
    -- 其他持仓字段...
);
```

### 分析查询示例
```sql
-- 交易数据分析
SELECT trade_date, section_type, SUM(profit) as total_profit
FROM trading_details 
GROUP BY trade_date, section_type;

-- 持仓数据分析
SELECT trade_date, section_type, SUM(market_value) as total_position
FROM position_details
GROUP BY trade_date, section_type;
```

## 系统实现建议

1. **数据导入时分类**:
   - 识别4个数据部分的边界
   - 按业务逻辑分别存储到不同表/字段

2. **分析报告分层**:
   - 交易层面: 基于交易数据组
   - 持仓层面: 基于持仓数据组
   - 综合层面: 结合汇总数据

3. **风险控制**:
   - 持仓集中度监控
   - 交易频率分析
   - 隔夜风险评估
