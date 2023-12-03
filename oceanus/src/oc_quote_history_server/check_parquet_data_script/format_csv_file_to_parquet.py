import os
import pandas as pd
from datetime import datetime

#pd.options.display.max_columns = None

def fix_char_type(_src):
    if _src == '\x00':
        return '0'
    if _src == 0:
        return '0'
    if _src == None:
        return '0'
    return _src

def trance_datetime(datetime_num):
    _ms = int(datetime_num % 1000)
    _y = int((datetime_num - _ms) / 10000000000000)
    _m = int((datetime_num - _ms) / 100000000000) - _y * 100
    _d = int((datetime_num - _ms) / 1000000000) - _y * 10000 - _m * 100
    _H = int((datetime_num - _ms) / 10000000) - _y * 1000000 - _m * 10000 - _d * 100
    _M = int((datetime_num - _ms) / 100000) - _y * 100000000 - _m * 1000000 - _d * 10000 - _H * 100
    _S = int((datetime_num - _ms) / 1000) - _y * 10000000000 - _m * 100000000 - _d * 1000000 - _H * 10000 - _M * 100
    return datetime(_y, _m, _d, _H, _M, _S, _ms*1000)

def format_order_df(df, exchange):
    df['order_kind'] = df['order_kind'].apply(lambda _: chr(_))
    df['function_code'] = df['function_code'].apply(lambda _: chr(_))
    df['symbol'] = df['wind_code'].str.slice(0, 6, 1) 
    df.rename(columns={'order': 'seq'}, inplace=True)
    df.rename(columns={'order_kind': 'order_type'}, inplace=True)
    df.rename(columns={'order_price': 'price'}, inplace=True)
    df.rename(columns={'order_volume': 'volume'}, inplace=True)
    # 多余 字段  ['wind_code', 'name', 'channel', 'order_orino', 'biz_index']
    df["timestamp"] = df["date"] * 1000000000 + df["time"]
    df['timestamp'] = df['timestamp'].apply(trance_datetime)
    df = df.drop(['wind_code', 'code', 'name', 'biz_index'], axis=1) 
    df['exchange'] = exchange
    df['price'] = df['price'].astype('float') / 10000
    df['order_type'] = df['order_type'].apply(fix_char_type)

    df = df.sort_values(by= ['time', 'seq'],ascending=True)
    df = df.reset_index(drop=True)
    return df

def format_matches_df(df, exchange):
    df['order_kind'] = df['order_kind'].apply(lambda _: chr(_))
    df['function_code'] = df['function_code'].apply(lambda _: chr(_))
    df['bs_flag'] = df['bs_flag'].apply(lambda _: chr(_))
    df['symbol'] = df['wind_code'].str.slice(0, 6, 1) 
    df.rename(columns={'index': 'seq'}, inplace=True)
    df.rename(columns={'order_kind': 'trade_type'}, inplace=True)
    df.rename(columns={'trade_price': 'price'}, inplace=True)
    df.rename(columns={'trade_volume': 'volume'}, inplace=True)
    df.rename(columns={'ask_order': 'ask_order_seq'}, inplace=True)
    df.rename(columns={'bid_order': 'bid_order_seq'}, inplace=True)

    df["timestamp"] = df["date"] * 1000000000 + df["time"]
    df['timestamp'] = df['timestamp'].apply(trance_datetime)
    df = df.drop(['wind_code', "code", 'name', 'biz_index'], axis=1) 
    df['exchange'] = exchange
    df['price'] = df['price'].astype('float') / 10000
    df['function_code'] = df['function_code'].apply(fix_char_type)
    df['trade_type'] = df['trade_type'].apply(fix_char_type)
    df['bs_flag'] = df['bs_flag'].apply(fix_char_type)

    df = df.sort_values(by= ['time', 'seq'],ascending=True)
    df = df.reset_index(drop=True)
    return df

