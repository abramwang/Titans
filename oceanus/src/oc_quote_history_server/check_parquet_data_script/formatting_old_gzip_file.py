import pandas as pd

def fix_char_type(_src):
    if _src == '\x00':
        return '0'
    if _src == 0:
        return '0'
    if _src == None:
        return '0'
    return _src

# 修改 symbol 格式, 增加 exchange 列，并且按照seq排序, 并且重建索引
def formatting_order(base_path, ex, day, dis_path):
    #读取文件
    _src_path = "{}orders.{}.{}.parquet.gzip".format(base_path, ex, day)
    df = pd.read_parquet(_src_path)
    print(df.head())
    #重新排序
    df = df.sort_values(by= ['seq', "snap_time"],ascending=True)
    print(df.head())
    #调整字段
    df['symbol'] = df['symbol'].str.slice(2,8)
    df['exchange'] = ex
    df['date'] = pd.to_numeric(df['date'].str.replace("-",""))
    df['time'] = pd.to_numeric(df['time'].str.replace(":","").str.replace(".",""))
    df.rename(columns={'snap_time': 'timestamp'}, inplace=True)
    df['order_type'] = df['order_type'].apply(fix_char_type)

    df = df.reset_index(drop=True)
    
    print(df.head())
    print(df.dtypes)
    df.to_parquet('{}ti.order.{}.{}.parquet'.format(dis_path, ex, day), compression='gzip')
    pass

# 修改 symbol 格式, 增加 exchange 列，并且按照seq排序, 并且重建索引
def formatting_matches(base_path, ex, day, dis_path):
    _src_path = "{}matches.{}.{}.parquet.gzip".format(base_path, ex, day)
    df = pd.read_parquet(_src_path)
    print(df.head())
    #重新排序
    df = df.sort_values(by= ['seq', "snap_time"],ascending=True)
    print(df.head())
    #调整字段
    df['symbol'] = df['symbol'].str.slice(2,8)
    df['exchange'] = ex
    df['date'] = pd.to_numeric(df['date'].str.replace("-",""))
    df['time'] = pd.to_numeric(df['time'].str.replace(":","").str.replace(".",""))
    df.rename(columns={'snap_time': 'timestamp'}, inplace=True)
    df['function_code'] = df['function_code'].apply(fix_char_type)
    df['trade_type'] = df['trade_type'].apply(fix_char_type)
    df['bs_flag'] = df['bs_flag'].apply(fix_char_type)

    df = df.reset_index(drop=True)
    
    print(df.head())
    print(df.dtypes)
    df.to_parquet('{}ti.matches.{}.{}.parquet'.format(dis_path, ex, day), compression='gzip')
    pass

def formatting_snapshot(base_path, ex, day, dis_path):
    _src_path = "{}snapshot.{}.{}.parquet.gzip".format(base_path, ex, day)
    df = pd.read_parquet(_src_path)
    print(df.dtypes)
    print(df.head())
    df = df.sort_values(by= ["snap_time"],ascending=True)
    print(df.head())
    #调整字段
    df['symbol'] = df['symbol'].str.slice(2,8)
    df['exchange'] = ex
    df['date'] = pd.to_numeric(df['date'].str.replace("-",""))
    df['time'] = pd.to_numeric(df['time'].str.replace(":","").str.replace(".",""))
    df.rename(columns={'snap_time': 'timestamp'}, inplace=True)

    df = df.reset_index(drop=True)
    
    print(df.head())
    print(df.dtypes)
    df.to_parquet('{}ti.snapshot.{}.{}.parquet'.format(dis_path, ex, day), compression='gzip')
    pass

for i in range(18, 20):
    for ex in ["SH","SZ"]:
        formatting_snapshot("/opt/ti_data/market_data/2021/", ex, "202110{}".format(i), "/opt/ti_data/ti_market_data/")
        #formatting_order("/opt/ti_data/market_data/2021/", ex, "202110{}".format(i), "/opt/ti_data/ti_market_data/")
        #formatting_matches("/opt/ti_data/market_data/2021/", ex, "202110{}".format(i), "/opt/ti_data/ti_market_data/")