#!/usr/bin/env python
# coding=utf-8

__version__ = '0.1.1'
__all__ = [
    'SetRootPath', 'GetDayBar', 'GetBar', 'GetMarket', 'GetBaseData'
]
__author__ = 'bo wang <bo.wang@sci-inv.cn>'


__history_root__ = "http://192.168.3.100/data"
__history_root_type__ = "URL"


# 设置数据存储的根目录
def SetRootPath(root):
    global __history_root__, __history_root_type__
    __history_root__ = root
    from pymoneta.data.tools import check_string
    __history_root_type__ = check_string(__history_root__)

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
        if "symbol" not in columns_:
            columns_.append("symbol")
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

#获取基础数据
def GetBaseData(symbols_, data_name):
    global __history_root__, __history_root_type__
    if not __history_root__:
        return -1
    import os
    import pandas as pd
    from pymoneta.data.tools import get_file_list_in_path
   
    file_list = get_file_list_in_path(f"{__history_root__}/ti_setting_data")

    data_file = f"ti.{data_name}.parquet"
    if data_file not in file_list:
        return -1
    file_path = f"{__history_root__}/ti_setting_data/{data_file}"

    df = pd.read_parquet(file_path)

    symbols_list = []
    if type(symbols_).__name__ == "str":
        if symbols_ == "*":
            symbols_list = []
        else:
            symbols_list = [symbols_]
    elif type(symbols_).__name__ == "list":
        symbols_list = symbols_

    if "symbol" in df.index.names:
        return df[df.index.get_level_values("symbol").isin(symbols_list)]
    else:
        return df
