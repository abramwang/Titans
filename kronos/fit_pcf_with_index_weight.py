import pymoneta.quote as quote
import pymoneta.data as data
import pandas as pd
import numpy as np
from datetime import datetime
from sklearn.linear_model import LinearRegression
from sqlalchemy import create_engine

# 创建MySQL数据库连接引擎
# 请替换以下参数为您的数据库信息
username = 'root'
password = 'fed10b43-ff8d-44ef-ab5a-9c2443790f5d'
host = '192.168.71.215'
port = '20000'  # MySQL默认端口

# 创建数据库引擎
ti_data_engine = create_engine(f'mysql+mysqlconnector://{username}:{password}@{host}:{port}/ti_data')
pcf_engine = create_engine(f'mysql+mysqlconnector://{username}:{password}@{host}:{port}/pcf')
today = datetime.now().strftime('%Y%m%d')
today_2 = datetime.now().strftime('%Y-%m-%d')

def get_etf_code_list():
    etf_info_df = pd.read_sql(f"""SELECT * FROM etf_info WHERE m_tradeDate = '{today}'
                              AND m_fundType = 'StockIndex';
                              """, pcf_engine)
    etf_info_df.rename(columns={'m_fundId': 'symbol', 'm_exchange':'exchange'}, inplace=True)
   

    etf_instruments_df = pd.read_sql("SELECT * FROM rq_base_etf_instruments WHERE status ='Active'", ti_data_engine)
    etf_instruments_df = etf_instruments_df[["symbol","exchange", "order_book_id", "underlying_order_book_id", "underlying_symbol", "underlying_exchange"]]
    
    etf_info_merge_df = pd.merge(etf_info_df, etf_instruments_df, on=["symbol", "exchange"], how="left")
    
    etf_code_list = []
    for index, row in etf_info_merge_df.iterrows():
        etf_code_list.append({
            "symbol": row["symbol"],
            "exchange": row["exchange"],
            "order_book_id": row["order_book_id"]
        })
    print(len(etf_code_list), etf_code_list[0], etf_code_list[-1])
    return etf_code_list

