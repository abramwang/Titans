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

分析Excel文件：
```shell
python analyze_excel.py          # 基础分析
python detailed_analysis.py      # 详细分析
```

导入数据到数据库：
```shell
python data_importer.py
```

## 文件说明

- `data/` - Excel数据文件目录
- `database_design.sql` - 数据库表结构设计
- `analyze_excel.py` - Excel文件分析脚本
- `detailed_analysis.py` - 详细数据分析脚本
- `data_importer.py` - 数据导入脚本
- `config.py` - 配置文件
- `README_DATABASE.md` - 详细的数据库设计文档