def format_tick_df(df, exchange):
    df['symbol'] = df['wind_code'].str.slice(0, 6, 1) 

    df["timestamp"] = df["date"] * 1000000000 + df["time"]
    df['timestamp'] = df['timestamp'].apply(trance_datetime)

    df.rename(columns={'turover': 'turnover'}, inplace=True)
    df.rename(columns={'accturover': 'acc_turnover'}, inplace=True)
    df.rename(columns={'accvolume': 'acc_volume'}, inplace=True)
    #ask
    df.rename(columns={'asize1'     : 'ask_volume1'}, inplace=True)
    df.rename(columns={'asize2'     : 'ask_volume2'}, inplace=True)
    df.rename(columns={'asize3'     : 'ask_volume3'}, inplace=True)
    df.rename(columns={'asize4'     : 'ask_volume4'}, inplace=True)
    df.rename(columns={'asize5'     : 'ask_volume5'}, inplace=True)
    df.rename(columns={'asize6'     : 'ask_volume6'}, inplace=True)
    df.rename(columns={'asize7'     : 'ask_volume7'}, inplace=True)
    df.rename(columns={'asize8'     : 'ask_volume8'}, inplace=True)
    df.rename(columns={'asize9'     : 'ask_volume9'}, inplace=True)
    df.rename(columns={'asize10'    : 'ask_volume10'}, inplace=True)
    df.rename(columns={'ask1'       : 'ask_price1'}, inplace=True)
    df.rename(columns={'ask2'       : 'ask_price2'}, inplace=True)
    df.rename(columns={'ask3'       : 'ask_price3'}, inplace=True)
    df.rename(columns={'ask4'       : 'ask_price4'}, inplace=True)
    df.rename(columns={'ask5'       : 'ask_price5'}, inplace=True)
    df.rename(columns={'ask6'       : 'ask_price6'}, inplace=True)
    df.rename(columns={'ask7'       : 'ask_price7'}, inplace=True)
    df.rename(columns={'ask8'       : 'ask_price8'}, inplace=True)
    df.rename(columns={'ask9'       : 'ask_price9'}, inplace=True)
    df.rename(columns={'ask10'      : 'ask_price10'}, inplace=True)
    df['ask_price1'] = df['ask_price1'].astype('float') / 10000
    df['ask_price2'] = df['ask_price2'].astype('float') / 10000
    df['ask_price3'] = df['ask_price3'].astype('float') / 10000
    df['ask_price4'] = df['ask_price4'].astype('float') / 10000
    df['ask_price5'] = df['ask_price5'].astype('float') / 10000
    df['ask_price6'] = df['ask_price6'].astype('float') / 10000
    df['ask_price7'] = df['ask_price7'].astype('float') / 10000
    df['ask_price8'] = df['ask_price8'].astype('float') / 10000
    df['ask_price9'] = df['ask_price9'].astype('float') / 10000
    df['ask_price10'] = df['ask_price10'].astype('float') / 10000
    df.rename(columns={'aorder1'    : 'ask_order_num1'}, inplace=True)
    df.rename(columns={'aorder2'    : 'ask_order_num2'}, inplace=True)
    df.rename(columns={'aorder3'    : 'ask_order_num3'}, inplace=True)
    df.rename(columns={'aorder4'    : 'ask_order_num4'}, inplace=True)
    df.rename(columns={'aorder5'    : 'ask_order_num5'}, inplace=True)
    df.rename(columns={'aorder6'    : 'ask_order_num6'}, inplace=True)
    df.rename(columns={'aorder7'    : 'ask_order_num7'}, inplace=True)
    df.rename(columns={'aorder8'    : 'ask_order_num8'}, inplace=True)
    df.rename(columns={'aorder9'    : 'ask_order_num9'}, inplace=True)
    df.rename(columns={'aorder10'   : 'ask_order_num10'}, inplace=True)
    
    df.rename(columns={'total_ask_volume'   : 'total_ask_qty'}, inplace=True)
    df.rename(columns={'ask_av_price'       : 'wavg_ask'}, inplace=True)
    df['wavg_ask'] = df['wavg_ask'].astype('float') / 10000
    #bid
    df.rename(columns={'bsize1'     : 'bid_volume1'}, inplace=True)
    df.rename(columns={'bsize2'     : 'bid_volume2'}, inplace=True)
    df.rename(columns={'bsize3'     : 'bid_volume3'}, inplace=True)
    df.rename(columns={'bsize4'     : 'bid_volume4'}, inplace=True)
    df.rename(columns={'bsize5'     : 'bid_volume5'}, inplace=True)
    df.rename(columns={'bsize6'     : 'bid_volume6'}, inplace=True)
    df.rename(columns={'bsize7'     : 'bid_volume7'}, inplace=True)
    df.rename(columns={'bsize8'     : 'bid_volume8'}, inplace=True)
    df.rename(columns={'bsize9'     : 'bid_volume9'}, inplace=True)
    df.rename(columns={'bsize10'    : 'bid_volume10'}, inplace=True)
    df.rename(columns={'bid1'       : 'bid_price1'}, inplace=True)
    df.rename(columns={'bid2'       : 'bid_price2'}, inplace=True)
    df.rename(columns={'bid3'       : 'bid_price3'}, inplace=True)
    df.rename(columns={'bid4'       : 'bid_price4'}, inplace=True)
    df.rename(columns={'bid5'       : 'bid_price5'}, inplace=True)
    df.rename(columns={'bid6'       : 'bid_price6'}, inplace=True)
    df.rename(columns={'bid7'       : 'bid_price7'}, inplace=True)
    df.rename(columns={'bid8'       : 'bid_price8'}, inplace=True)
    df.rename(columns={'bid9'       : 'bid_price9'}, inplace=True)
    df.rename(columns={'bid10'      : 'bid_price10'}, inplace=True)
    df['bid_price1'] = df['bid_price1'].astype('float') / 10000
    df['bid_price2'] = df['bid_price2'].astype('float') / 10000
    df['bid_price3'] = df['bid_price3'].astype('float') / 10000
    df['bid_price4'] = df['bid_price4'].astype('float') / 10000
    df['bid_price5'] = df['bid_price5'].astype('float') / 10000
    df['bid_price6'] = df['bid_price6'].astype('float') / 10000
    df['bid_price7'] = df['bid_price7'].astype('float') / 10000
    df['bid_price8'] = df['bid_price8'].astype('float') / 10000
    df['bid_price9'] = df['bid_price9'].astype('float') / 10000
    df['bid_price10'] = df['bid_price10'].astype('float') / 10000
    df.rename(columns={'border1'    : 'bid_order_num1'}, inplace=True)
    df.rename(columns={'border2'    : 'bid_order_num2'}, inplace=True)
    df.rename(columns={'border3'    : 'bid_order_num3'}, inplace=True)
    df.rename(columns={'border4'    : 'bid_order_num4'}, inplace=True)
    df.rename(columns={'border5'    : 'bid_order_num5'}, inplace=True)
    df.rename(columns={'border6'    : 'bid_order_num6'}, inplace=True)
    df.rename(columns={'border7'    : 'bid_order_num7'}, inplace=True)
    df.rename(columns={'border8'    : 'bid_order_num8'}, inplace=True)
    df.rename(columns={'border9'    : 'bid_order_num9'}, inplace=True)
    df.rename(columns={'border10'   : 'bid_order_num10'}, inplace=True)
    
    df.rename(columns={'total_bid_volume'   : 'total_bid_qty'}, inplace=True)
    df.rename(columns={'bid_av_price'       : 'wavg_bid'}, inplace=True)
    df['wavg_bid'] = df['wavg_bid'].astype('float') / 10000

    df.rename(columns={'price'   : 'last'}, inplace=True)
    df['last'] = df['last'].astype('float') / 10000
    df['high'] = df['high'].astype('float') / 10000
    df['low'] = df['low'].astype('float') / 10000
    df['open'] = df['open'].astype('float') / 10000
    df['pre_close'] = df['pre_close'].astype('float') / 10000

    df = df.drop([
        'wind_code', 'afterMatchItems', 'afterPrice', 'afterTurnover', 'afterVolume',
        'avgPrice', 'bs_flag', 'code', 'curDelta', 'downs', 'holdLines', 'index', 'name',
        'position', 'prePosition', 'preSettle', 'settle', 'stocks', 'trade_flag', 'ups'
        ], axis=1) 
    df['exchange'] = exchange

    df = df.sort_values(by= ['time', "symbol"],ascending=True)
    df = df.reset_index(drop=True)
    return df