def fit_pcf_index_weight(ETF_code):
    etf_constituent_info_df = pd.read_sql(f"SELECT * FROM constituent_info WHERE m_fundId IN ('{ETF_code['symbol']}') AND m_tradeDate = '{today}';", pcf_engine)
    etf_constituent_info_df["date"] = pd.to_datetime(etf_constituent_info_df["m_tradeDate"].astype(str), format="%Y%m%d")
    etf_constituent_info_df.rename(columns={"m_symbol": "symbol", "m_exchange": "exchange"}, inplace=True)

    if etf_constituent_info_df.shape[0] == 1:
        print(f"ETF {ETF_code['symbol']} has no constituent info")
        return False
    
    exchange_set = set(etf_constituent_info_df["exchange"].unique())
    if "HK" in exchange_set:
        print(f"ETF {ETF_code['symbol']} has HK exchanges")
        return False
    
    if "BJ" in exchange_set:
        print(f"ETF {ETF_code['symbol']} has BJ exchanges")
        return False
    #print(ETF_code["order_book_id"], etf_constituent_info_df["exchange"].unique())

    etf_instruments_df = pd.read_sql(f"SELECT * FROM rq_base_etf_instruments WHERE status ='Active' AND symbol = '{ETF_code['symbol']}'", ti_data_engine)
    etf_instruments_df = etf_instruments_df[["symbol","exchange", "order_book_id", "underlying_order_book_id", "underlying_symbol", "underlying_exchange"]]
    for index, row in etf_instruments_df.iterrows():
        if row["symbol"] == ETF_code["symbol"] and row["exchange"] == ETF_code["exchange"]:
            ETF_code["underlying_order_book_id"] = row["underlying_order_book_id"]
            ETF_code["underlying_symbol"] = row["underlying_symbol"]
            ETF_code["underlying_exchange"] = row["underlying_exchange"]
    #print(ETF_code)

    index_weight_df = pd.read_sql(f"SELECT * FROM ti_index_weights WHERE order_book_id ='{ETF_code['underlying_order_book_id']}' and date = '{today_2}'", ti_data_engine)
    index_weight_df = index_weight_df[["date", "component_symbol", "component_exchange", "weight"]]
    index_weight_df["date"] = pd.to_datetime(index_weight_df["date"])
    index_weight_df.rename(columns={"component_symbol": "symbol", "component_exchange": "exchange"}, inplace=True)

    if index_weight_df.empty:
        print(f"ETF {ETF_code['symbol']} has no index weight")
        return False
    
    if index_weight_df.shape[0] > etf_constituent_info_df.shape[0] * 1.5:
        print(f"ETF {ETF_code['symbol']} has more index weight {index_weight_df.shape[0]} than constituent info {etf_constituent_info_df.shape[0]}")
        return False

    etf_constituent_info_df = pd.merge(etf_constituent_info_df, index_weight_df, on=["date", "symbol", "exchange"], how = "outer")
    etf_constituent_info_df["m_tradeDate"] = etf_constituent_info_df["m_tradeDate"].ffill()
    etf_constituent_info_df["m_tradeDate"] = etf_constituent_info_df["m_tradeDate"].bfill()
    etf_constituent_info_df["m_fundId"] = etf_constituent_info_df["m_fundId"].ffill()
    etf_constituent_info_df["m_fundId"] = etf_constituent_info_df["m_fundId"].bfill()
    etf_constituent_info_df["m_name"] = etf_constituent_info_df["m_name"].fillna('')
    etf_constituent_info_df["m_replace_flag"] = etf_constituent_info_df["m_replace_flag"].fillna('ERT_CASH_OPTIONAL')
    etf_constituent_info_df["m_cash_replaced_creation_premium_rate"] = etf_constituent_info_df["m_cash_replaced_creation_premium_rate"].ffill()
    etf_constituent_info_df["m_cash_replaced_creation_premium_rate"] = etf_constituent_info_df["m_cash_replaced_creation_premium_rate"].bfill()
    etf_constituent_info_df["m_cash_replaced_redemption_discount_rate"] = etf_constituent_info_df["m_cash_replaced_redemption_discount_rate"].ffill()
    etf_constituent_info_df["m_cash_replaced_redemption_discount_rate"] = etf_constituent_info_df["m_cash_replaced_redemption_discount_rate"].bfill()
    
    #print(etf_constituent_info_df)
    #return True

    stock_day_bar_df = pd.read_sql(f"SELECT * FROM ti_instrument_info;", ti_data_engine)
    stock_day_bar_df["date"] = pd.to_datetime(stock_day_bar_df["update_date"])
    stock_day_bar_df.rename(columns={"pre_close": "prev_close"}, inplace=True)


    etf_constituent_info_df = pd.merge(etf_constituent_info_df, stock_day_bar_df, on=["date", "symbol", "exchange"], how="left")
    etf_constituent_info_df["market_value"] = etf_constituent_info_df["m_disclosure_vol"] * etf_constituent_info_df["prev_close"]
    etf_constituent_info_df.fillna(0, inplace=True)
    
    fit_df_list = []
    group = etf_constituent_info_df.groupby("date")
    for date, group_df in group:
        #print(group_df)
        # 计算列B与列A的偏差
        X = group_df['weight'].values.reshape(-1, 1)
        y = group_df['market_value'].values

        # 使用线性回归拟合常数 c
        model = LinearRegression().fit(X, y)
        c = model.coef_[0]

        print(date, c)

        group_df["new_weight"] = c * group_df["weight"]
        group_df["fit_reality_vol"] = group_df["new_weight"] / group_df["prev_close"]

        fit_df_list.append(group_df.copy())

        #test_df = group_df[["symbol", "exchange", "m_disclosure_vol", "m_reality_vol", "fit_reality_vol"]]
        #break

    new_etf_constituent_info_df = pd.concat(fit_df_list)
    #print(new_etf_constituent_info_df)
    new_etf_constituent_info_df.rename(columns={
        "symbol": "m_symbol", 
        "exchange": "m_exchange", 
        "fit_reality_vol": "m_fit_reality_vol"}, inplace=True)
    new_etf_constituent_info_df.fillna(0, inplace=True)
    new_etf_constituent_info_df = new_etf_constituent_info_df[
        ["m_tradeDate", "m_fundId", "m_symbol", "m_exchange", "m_name", 
        "m_replace_flag", "m_replace_amount", "m_creation_amount", "m_redemption_amount",
        "m_disclosure_vol", "m_reality_vol", "m_fit_reality_vol",
        "m_cash_replaced_creation_premium_rate", "m_cash_replaced_redemption_discount_rate"]]
    new_etf_constituent_info_df.to_sql("ti_fit_constituent_info", ti_data_engine, if_exists="append", index=False)

    return True

def main():
    etf_code_list = get_etf_code_list()
    fitted_code_list = []
    for ETF_code in etf_code_list:
        #if ETF_code["symbol"] != "159150":
        #    continue
        if fit_pcf_index_weight(ETF_code):
            fitted_code_list.append(ETF_code)
            #break
    print(len(fitted_code_list), fitted_code_list[0], fitted_code_list[-1])

if __name__ == "__main__":
    main()