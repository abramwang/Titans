#!/bin/bash

# 量化交易系统启动脚本

echo "正在启动量化交易系统..."

# 检查Python环境
if ! command -v python3 &> /dev/null; then
    echo "错误: 未找到Python3，请先安装Python3"
    exit 1
fi

# 检查是否在虚拟环境中
if [[ "$VIRTUAL_ENV" != "" ]]; then
    echo "已在虚拟环境中: $VIRTUAL_ENV"
else
    echo "建议在虚拟环境中运行此系统"
fi

# 安装依赖
echo "正在安装Python依赖..."
pip install -r requirements.txt

# 检查数据目录
DATA_DIR="../data"
if [ ! -d "$DATA_DIR" ]; then
    echo "警告: 数据目录 $DATA_DIR 不存在，正在创建..."
    mkdir -p "$DATA_DIR"
fi

# 检查日志目录
LOG_DIR="../logs"
if [ ! -d "$LOG_DIR" ]; then
    echo "正在创建日志目录..."
    mkdir -p "$LOG_DIR"
fi

# 设置环境变量
export FLASK_APP=src/app.py
export FLASK_ENV=development
export PYTHONPATH=$(pwd)

echo "环境准备完成，正在启动Web服务器..."
echo "Web界面将在 http://localhost:5000 启动"
echo "按 Ctrl+C 停止服务器"

# 启动Flask应用
python src/app.py
