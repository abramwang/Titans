import pandas as pd
import os

pd.options.display.max_rows = None
#pd.options.display.max_columns = None

def fix_char_type(_src):
    if _src == '\x00':
        return '0'
    if _src == 0:
        return '0'
    if _src == None:
        return '0'
    return _src

def fix_sz_order(path):
    df = pd.read_parquet(path)
    df['order_type'] = df['order_type'].apply(fix_char_type)
    print(path, df['order_type'].unique())
    df.to_parquet(path, compression='gzip')

def fix_matches(path):
    df = pd.read_parquet(path)
    df['function_code'] = df['function_code'].apply(fix_char_type)
    df['trade_type'] = df['trade_type'].apply(fix_char_type)
    df['bs_flag'] = df['bs_flag'].apply(fix_char_type)
    print(path)
    df.to_parquet(path, compression='gzip')

root = "/opt/ti_data/ti_market_data/"
for file in os.listdir(root):
    if "order.SZ" in file:
        fix_sz_order("{}{}".format(root, file))
    if "matches" in file:
        fix_matches("{}{}".format(root, file))
