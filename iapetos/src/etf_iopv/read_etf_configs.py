#!/usr/bin/env python
# coding=utf-8

import os
import xlrd2
import redis
import json

def readExcel(file_name, file_path, quotient):
    print(file_name, file_path)
    excel_file = xlrd2.open_workbook(file_path)
    sheet = excel_file.sheets()[0]

    stock_weight_array = []
    for i in range(1, sheet.nrows):
        row = sheet.row_values(i)
        code = row[0]
        name = row[1]
        ex = ""
        weight = 0
        try:
            weight = float(row[2])
        except:
            weight = float(row[2].replace(",",""))
        prefix = code[:2]
        if prefix == "00" or prefix == "30":
            ex = "SZ"
        if prefix == "60" or prefix == "68":
            ex = "SH"
        if ex:
            stock_weight_array.append([code, ex, name, weight])
    
    etf_name = file_name.split(".")[0]
    return etf_name, {
        "etf_name" : etf_name,
        "quotient" : quotient,
        "stock_weight_array" : stock_weight_array
    }

def setEtfToRedis(etf, config):
    pool = redis.ConnectionPool(host='172.19.93.98', port=20184, password ="kymq20180", decode_responses=True)
    r = redis.Redis(connection_pool=pool)
    r.hset("operator.etf.iopv.conf", etf, json.dumps(config))

def listDir(root):
    quotient = 1000000
    for dirpath, dirnames, filenames in os.walk(root):
        for filename in filenames:
            if "xlsx" in filename:
                ex = "SZ"
                if "560010" in filename:
                    quotient = 1000000
                    ex = "SH"
                if "512100" in filename:
                    quotient = 1000000
                    ex = "SH"
                if "159633" in filename:
                    quotient = 1300000
                if "159845" in filename:
                    quotient = 2000000
                if "159629" in filename:
                    quotient = 2800000
                print(filename, ex)
                etf_name, result = readExcel(filename, os.path.join(dirpath, filename), quotient)
                result["etf_exchange"] = ex
                setEtfToRedis(etf_name, result)
    pass

def main():
    listDir("./etf_config")

    pass

main()