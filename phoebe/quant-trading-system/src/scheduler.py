#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import schedule
import time
import logging
from datetime import datetime, timedelta
import sys
import os

# 添加项目根目录到Python路径
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from services.data_importer import DataImporter
from services.trading_analytics import TradingAnalytics
import smtplib
from email.mime.text import MimeText
from email.mime.multipart import MimeMultipart

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('/app/logs/scheduler.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

class TaskScheduler:
    """任务调度器"""
    
    def __init__(self):
        self.data_importer = DataImporter()
        self.trading_analytics = TradingAnalytics()
        
    def daily_import_task(self):
        """每日数据导入任务"""
        logger.info("开始执行每日数据导入任务")
        
        try:
            # 导入今日数据
            result = self.data_importer.import_daily_data()
            
            if result:
                logger.info(f"每日数据导入完成，处理了 {len(result)} 个文件")
                
                # 发送成功通知
                self.send_notification(
                    subject="每日数据导入成功",
                    message=f"今日数据导入任务已完成，共处理 {len(result)} 个文件。"
                )
            else:
                logger.info("今日没有新的数据文件需要导入")
                
        except Exception as e:
            logger.error(f"每日数据导入任务失败: {e}")
            
            # 发送失败通知
            self.send_notification(
                subject="每日数据导入失败",
                message=f"今日数据导入任务失败，错误信息: {str(e)}",
                is_error=True
            )
    
    def weekly_report_task(self):
        """每周报告生成任务"""
        logger.info("开始生成每周报告")
        
        try:
            # 计算上周日期范围
            today = datetime.now().date()
            last_monday = today - timedelta(days=today.weekday() + 7)
            last_sunday = last_monday + timedelta(days=6)
            
            # 生成周报
            report = self.trading_analytics.generate_pnl_report(last_monday, last_sunday)
            metrics = self.trading_analytics.calculate_performance_metrics(last_monday, last_sunday)
            
            # 发送周报邮件
            self.send_weekly_report(report, metrics, last_monday, last_sunday)
            
            logger.info("每周报告生成完成")
            
        except Exception as e:
            logger.error(f"每周报告生成失败: {e}")
    
    def monthly_cleanup_task(self):
        """每月清理任务"""
        logger.info("开始执行每月清理任务")
        
        try:
            # 清理30天前的日志文件
            self.cleanup_old_logs(days=30)
            
            # 数据库优化（如果需要）
            # self.optimize_database()
            
            logger.info("每月清理任务完成")
            
        except Exception as e:
            logger.error(f"每月清理任务失败: {e}")
    
    def health_check_task(self):
        """健康检查任务"""
        try:
            # 检查数据库连接
            session = self.data_importer.db_manager.get_session()
            session.execute("SELECT 1")
            session.close()
            
            # 检查数据目录
            data_path = self.data_importer.data_path
            if not os.path.exists(data_path):
                raise Exception(f"数据目录不存在: {data_path}")
            
            logger.info("系统健康检查通过")
            
        except Exception as e:
            logger.error(f"系统健康检查失败: {e}")
            
            # 发送告警
            self.send_notification(
                subject="系统健康检查失败",
                message=f"系统健康检查失败，请及时处理。错误信息: {str(e)}",
                is_error=True
            )
    
    def cleanup_old_logs(self, days=30):
        """清理旧日志文件"""
        log_dir = "/app/logs"
        if not os.path.exists(log_dir):
            return
        
        cutoff_date = datetime.now() - timedelta(days=days)
        
        for filename in os.listdir(log_dir):
            file_path = os.path.join(log_dir, filename)
            if os.path.isfile(file_path):
                file_mtime = datetime.fromtimestamp(os.path.getmtime(file_path))
                if file_mtime < cutoff_date:
                    os.remove(file_path)
                    logger.info(f"删除旧日志文件: {filename}")
    
    def send_notification(self, subject, message, is_error=False):
        """发送通知邮件"""
        # 这里可以配置邮件服务器信息
        # 或者使用其他通知方式如钉钉、企业微信等
        logger.info(f"通知: {subject} - {message}")
    
    def send_weekly_report(self, report, metrics, start_date, end_date):
        """发送周报邮件"""
        # 生成周报内容
        subject = f"量化交易系统周报 ({start_date} 至 {end_date})"
        
        if metrics.get('returns'):
            returns = metrics['returns']
            message = f"""
            量化交易系统周报
            
            报告期间: {start_date} 至 {end_date}
            
            主要指标:
            - 总收益: ¥{returns.get('total_return', 0):,.2f}
            - 平均日收益: ¥{returns.get('avg_daily_return', 0):,.2f}
            - 夏普比率: {returns.get('sharpe_ratio', 0):.4f}
            - 交易天数: {metrics.get('period', {}).get('trading_days', 0)}
            
            风险指标:
            - 最大回撤: ¥{metrics.get('risk', {}).get('max_drawdown', 0):,.2f}
            - 波动率: {metrics.get('risk', {}).get('volatility', 0):.4f}
            
            胜负情况:
            - 胜率: {metrics.get('win_loss', {}).get('win_rate', 0):.2f}%
            - 盈利天数: {metrics.get('win_loss', {}).get('winning_days', 0)}
            - 亏损天数: {metrics.get('win_loss', {}).get('losing_days', 0)}
            """
        else:
            message = f"""
            量化交易系统周报
            
            报告期间: {start_date} 至 {end_date}
            
            本周期内没有交易数据。
            """
        
        self.send_notification(subject, message)

def main():
    """主函数"""
    logger.info("量化交易系统调度器启动")
    
    scheduler = TaskScheduler()
    
    # 安排任务
    # 每天早上8点30分执行数据导入
    schedule.every().day.at("08:30").do(scheduler.daily_import_task)
    
    # 每天下午6点执行数据导入（补充导入）
    schedule.every().day.at("18:00").do(scheduler.daily_import_task)
    
    # 每周一早上9点生成周报
    schedule.every().monday.at("09:00").do(scheduler.weekly_report_task)
    
    # 每月1号凌晨2点执行清理任务
    schedule.every().month.do(scheduler.monthly_cleanup_task)
    
    # 每小时执行健康检查
    schedule.every().hour.do(scheduler.health_check_task)
    
    logger.info("调度任务已安排，开始运行...")
    
    # 运行调度器
    while True:
        try:
            schedule.run_pending()
            time.sleep(60)  # 每分钟检查一次
        except KeyboardInterrupt:
            logger.info("收到停止信号，调度器正在退出...")
            break
        except Exception as e:
            logger.error(f"调度器运行异常: {e}")
            time.sleep(60)  # 出错后等待1分钟再继续

if __name__ == "__main__":
    main()
