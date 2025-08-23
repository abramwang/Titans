# 量化交易系统

## 📊 项目概述

这是一个专为华鑫证券业绩管理设计的量化交易系统，能够实现每日Excel数据的自动化导入、交易结果查询显示以及类似QuantStats的绩效分析报告。

## 🚀 主要功能

### 1. 数据导入管理
- **增量导入**: 自动检测并导入`./data`目录下的新Excel文件
- **智能识别**: 支持4种业务类型的数据识别和分类
  - 日内底仓T0
  - ETF日内申赎
  - 隔夜底仓
  - ETF赎回涨停票
- **错误处理**: 完善的错误日志和重试机制

### 2. 交易概览仪表板
- **今日交易汇总**: 显示当日交易额、盈亏、手续费等关键指标
- **业务类型分析**: 不同业务类型的收益分布图表
- **最活跃证券**: 展示交易频次最高的证券及其表现
- **当前持仓**: 实时显示持仓情况、浮动盈亏和市值

### 3. 绩效分析报告
- **收益指标**: 总收益、夏普比率、年化收益率等
- **风险指标**: 最大回撤、波动率、VaR等
- **交易统计**: 胜率、盈亏比、连续盈亏天数
- **可视化图表**: 
  - 累计收益曲线
  - 每日收益分布直方图
  - 业务类型收益对比
  - 月度统计表格

## 🛠️ 技术架构

### 后端技术栈
- **框架**: Flask (轻量级Web框架)
- **数据库**: MySQL 8.0 + SQLAlchemy ORM
- **数据处理**: Pandas + NumPy
- **任务调度**: Schedule (定时任务)

### 前端技术栈
- **UI框架**: Bootstrap 5
- **图表库**: Chart.js
- **图标**: Font Awesome
- **交互**: jQuery + 原生JavaScript

### 项目结构
```
quant-trading-system/
├── src/                    # 源代码目录
│   ├── app.py             # Flask主应用
│   ├── scheduler.py       # 定时任务调度器
│   ├── models/            # 数据模型
│   │   └── database.py    # 数据库模型定义
│   └── services/          # 业务服务
│       ├── data_importer.py      # 数据导入服务
│       ├── trading_analytics.py  # 交易分析服务
│       └── portfolio_manager.py  # 投资组合管理服务
├── templates/             # HTML模板
│   ├── base.html         # 基础模板
│   ├── index.html        # 首页
│   ├── analytics.html    # 分析页面
│   └── schedule.html     # 导入管理页面
├── static/               # 静态资源
│   ├── css/             # 样式文件
│   └── js/              # JavaScript文件
├── data/                # Excel数据文件目录
├── logs/                # 日志文件目录
├── requirements.txt     # Python依赖
├── Dockerfile          # Docker镜像构建文件
├── docker-compose.yml  # Docker编排配置
└── start.sh           # 启动脚本
```

## 📋 安装和部署

### 方式一：本地开发环境

1. **克隆项目**
```bash
cd /root/titans_github/phoebe/quant-trading-system
```

2. **创建虚拟环境**
```bash
python3 -m venv env
source env/bin/activate  # Linux/Mac
# 或
env\Scripts\activate  # Windows
```

3. **安装依赖**
```bash
pip install -r requirements.txt
```

4. **配置数据库**
确保MySQL服务运行，并更新`../config.py`中的数据库配置：
```python
DATABASE_CONFIG = {
    'host': 'localhost',
    'port': 3307,
    'user': 'root',
    'password': 'MySecureRootPassword123!',
    'database': 'ti_trading_performance',
    'charset': 'utf8mb4'
}
```

5. **启动应用**
```bash
chmod +x start.sh
./start.sh
```

或手动启动：
```bash
export FLASK_APP=src/app.py
export PYTHONPATH=$(pwd)
python src/app.py
```

6. **访问系统**
打开浏览器访问: `http://localhost:5000`

### 方式二：Docker部署

1. **构建和启动**
```bash
docker-compose up -d
```

2. **查看日志**
```bash
docker-compose logs -f web
```

3. **停止服务**
```bash
docker-compose down
```

## 📁 数据文件格式

系统支持华鑫证券标准格式的Excel文件，文件命名规范：
```
华鑫_314000045768_业绩估算_2025-MM-DD.xlsx
```

Excel文件应包含以下业务部分：
- 日内底仓T0交易明细
- ETF日内申赎交易明细  
- 隔夜底仓交易明细
- ETF赎回涨停票交易明细

## 🔧 使用指南

### 1. 数据导入
- 将Excel文件放入`data/`目录
- 访问"数据导入"页面
- 点击"导入所有未处理数据"或选择特定日期导入
- 查看导入历史和状态

### 2. 查看交易概览
- 首页显示今日交易汇总
- 查看各业务类型收益分布
- 监控当前持仓情况

### 3. 绩效分析
- 访问"绩效分析"页面
- 选择分析时间范围
- 查看详细的绩效指标和图表
- 下载分析报告

### 4. 定时任务
系统包含以下自动任务：
- 每日8:30和18:00自动导入数据
- 每周一9:00生成周报
- 每小时执行系统健康检查
- 每月清理过期日志

## ⚠️ 注意事项

1. **数据安全**: 确保数据库访问权限配置正确
2. **文件权限**: 确保应用有读取data目录的权限
3. **内存使用**: 大量数据导入时注意内存使用情况
4. **备份策略**: 定期备份数据库和重要配置文件

## 🔍 故障排除

### 常见问题

1. **数据库连接失败**
   - 检查MySQL服务是否运行
   - 验证config.py中的数据库配置
   - 确认网络连接和防火墙设置

2. **Excel文件导入失败**
   - 检查文件格式是否正确
   - 确认文件命名规范
   - 查看导入日志获取详细错误信息

3. **Web页面无法访问**
   - 检查Flask应用是否正常启动
   - 确认端口5000未被占用
   - 查看应用日志

### 日志位置
- 应用日志: `logs/app.log`
- 导入日志: `logs/import.log`
- 调度器日志: `logs/scheduler.log`

## 📞 技术支持

如有问题或建议，请联系开发团队或提交Issue。

## 📄 许可证

本项目仅供华鑫证券内部使用。
