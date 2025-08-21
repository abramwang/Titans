#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
数据验证和对比分析工具
比较项目数据库统计结果与华鑫业绩统计Excel文件的差异
"""

import pandas as pd
import numpy as np
from sqlalchemy import create_engine
from config import DATABASE_CONFIG
from datetime import datetime

class DataValidationComparator:
    """数据验证对比器"""
    
    def __init__(self):
        """初始化对比器"""
        self.setup_database()
        self.load_excel_stats()
        self.load_project_stats()
    
    def setup_database(self):
        """设置数据库连接"""
        try:
            connection_string = f"mysql+pymysql://{DATABASE_CONFIG['user']}:{DATABASE_CONFIG['password']}@{DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}/{DATABASE_CONFIG['database']}?charset={DATABASE_CONFIG['charset']}"
            self.engine = create_engine(connection_string)
            print("✓ 数据库连接成功")
        except Exception as e:
            print(f"✗ 数据库连接失败: {e}")
            raise
    
    def load_excel_stats(self):
        """加载Excel统计文件数据"""
        try:
            self.excel_df = pd.read_excel('./data/华鑫_314000045768_业绩统计.xlsx')
            self.excel_df['日期'] = pd.to_datetime(self.excel_df['日期'])
            
            # 筛选出项目数据时间范围
            start_date = '2025-07-22'
            end_date = '2025-08-08'
            self.excel_filtered = self.excel_df[
                (self.excel_df['日期'] >= start_date) & 
                (self.excel_df['日期'] <= end_date)
            ].copy()
            
            print("✓ Excel统计文件加载成功")
        except Exception as e:
            print(f"✗ Excel统计文件加载失败: {e}")
            raise
    
    def load_project_stats(self):
        """加载项目数据库统计"""
        try:
            self.project_df = pd.read_sql("""
                SELECT 
                    trade_date,
                    COUNT(*) as record_count,
                    SUM(profit) as daily_profit,
                    SUM(CASE WHEN section_type = '隔夜底仓' THEN profit ELSE 0 END) as overnight_profit,
                    SUM(CASE WHEN section_type = 'ETF赎回涨停票' THEN profit ELSE 0 END) as etf_redemption_profit
                FROM daily_trading_details
                WHERE section_type IS NOT NULL
                GROUP BY trade_date
                ORDER BY trade_date
            """, self.engine)
            
            self.project_df['trade_date'] = pd.to_datetime(self.project_df['trade_date'])
            print("✓ 项目数据库统计加载成功")
        except Exception as e:
            print(f"✗ 项目数据库统计加载失败: {e}")
            raise
    
    def generate_comparison_report(self):
        """生成对比分析报告"""
        print("="*100)
        print("华鑫证券业绩数据验证对比分析报告")
        print(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("="*100)
        
        # 1. 数据源概览
        self.analyze_data_sources()
        
        # 2. 日度数据对比
        self.compare_daily_data()
        
        # 3. 汇总数据对比
        self.compare_summary_data()
        
        # 4. 差异分析
        self.analyze_differences()
        
        # 5. 数据质量评估
        self.assess_data_quality()
        
        # 6. 结论和建议
        self.generate_conclusions()
    
    def analyze_data_sources(self):
        """分析数据源"""
        print("\n📋 1. 数据源分析")
        print("-" * 50)
        
        print("Excel统计文件:")
        print(f"  时间范围: {self.excel_df['日期'].min().strftime('%Y-%m-%d')} 到 {self.excel_df['日期'].max().strftime('%Y-%m-%d')}")
        print(f"  总记录数: {len(self.excel_df)}")
        print(f"  对比时间段记录数: {len(self.excel_filtered)}")
        
        print(f"\n项目数据库:")
        print(f"  时间范围: {self.project_df['trade_date'].min().strftime('%Y-%m-%d')} 到 {self.project_df['trade_date'].max().strftime('%Y-%m-%d')}")
        print(f"  交易天数: {len(self.project_df)}")
        
        # 检查时间范围匹配度
        excel_dates = set(self.excel_filtered['日期'].dt.date)
        project_dates = set(self.project_df['trade_date'].dt.date)
        
        common_dates = excel_dates & project_dates
        excel_only = excel_dates - project_dates
        project_only = project_dates - excel_dates
        
        print(f"\n📅 时间范围对比:")
        print(f"  共同日期: {len(common_dates)} 天")
        print(f"  仅Excel有: {len(excel_only)} 天 {list(excel_only) if excel_only else ''}")
        print(f"  仅项目有: {len(project_only)} 天 {list(project_only) if project_only else ''}")
    
    def compare_daily_data(self):
        """对比日度数据"""
        print("\n📊 2. 日度数据对比")
        print("-" * 50)
        
        # 合并数据进行对比
        excel_daily = self.excel_filtered[['日期', '预估盈利']].copy()
        excel_daily.columns = ['date', 'excel_profit']
        excel_daily['date'] = excel_daily['date'].dt.date
        
        project_daily = self.project_df[['trade_date', 'daily_profit']].copy()
        project_daily.columns = ['date', 'project_profit']
        project_daily['date'] = project_daily['date'].dt.date
        
        # 合并数据
        comparison = pd.merge(excel_daily, project_daily, on='date', how='outer')
        comparison = comparison.fillna(0)
        comparison['difference'] = comparison['project_profit'] - comparison['excel_profit']
        comparison['abs_difference'] = abs(comparison['difference'])
        comparison['relative_diff_pct'] = np.where(
            comparison['excel_profit'] != 0,
            (comparison['difference'] / comparison['excel_profit']) * 100,
            np.inf
        )
        
        print("日期".ljust(12), "Excel预估".rjust(15), "项目统计".rjust(15), "差异".rjust(15), "相对差异%".rjust(12))
        print("-" * 75)
        
        total_excel = 0
        total_project = 0
        total_abs_diff = 0
        
        for idx, row in comparison.iterrows():
            if pd.notna(row['date']):
                total_excel += row['excel_profit']
                total_project += row['project_profit']
                total_abs_diff += row['abs_difference']
                
                rel_diff = row['relative_diff_pct'] if not np.isinf(row['relative_diff_pct']) else 'N/A'
                rel_diff_str = f"{rel_diff:.1f}" if rel_diff != 'N/A' else 'N/A'
                
                print(f"{row['date']}  {row['excel_profit']:>15,.0f}  {row['project_profit']:>15,.0f}  {row['difference']:>15,.0f}  {rel_diff_str:>12}")
        
        print("-" * 75)
        print(f"{'总计':12}  {total_excel:>15,.0f}  {total_project:>15,.0f}  {total_project-total_excel:>15,.0f}")
        
        # 存储对比数据供后续分析
        self.comparison_df = comparison
        self.total_excel_profit = total_excel
        self.total_project_profit = total_project
        self.total_abs_difference = total_abs_diff
    
    def compare_summary_data(self):
        """对比汇总数据"""
        print("\n📈 3. 汇总数据对比")
        print("-" * 50)
        
        excel_summary = {
            '数据源': 'Excel统计文件',
            '时间段总利润': f"{self.total_excel_profit:,.0f}",
            '平均日利润': f"{self.total_excel_profit/len(self.excel_filtered):,.0f}",
            '数据天数': len(self.excel_filtered)
        }
        
        project_summary = {
            '数据源': '项目数据库',
            '时间段总利润': f"{self.total_project_profit:,.0f}",
            '平均日利润': f"{self.total_project_profit/len(self.project_df):,.0f}",
            '数据天数': len(self.project_df)
        }
        
        print("指标".ljust(20), "Excel统计".rjust(20), "项目数据库".rjust(20))
        print("-" * 65)
        print(f"{'时间段总利润':20}  {excel_summary['时间段总利润']:>20}  {project_summary['时间段总利润']:>20}")
        print(f"{'平均日利润':20}  {excel_summary['平均日利润']:>20}  {project_summary['平均日利润']:>20}")
        print(f"{'数据天数':20}  {excel_summary['数据天数']:>20}  {project_summary['数据天数']:>20}")
    
    def analyze_differences(self):
        """分析差异"""
        print("\n🔍 4. 差异分析")
        print("-" * 50)
        
        # 计算差异统计
        total_difference = self.total_project_profit - self.total_excel_profit
        relative_difference = (total_difference / self.total_excel_profit) * 100 if self.total_excel_profit != 0 else 0
        
        print(f"总差异: {total_difference:,.0f}")
        print(f"相对差异: {relative_difference:.2f}%")
        print(f"绝对差异总和: {self.total_abs_difference:,.0f}")
        
        # 分析差异原因
        print(f"\n🧐 可能的差异原因:")
        
        if abs(relative_difference) > 1000:  # 差异巨大
            print("  ⚠️  数据差异极大，可能存在以下原因:")
            print("     1. 数据源定义不同 - Excel记录净利润，项目记录总收益")
            print("     2. 计算方法差异 - Excel可能包含费用扣除")
            print("     3. 数据范围不同 - Excel可能包含更多业务类型")
            print("     4. 时间点差异 - 可能存在T+1结算差异")
        elif abs(relative_difference) > 10:
            print("  ⚠️  数据差异较大，建议检查:")
            print("     1. 业务分类是否完整")
            print("     2. 计算逻辑是否一致")
            print("     3. 数据提取范围是否匹配")
        else:
            print("  ✅ 数据差异在合理范围内")
        
        # 分析单日最大差异
        max_diff_day = self.comparison_df.loc[self.comparison_df['abs_difference'].idxmax()]
        print(f"\n📅 最大差异日: {max_diff_day['date']}")
        print(f"     Excel: {max_diff_day['excel_profit']:,.0f}")
        print(f"     项目: {max_diff_day['project_profit']:,.0f}")
        print(f"     差异: {max_diff_day['difference']:,.0f}")
    
    def assess_data_quality(self):
        """评估数据质量"""
        print("\n✅ 5. 数据质量评估")
        print("-" * 50)
        
        # 时间完整性
        excel_dates = set(self.excel_filtered['日期'].dt.date)
        project_dates = set(self.project_df['trade_date'].dt.date)
        coverage = len(excel_dates & project_dates) / len(excel_dates) * 100
        
        print(f"时间覆盖率: {coverage:.1f}%")
        
        # 数据一致性
        consistent_days = len(self.comparison_df[self.comparison_df['abs_difference'] < 1000])
        consistency = consistent_days / len(self.comparison_df) * 100
        
        print(f"数据一致性: {consistency:.1f}% (差异<1000的天数占比)")
        
        # 数据完整性
        non_zero_excel = len(self.excel_filtered[self.excel_filtered['预估盈利'] != 0])
        non_zero_project = len(self.project_df[self.project_df['daily_profit'] != 0])
        
        print(f"Excel非零数据天数: {non_zero_excel}/{len(self.excel_filtered)}")
        print(f"项目非零数据天数: {non_zero_project}/{len(self.project_df)}")
    
    def generate_conclusions(self):
        """生成结论和建议"""
        print("\n💡 6. 结论和建议")
        print("-" * 50)
        
        total_difference = self.total_project_profit - self.total_excel_profit
        relative_difference = abs(total_difference / self.total_excel_profit) * 100 if self.total_excel_profit != 0 else 0
        
        print("🎯 主要发现:")
        
        if relative_difference > 1000:
            print("  • 项目数据与Excel统计存在显著差异")
            print("  • 项目数据显示的利润规模远大于Excel统计")
            print("  • 这表明两个数据源的业务范围或计算方法不同")
        
        print(f"  • Excel统计显示预估盈利总计: {self.total_excel_profit:,.0f}")
        print(f"  • 项目数据显示利润总计: {self.total_project_profit:,.0f}")
        print(f"  • 差异倍数: {self.total_project_profit/self.total_excel_profit:.1f}x" if self.total_excel_profit != 0 else "")
        
        print(f"\n📋 建议:")
        print("  1. 核实Excel统计与项目数据的业务范围定义")
        print("  2. 确认Excel中'预估盈利'的具体计算方法")
        print("  3. 检查项目数据是否包含了所有相关费用扣除")
        print("  4. 考虑Excel数据可能是净利润，项目数据是总收益")
        print("  5. 建议与业务部门确认正确的数据口径")
        
        print(f"\n🔍 数据质量总评:")
        if relative_difference < 10:
            print("  ✅ 优秀 - 数据高度一致")
        elif relative_difference < 50:
            print("  ⚠️  良好 - 存在可解释的差异")
        else:
            print("  ❌ 需要关注 - 差异较大，需深入调查")

def main():
    """主函数"""
    try:
        comparator = DataValidationComparator()
        comparator.generate_comparison_report()
    except Exception as e:
        print(f"分析过程中出错: {e}")
        raise

if __name__ == "__main__":
    main()
