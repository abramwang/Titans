#!/usr/bin/env python
# coding=utf-8

__version__ = '0.1.0'
__all__ = [
    'GetDayBar', 'GetBar', 'GetMarket'
]
__author__ = 'bo wang <bo.wang@sci-inv.cn>'


__history_root__ = "http://172.17.0.1"

"""
通过 按市场和日期存储的数据获取数据
"""

def GetDayBar(asset_type_:str, symbols_, start_date_, end_date_):
    global __history_root__
    if not __history_root__:
        return -1
    import os
    import pandas as pd

    _start_date = pd.to_datetime(start_date_)
    _end_date = pd.to_datetime(end_date_)

    
    file_path = f"{__history_root__}/ti_bar_data/ti.1d.{asset_type_}.parquet"
    
    symbols_list = []
    if type(symbols_).__name__ == "str":
        if symbols_ == "*":
            symbols_list = []
        else:
            symbols_list = [symbols_]
    elif type(symbols_).__name__ == "list":
        symbols_list = symbols_

    df = pd.read_parquet(file_path)
    if symbols_list:
        df = df[df.index.get_level_values("symbol").isin(symbols_list)]

    df = df[df.index.get_level_values("date") >= _start_date]
    df = df[df.index.get_level_values("date") <= _end_date]
    df.sort_index(inplace=True)
    
    return df

def GetBar(date_, exchange_:str, symbols_, period_ = "1m"):
    global __history_root__
    if not __history_root__:
        return -1
    import os
    import pandas as pd

    _date = 0
    if type(date_).__name__ == "datetime":
        _date = date_.strftime('%Y%m%d')
    else:
        _date = date_
    
    file_path = f"{__history_root__}/ti_bar_data/ti.{period_}.{exchange_}.{_date}.parquet"
    
    symbols_list = []
    if type(symbols_).__name__ == "str":
        if symbols_ == "*":
            symbols_list = []
        else:
            symbols_list = [symbols_]
    elif type(symbols_).__name__ == "list":
        symbols_list = symbols_

    df = pd.read_parquet(file_path)
    if symbols_list:
        df = df[df["symbol"].isin(symbols_list)]
    return df

def GetMarket(date_, exchange_:str, symbols_, dateType_:str, columns_:list = [] ):
    global __history_root__
    if not __history_root__:
        return -1
    import os
    import pandas as pd

    _date = 0
    if type(date_).__name__ == "datetime":
        _date = date_.strftime('%Y%m%d')
    else:
        _date = date_
    
    file_path = f"{__history_root__}/ti_market_data/ti.{dateType_}.{exchange_}.{_date}.parquet"
    
    symbols_list = []
    if type(symbols_).__name__ == "str":
        if symbols_ == "*":
            symbols_list = []
        else:
            symbols_list = [symbols_]
    elif type(symbols_).__name__ == "list":
        symbols_list = symbols_

    df = pd.DataFrame()
    if not columns_:
        df = pd.read_parquet(file_path)
    else:
        df = pd.read_parquet(file_path, columns=columns_)
    if symbols_list:
        df = df[df["symbol"].isin(symbols_list)]
    return df


"""
通过按市场和标的的方式获取数据
"""

def GetBarBySymbol(exchange_:str, symbol_:str, period_:str, start_time_, end_time_):
    global __history_root__
    if not __history_root__:
        return -1
    import os
    import pandas as pd

    start_time = start_time_
    end_time = end_time_

    if type(start_time).__name__ != "datetime":
        start_time = pd.to_datetime(start_time)
    if type(end_time).__name__ != "datetime":
        end_time = pd.to_datetime(end_time)
    
    date_range = pd.date_range(start_time, end_time)
    
    file_list = []

    year_group = date_range.groupby(date_range.year)
    for year in year_group:
        file_path = f"{__history_root__}/ti_bar_data_by_symbol/ti.{period_}.{exchange_}.{year}.{symbol_}.parquet"
        #if not os.path.exists(file_path):
        #    continue
        file_list.append(file_path)

    if not file_list:
        return -1
    
    df_list = []
    for file in file_list:
        df = pd.read_parquet(file)
        df_list.append(df)
    
    df = pd.concat(df_list)
    df = df[df["timestamp"] >= start_time]
    df = df[df["timestamp"] < end_time]

    return df