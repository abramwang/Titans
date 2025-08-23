#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试数据导入功能
"""

import sys
import os

# 添加项目路径
project_root = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, project_root)
sys.path.insert(0, os.path.join(project_root, 'src'))

from src.services.data_importer import DataImporter

def test_import():
    """测试导入一个Excel文件"""
    
    print("开始导入2025-07-22的数据")
    
    # 创建数据导入器
    importer = DataImporter()
    
    # 测试glob模式
    import glob
    pattern1 = "/root/titans_github/phoebe/data/华鑫_*_业绩估算_*.xlsx"
    pattern2 = os.path.join(importer.data_path, "华鑫_*_业绩估算_*.xlsx")
    
    files1 = glob.glob(pattern1)
    files2 = glob.glob(pattern2)
    
    print(f"模式1 ({pattern1}) 找到: {len(files1)} 个文件")
    print(f"模式2 ({pattern2}) 找到: {len(files2)} 个文件")
    print(f"data_path: {importer.data_path}")
    
    # 检查是否有2025-07-22的文件
    target_files = [f for f in files2 if "2025-07-22" in f]
    print(f"2025-07-22的文件: {target_files}")
    
    # 直接调用_get_files_to_import来查看返回什么
    files_to_import = importer._get_files_to_import("2025-07-22")
    print(f"待导入文件: {files_to_import}")
    
    try:
        # 导入数据 - 传入日期字符串
        result = importer.import_daily_data("2025-07-22")
        print(f"导入结果: {result}")
        if result:
            print("数据导入成功！")
        else:
            print("没有导入任何数据（可能文件已存在或不存在）")
    except Exception as e:
        print(f"导入失败: {str(e)}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    test_import()
