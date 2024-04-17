#!/usr/bin/env python
# coding=utf-8

import pymoneta.quote as quote
import pymoneta.quote.callback as cb
import pymoneta.data as data
from datetime import datetime


class MyCallback(cb.CallBack):
    def OnSnapshotRtn(self, data):
        print(data)
    
    
def main():
    #data.SetRootPath("http://192.168.124.5")
    """
    data.SetRootPath("http://172.17.0.1")

    df = data.GetBaseData(["600000", "000001"], "ex_factor")
    print(df)

    return
    #df = data.GetBar(datetime(2024,1,16), "SH", "600000", "1m")
    #df = data.GetBar(20240116, "SH", "600000", "1m")
    
    #df = data.GetMarket(20240116, "BNB", "*", "trades")

    #df = data.GetMarket("20230309", "SZ", "*", "matches", columns_=["timestamp", "exchange", "symbol", "price", "volume"])

    #df = data.GetDayBar("stock", ["600000", "000001"], "20210101", "20210131")
    #df = data.GetDayBar("index", "*", "20210101", "20210131")
    #df = data.GetDayBar("etf", "*", "20210101", "20210131")

    df = data.GetMarket(20230713, "SH", "*", "matches")

    #df = data.GetBarBySymbol("BNB", "BTCUSDT", "1m", "20230101", "20231231 12:00:00")

    print(df)
    exit()
    """
    quote.Init('172.17.0.1', 20184, 'lw', "P7pO48Lw4AZTOLXKlR")

    """
    quote.SubQuote("snapshot", "SH", [
        "000053",
        "600000",
        "600004",
        "688788"
    ])

    quote.SubQuote("snapshot", "SZ", [
        "000001",
        "000006",
        "000053",
        "688788"
    ])

    quote.SubQuote("snapshot", "CF", [
        "IF2402",
        "IH2402",
        "IC2402",
        "IM2402"
    ])
    """
    quote.SubQuote("snapshot", "CF", ["*"])

    new_cb = MyCallback()
    quote.ReadLoop(new_cb)
        #time.sleep(1)
    pass

main()
