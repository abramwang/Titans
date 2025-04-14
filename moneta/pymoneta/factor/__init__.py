from influxdb_client import InfluxDBClient, Point, WriteOptions, Task, TaskStatusType
from influxdb_client.client.write_api import SYNCHRONOUS
from influxdb_client.client.exceptions import InfluxDBError
import pandas as pd
from typing import List, Dict, Optional
from datetime import timedelta

class FactorDB:
    def __init__(self, url: str, token: str, org: str, bucket: str):
        """
        初始化因子数据库连接
        :param url: InfluxDB地址，例如 http://localhost:8086
        :param token: 认证token
        :param org: 组织名称
        :param bucket: 存储桶名称
        """
        self.client = InfluxDBClient(url=url, token=token, org=org)
        self.write_api = self.client.write_api(write_options=WriteOptions(
            batch_size=5000,
            flush_interval=10_000,
            retry_interval=5,
            max_retries=3,
            max_retry_delay=15,
            exponential_base=2
        ))
        self.query_api = self.client.query_api()
        self.org = org
        self.bucket = bucket

    def write_factor_point(self, 
                          frequency: str,
                          factor_type: str,
                          name: str,
                          symbol: str,
                          value: float,
                          market: str = "UNKNOWN",
                          source: str = "DEFAULT",
                          timestamp: Optional[int] = None,
                          **kwargs):
        """
        写入单因子数据点
        :param frequency: 数据频率（'1m','1d'等）
        :param factor_type: 因子类型（'momentum','value'等）
        :param name: 因子名称（'alpha101'等）
        :param symbol: 证券代码
        :param value: 因子数值（必须字段）
        :param market: 交易市场
        :param source: 数据源
        :param timestamp: 纳秒级时间戳（可选）
        :param kwargs: 其他字段（z_score, percentile等）
        """
        measurement = f"factor_{frequency}"
        
        point = Point(measurement)\
            .tag("factor_type", factor_type)\
            .tag("name", name)\
            .tag("symbol", symbol)\
            .tag("market", market)\
            .tag("source", source)\
            .field("value", value)
        
        for k, v in kwargs.items():
            point = point.field(k, v)
            
        if timestamp:
            point = point.time(timestamp)
            
        try:
            self.write_api.write(bucket=self.bucket, record=point)
        except InfluxDBError as e:
            print(f"写入失败: {e}")

    def write_batch_factors(self, data_points: List[Dict]):
        """
        批量写入因子数据（高性能写入）
        :param data_points: 数据点列表，每个元素为字典格式数据
        示例数据点：
        {
            "frequency": "1m",
            "factor_type": "momentum",
            "name": "alpha101",
            "symbol": "AAPL",
            "value": 2.34,
            "market": "NASDAQ",
            "source": "alpha_vantage",
            "z_score": 0.87,
            "timestamp": 1693553400000000000
        }
        """
        points = []
        for dp in data_points:
            measurement = f"factor_{dp['frequency']}"
            point = Point(measurement)\
                .tag("factor_type", dp['factor_type'])\
                .tag("name", dp['name'])\
                .tag("symbol", dp['symbol'])\
                .tag("market", dp.get('market', 'UNKNOWN'))\
                .tag("source", dp.get('source', 'DEFAULT'))\
                .field("value", dp['value'])
            
            # 添加可选字段
            for field in ['z_score', 'percentile', 'rank']:
                if field in dp:
                    point = point.field(field, dp[field])
                    
            if 'timestamp' in dp:
                point = point.time(dp['timestamp'])
                
            points.append(point)
        
        try:
            self.write_api.write(bucket=self.bucket, record=points)
        except InfluxDBError as e:
            print(f"批量写入失败: {e}")

    def query_factors(self, 
                     frequency: str,
                     name: str,
                     symbol: str,
                     factor_type: str,
                     start: str,
                     stop: str = "now()",
                     fields: List[str] = ["value"],
                     market: Optional[str] = None) -> pd.DataFrame:
        """
        查询因子数据（返回DataFrame）
        :param frequency: 数据频率
        :param name: 因子名称
        :param symbol: 证券代码
        :param factor_type: 因子类型
        :param start: 开始时间（RFC3339格式）
        :param stop: 结束时间（默认当前时间）
        :param fields: 需要返回的字段列表
        :param market: 过滤市场（可选）
        """
        measurement = f"factor_{frequency}"
        
        query = f'''
        from(bucket: "{self.bucket}")
          |> range(start: {start}, stop: {stop})
          |> filter(fn: (r) => r._measurement == "{measurement}")
          |> filter(fn: (r) => r.name == "{name}")
          |> filter(fn: (r) => r.symbol == "{symbol}")
          |> filter(fn: (r) => r.factor_type == "{factor_type}")
        '''
        
        if market:
            query += f'|> filter(fn: (r) => r.market == "{market}")\n'
            
        query += '|> pivot(\n' \
                '    rowKey:["_time"],\n' \
                '    columnKey: ["_field"],\n' \
               f'    valueColumn: "_value"\n' \
                ')'
        
        try:
            result = self.query_api.query_data_frame(query, org=self.org)
            if not fields:
                return result
            return result[["_time", "symbol"] + fields]
        except Exception as e:
            print(f"查询失败: {e}")
            return pd.DataFrame()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def close(self):
        """释放资源"""
        self.write_api.close()
        self.client.close()


