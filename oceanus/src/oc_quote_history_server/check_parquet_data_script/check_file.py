import pandas as pd

#pd.options.display.max_rows = None
pd.options.display.max_columns = None
if 0:
    #df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.order.SH.20230216.parquet")
    df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.order.SZ.20211018.parquet")
    print(df.dtypes)
    print("function_code")
    print(df['function_code'].unique())
    print("order_type")
    print(df['order_type'].unique())

    df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.order.SH.20211018.parquet")
    print(df.dtypes)
    print("function_code")
    print(df['function_code'].unique())
    print("order_type")
    print(df['order_type'].unique())

if 0:
    df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.order.SZ.20230216.parquet")
    print(df.dtypes)
    print("function_code")
    print(df['function_code'].unique())
    print("order_type")
    print(df['order_type'].unique())

    df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.order.SH.20230216.parquet")
    print(df.dtypes)
    print("function_code")
    print(df['function_code'].unique())
    print("order_type")
    print(df['order_type'].unique())

if 0:
    if 1:
        df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.matches.SZ.20211018.parquet")
        print(df.dtypes)
        print("function_code")
        print(df['function_code'].unique())
        print("trade_type")
        print(df['trade_type'].unique())
        print("bs_flag")
        print(df['bs_flag'].unique())

        df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.matches.SH.20211018.parquet")
        print(df.dtypes)
        print("function_code")
        print(df['function_code'].unique())
        print("trade_type")
        print(df['trade_type'].unique())
        print("bs_flag")
        print(df['bs_flag'].unique())

    if 1:
        df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.matches.SZ.20230216.parquet")
        print(df.dtypes)
        print("function_code")
        print(df['function_code'].unique())
        print("trade_type")
        print(df['trade_type'].unique())
        print("bs_flag")
        print(df['bs_flag'].unique())

        df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.matches.SH.20230216.parquet")
        print(df.dtypes)
        print("function_code")
        print(df['function_code'].unique())
        print("trade_type")
        print(df['trade_type'].unique())
        print("bs_flag")
        print(df['bs_flag'].unique())

    
#df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.snapshot.SH.20230307.parquet")
#print(df.dtypes)

df = pd.read_parquet("/opt/ti_data/ti_market_data/ti.snapshot.SH.20230407.parquet")
symbol_list =["603489", "603938"]
df = df.query('symbol in @symbol_list and time > 91900000 and time < 150000000')
print(df.dtypes)
print(df)