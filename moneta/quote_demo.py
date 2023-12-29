#!/usr/bin/env python
# coding=utf-8

import pymoneta.quote as quote
from datetime import datetime


time_delta_list = []
x = 0

def cb(topic_, data_):
    print(topic_, data_)
    
def main():
    quote.Init('47.103.74.35', 20183, 't0')

    mq.Sub("*", cb)
        #time.sleep(1)
    pass

main()