def format_queue_df(df, exchange):
    print(df.head())
    print(df.dtypes)
    return df

def get_dis_parquet_path(dis_path, trading_day, exchange, data_type):
    path = ""
    if data_type == "Order":
        path = '{}ti.{}.{}.{}.parquet'.format(dis_path, "order", exchange, trading_day)
    elif data_type == "Transaction":
        path = '{}ti.{}.{}.{}.parquet'.format(dis_path, "matches", exchange, trading_day)
    elif data_type == "Tick":
        path = '{}ti.{}.{}.{}.parquet'.format(dis_path, "snapshot", exchange, trading_day)
    elif data_type == "OrderQueue":
        path = '{}ti.{}.{}.{}.parquet'.format(dis_path, "queue", exchange, trading_day)
    else:
        pass
    return path

def get_files(root_path = "./", dis_path ="./", code = None, trading_day = None, exchange = None, data_type = None):
    out_file_path = get_dis_parquet_path(dis_path, trading_day, exchange, data_type)
    if os.path.exists(out_file_path):
        print("already has this file:", out_file_path)
        return
    df_list = []
    i = 0
    for filepath, dirnames, filenames in os.walk(root_path):
        for filename in filenames:
            if "csv.gz" in filename :# and filename[:2] == "00":
                file_path = os.path.join(filepath,filename)
                _, _day, _data_type, _exchange, _, _ = file_path.split("/")
                if(
                    (not code or code in filename) and 
                    (not trading_day or trading_day == _day) and 
                    (not exchange or exchange == _exchange) and 
                    (not data_type or data_type == _data_type)
                ):
                    df_list.append(pd.read_csv(file_path, compression='gzip'))
                    """
                    _new_df = format_file(file_path, "./", exchange, data_type)
                    if loaded_file:
                        df = pd.concat([df, _new_df],axis=1)
                    else:
                        df = _new_df
                    loaded_file = True
                    """
                    i += 1
                    #if i > 10:
                    #    break
    print(trading_day, exchange, data_type)
    if(df_list):
        df = pd.concat(df_list, ignore_index=True)
        print(df.head())
        if data_type == "Order":
            df = format_order_df(df, exchange)
        elif data_type == "Transaction":
            df = format_matches_df(df, exchange)
        elif data_type == "Tick":
            df = format_tick_df(df, exchange)
        elif data_type == "OrderQueue":
            df = format_queue_df(df, exchange)
        else:
            return
        print(df.head())
        print(df.dtypes)
        df.to_parquet(out_file_path, compression='gzip')

root_path = "./"

_time_num = 20230216145959840
print(trance_datetime(_time_num))

#get_files(root_path, "/opt/ti_data/ti_market_data/", trading_day = "20230307", exchange= "SH", data_type = "Transaction")
#exit()

for path in os.listdir(root_path):
    try:
        print(int(path))
    except:
        print(path)
        print("continue")
        continue
    #for type in ["Tick"]:
    for type in ["Order", "Transaction", "Tick"]:
        for ex in ["SH", "SZ"]:
            get_files(root_path, "/opt/ti_data/ti_market_data/", trading_day = path, exchange= ex, data_type = type)
