# 华鑫证券业绩数据分析系统

## 项目概述

本项目是一个专门用于分析和管理华鑫证券交易账表数据的系统。系统能够：

1. 解析Excel格式的业绩估算和业绩统计文件
2. 将数据规范化存储到MySQL数据库中
3. 提供数据查询和分析功能

## 数据库设计

### 核心表结构

1. **account_info** - 账户基本信息表
2. **securities** - 证券基础信息表
3. **daily_account_assets** - 日度账户资产表（来源：业绩统计文件）
4. **daily_trading_details** - 日度交易明细表（来源：业绩估算文件）
5. **daily_pnl_summary** - 日度盈亏汇总表
6. **position_details** - 持仓明细表
7. **import_logs** - 数据导入日志表

### 数据来源分析

#### 业绩统计文件 (华鑫_314000045768_业绩统计.xlsx)
- 包含日度账户资产变化
- 字段：日期、日初资产、日末资产、预估盈利、浮动盈亏、累计盈利等

#### 业绩估算文件 (华鑫_314000045768_业绩估算_YYYY-MM-DD.xlsx)
- 包含具体的交易明细和持仓信息
- 字段：证券代码、证券名称、买入价格、买入数量、卖出价格、卖出数量、当前价格、利润等

## 安装和配置

### 1. 环境准备

```bash
# 激活虚拟环境
source env/bin/activate

# 安装依赖包
pip install pandas openpyxl xlrd pymysql sqlalchemy
```

### 2. 数据库准备

```bash
# 登录MySQL
mysql -u root -p

# 创建数据库和表
source database_design.sql
```

### 3. 配置数据库连接

编辑 `config.py` 文件，修改数据库连接参数：

```python
DATABASE_CONFIG = {
    'host': 'localhost',
    'port': 3306,
    'user': 'your_username',
    'password': 'your_password',
    'database': 'trading_performance',
    'charset': 'utf8mb4'
}
```

## 使用方法

### 1. 数据分析

```bash
# 分析Excel文件结构
python analyze_excel.py

# 详细数据分析
python detailed_analysis.py
```

### 2. 数据导入

```bash
# 导入所有Excel文件到数据库
python data_importer.py
```

### 3. 数据查询

```sql
-- 查看日度绩效
SELECT * FROM v_daily_performance ORDER BY trade_date DESC;

-- 查看证券交易统计
SELECT * FROM v_security_trading_stats ORDER BY total_profit DESC;

-- 查看最近的交易明细
SELECT 
    dtd.trade_date,
    dtd.security_code,
    s.security_name,
    dtd.buy_price,
    dtd.buy_quantity,
    dtd.profit
FROM daily_trading_details dtd
LEFT JOIN securities s ON dtd.security_code = s.security_code
ORDER BY dtd.trade_date DESC
LIMIT 20;
```

## 文件说明

- `database_design.sql` - 数据库表结构设计
- `analyze_excel.py` - Excel文件结构分析脚本
- `detailed_analysis.py` - 详细数据分析脚本
- `data_importer.py` - 数据导入脚本
- `config.py` - 配置文件

## 数据特点

1. **Excel文件格式不规范**：
   - 列名可能为空或包含 "Unnamed" 
   - 数据可能包含合并单元格
   - 数值格式不统一（包含逗号、小数点位置不固定）

2. **业绩估算文件特点**：
   - 每个文件对应一个交易日
   - 包含多条交易记录
   - 最后几行通常是汇总数据

3. **业绩统计文件特点**：
   - 包含一段时间的汇总数据
   - 每行对应一个交易日
   - 数据相对规范

## 注意事项

1. 导入前请确保MySQL服务运行正常
2. 首次导入前需要执行 `database_design.sql` 创建表结构
3. 重复导入会跳过已存在的记录
4. 所有导入操作都会记录在 `import_logs` 表中
5. 建议定期备份数据库

## 扩展功能

可以基于此数据库进一步开发：

1. Web界面的数据展示和分析
2. 绩效报告自动生成
3. 风险控制指标计算
4. 交易策略回测分析
