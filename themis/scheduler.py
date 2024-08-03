# scheduler.py

import schedule
import time
from tasks.download_rq_index import main as download_rq_index

# 安排任务在特定时间运行
schedule.every().day.at("11:45").do(download_rq_index)

# 不断运行调度器
while True:
    schedule.run_pending()
    time.sleep(1)
