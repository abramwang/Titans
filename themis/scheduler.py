# scheduler.py

import schedule
import time
import glog as log
import sys
from tasks.download_rq_index import main as download_rq_index


# 配置日志记录
log.setLevel("INFO")
log.handler.setStream(open('scheduler.log', 'a'))

def log_task(task, task_name):
    try:
        log.info(f"{task_name} started")
        task()
        log.info(f"{task_name} completed successfully")
    except Exception as e:
        log.error(f"Error in {task_name}: {e}")

# 安排任务在特定时间运行

schedule.every().day.at("15:42").do(log_task, download_rq_index, "Task download_rq_index")

# 不断运行调度器
while True:
    schedule.run_pending()
    time.sleep(1)