class EnhancedFactorDB(FactorDB):
    def __init__(self, url: str, token: str, org: str, bucket: str):
        super().__init__(url, token, org, bucket)
        self.tasks_api = self.client.tasks_api()
        self.buckets_api = self.client.buckets_api()
    
    def create_resample_task(self, 
                           task_name: str,
                           source_bucket: str,
                           destination_bucket: str,
                           frequency: str,
                           agg_func: str = "mean",
                           every_interval: str = "1h",
                           delay: str = "5m"):
        """
        创建重采样定时任务
        :param task_name: 任务唯一名称
        :param source_bucket: 原始数据存储桶
        :param destination_bucket: 目标存储桶
        :param frequency: 目标频率（1h, 1d等）
        :param agg_func: 聚合函数（mean, median, sum等）
        :param every_interval: 任务执行间隔
        :param delay: 数据延迟等待时间
        """
        flux_script = f'''
        option task = {{
            name: "{task_name}",
            every: {every_interval},
            delay: {delay}
        }}
        
        from(bucket: "{source_bucket}")
            |> range(start: -task.every)
            |> filter(fn: (r) => r._measurement =~ /^factor_/)
            |> aggregateWindow(
                every: {frequency},
                fn: {agg_func},
                createEmpty: false
            )
            |> to(bucket: "{destination_bucket}")
        '''
        
        try:
            task = Task(
                org_id=self.buckets_api.find_bucket_by_name(source_bucket).org_id,
                flux=flux_script,
                description=f"Resample to {frequency}",
                status=TaskStatusType.ACTIVE
            )
            return self.tasks_api.create_task(task)
        except InfluxDBError as e:
            print(f"创建任务失败: {e}")
    
    def query_with_resample(self,
                          frequency: str,
                          name: str,
                          symbol: str,
                          factor_type: str,
                          start: str,
                          stop: str = "now()",
                          fields: List[str] = ["value"],
                          auto_downsample: bool = True) -> pd.DataFrame:
        """
        智能查询（自动选择最优数据源）
        :param auto_downsample: 是否自动使用降采样数据
        """
        # 自动选择数据源逻辑
        if auto_downsample:
            time_range = pd.Timestamp(stop) - pd.Timestamp(start)
            if time_range > timedelta(days=7):
                target_bucket = "factors_1h"
            elif time_range > timedelta(days=30):
                target_bucket = "factors_1d"
            else:
                target_bucket = self.bucket
        else:
            target_bucket = self.bucket
        
        measurement = f"factor_{frequency}"
        
        query = f'''
        from(bucket: "{target_bucket}")
          |> range(start: {start}, stop: {stop})
          |> filter(fn: (r) => r._measurement == "{measurement}")
          |> filter(fn: (r) => r.name == "{name}")
          |> filter(fn: (r) => r.symbol == "{symbol}")
          |> filter(fn: (r) => r.factor_type == "{factor_type}")
          |> pivot(
              rowKey:["_time"],
              columnKey: ["_field"],
              valueColumn: "_value"
          )
        '''
        
        try:
            return self.query_api.query_data_frame(query, org=self.org)
        except Exception as e:
            print(f"查询失败: {e}")
            return pd.DataFrame()
    
    def list_active_tasks(self) -> List[Dict]:
        """获取所有活跃任务"""
        return [
            {
                "id": task.id,
                "name": task.name,
                "status": task.status,
                "interval": task.every
            }
            for task in self.tasks_api.find_tasks()
            if task.status == TaskStatusType.ACTIVE
        ]
    
    def delete_task(self, task_id: str):
        """删除指定任务"""
        try:
            self.tasks_api.delete_task(task_id)
        except InfluxDBError as e:
            print(f"删除任务失败: {e}")

    def monitor_tasks(self) -> pd.DataFrame:
        """监控任务运行状态"""
        query = '''
        import "influxdata/influxdb/tasks"
        tasks.lastSuccess()
        |> map(fn: (r) => ({
            task_id: r.taskID,
            last_run: r._time,
            status: if r.started >= r.finished then "running" else "completed",
            duration: duration(v: uint(v: r.finished) - uint(v: r.started)
        }))
        '''
        return self.query_api.query_data_frame(query, org=self.org)


# 使用示例
if __name__ == "__main__":
    # 初始化连接
    with FactorDB(
        url="http://121.36.210.85:2211",
        token="zeR6zh0QzsuAen5Acu7kPliRaE2QvbD08-W3NYT6r5gXYAcbk-gQJqMpTA1-mO13vvs0UR4O2-kJYU8wgCybTg==",
        org="timely",
        bucket="ti_factor"
    ) as db:
        # 写入单点数据
        db.write_factor_point(
            frequency="1m",
            factor_type="momentum",
            name="alpha_001",
            symbol="AAPL",
            value=2.34,
            market="NASDAQ",
            z_score=0.87,
            percentile=0.92
        )

        # 批量写入
        batch_data = [{
            "frequency": "1d",
            "factor_type": "value",
            "name": "alpha_002",
            "symbol": "MSFT",
            "value": 15.6,
            "market": "NYSE",
            "percentile": 0.78,
            "timestamp": 1693553400000000000
        }]
        db.write_batch_factors(batch_data)

        # 查询数据
        df = db.query_factors(
            frequency="1m",
            name="alpha_001",
            symbol="AAPL",
            factor_type="momentum",
            start="2023-09-01T00:00:00Z",
            fields=["value", "z_score"]
        )
        print(df.head())