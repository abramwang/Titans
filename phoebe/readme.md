# Phoebe
对交易账表数据进行维护和统计项目

## 项目功能

- 解析华鑫证券Excel格式的业绩估算和业绩统计文件
- 将数据规范化存储到MySQL数据库
- 提供交易数据的分析和查询功能

## 快速开始

### 1. Python 虚拟环境

创建虚拟环境
```shell
python3 -m venv env
```

激活虚拟环境
```shell
source env/bin/activate
```

使用国内源
```shell
pip3 config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
```

安装依赖
```shell
pip install pandas openpyxl xlrd pymysql sqlalchemy
```

### 2. 数据库设置

创建MySQL数据库并执行建表脚本：
```shell
mysql -u root -p < database_design.sql
```

### 3. 数据分析和导入

#### 方式一：使用图形化菜单（推荐）
```shell
python main.py
```

#### 方式二：单独运行脚本
分析Excel文件：
```shell
python analyze_excel.py          # 基础分析
python detailed_analysis.py      # 详细分析
```

创建数据库：
```shell
python create_database.py        # 创建数据库和表结构
```

导入数据到数据库：
```shell
python data_importer.py          # 导入Excel数据
```

验证和分析数据：
```shell
python verify_data.py            # 验证导入的数据
python advanced_analysis.py      # 生成高级分析报告
```

## 主要功能

### 1. 数据导入功能
- ✅ 自动解析Excel文件格式
- ✅ 智能处理数据不规范问题
- ✅ 批量导入多个文件
- ✅ 完整的错误处理和日志记录

### 2. 数据分析功能
- ✅ 综合绩效分析报告
- ✅ 收益风险分析
- ✅ 证券交易统计
- ✅ 时间序列分析
- ✅ 导出Excel详细报告

### 3. 数据查询功能
- ✅ 日度绩效查询
- ✅ 交易明细查询
- ✅ 证券统计查询
- ✅ 自定义SQL查询

## 系统特点

1. **智能数据处理**：自动识别和清理Excel文件格式问题
2. **完整数据库设计**：规范化的MySQL数据库结构
3. **丰富的分析功能**：从基础统计到高级风险分析
4. **友好的用户界面**：图形化菜单和命令行工具
5. **详细的日志记录**：完整的导入和错误日志
6. **可扩展架构**：模块化设计便于功能扩展

## 文件说明

- `data/` - Excel数据文件目录
- `database_design.sql` - 数据库表结构设计
- `main.py` - 主程序入口（图形化菜单）
- `analyze_excel.py` - Excel文件分析脚本
- `detailed_analysis.py` - 详细数据分析脚本
- `create_database.py` - 数据库创建脚本
- `data_importer.py` - 数据导入脚本
- `verify_data.py` - 数据验证脚本
- `advanced_analysis.py` - 高级分析报告脚本
- `config.py` - 配置文件
- `README_DATABASE.md` - 详细的数据库设计文档

## 数据导入结果

✅ **导入完成情况:**
- 账户信息: 1 条记录
- 证券信息: 101 条记录  
- 日度账户资产: 25 条记录
- 交易明细: 209 条记录
- 盈亏汇总: 14 条记录
- 导入日志: 15 条记录

✅ **绩效概览:**
- 交易天数: 25 天 (2025年7月-8月)
- 累计收益: 522,692.34 元
- 总收益率: 5.23%
- 交易胜率: 76.00%
- 最佳收益证券: 通信ETF (546,778.85元)

## 快速使用

1. **一键启动**: `python main.py`
2. **验证数据**: `python verify_data.py` 
3. **分析报告**: `python advanced_analysis.py`