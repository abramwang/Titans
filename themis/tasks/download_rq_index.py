# tasks/task1.py
import glog as log

# 配置日志记录
log.setLevel("INFO")
log.handler.setStream(open('scheduler.log', 'a'))

def main():
    print("Task 1 executed")
    try:
        log.info("Task 1 is executing")
        # 你的任务代码
        log.info("Task 1 finished execution")
    except Exception as e:
        log.error(f"Error in Task 1: {e}")