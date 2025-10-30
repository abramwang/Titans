#include <iostream>
#include <unistd.h>
#include <set>
#include <glog/logging.h>

#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_ipc_client.h"
#include "ti_quote_formater.h"

#include "ti_influxdb_client.h"

class UserCallback : 
    public TiQuoteCallback, public TiQuoteDepthCallback
{
private:
    TiInfluxdbClient* m_influxdb_client;

    uv_timer_t m_timer;

    int64_t m_cout_future_time;
    int64_t m_cout_snap_time;
    int64_t m_cout_match_time;
    int64_t m_cout_order_time;
    int64_t m_cout_order_book_time;
public:
    UserCallback(uv_loop_s* loop){
        m_influxdb_client = new TiInfluxdbClient(
            "http://192.168.3.100:8086", 
            "NKvi_7kqu2enf8vYiwYcl-cc8UnR5_ne4pZEMxPOWJkh9vGfrRuN0KUyX0DxxoEBQsHuosg3X7KX-N2QyAKogg==");

        m_cout_future_time = 0;
        m_cout_snap_time = 0;
        m_cout_match_time = 0;
        m_cout_order_time = 0;
        m_cout_order_book_time = 0;

            
        m_timer.data = this;
        uv_timer_init(loop, &m_timer);
        uv_timer_start(&m_timer, onTimer, 1000, 100);
    };
    virtual ~UserCallback(){};
public:
    virtual void OnTimer()
    {
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);

        if (localTime->tm_hour > 15 )
        {
            std::cout << "std::terminate" << std::endl;
            std::terminate();
        }
    };

    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){
    
        auto rtn = m_influxdb_client->add_point();
        rtn->meas("index");
        rtn->tag("symbol", pData->symbol);
        rtn->tag("exchange", pData->exchange);
        rtn->field("volume", pData->volume);
        rtn->field("turnover", pData->turnover);
        rtn->field("open", pData->open);
        rtn->field("high", pData->high);
        rtn->field("low", pData->low);
        rtn->field("last", pData->last);
        rtn->field("pre_close", pData->pre_close);
        rtn->timestamp(pData->timestamp);

        printf("[OnL2IndexSnapshotRtn] %s, %s, %d, %s, %f, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->pre_close);

    };

    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
    {
        auto rtn = m_influxdb_client->add_point();
        rtn->meas("future");
        rtn->tag("symbol", pData->symbol);
        rtn->tag("exchange", pData->exchange);
        rtn->field("volume", pData->volume);
        rtn->field("turnover", pData->turnover);
        rtn->field("acc_volume", pData->acc_volume);
        rtn->field("acc_turnover", pData->acc_turnover);
        rtn->field("open", pData->open);
        rtn->field("high", pData->high);
        rtn->field("low", pData->low);
        rtn->field("last", pData->last);
        rtn->field("high_limit", pData->high_limit);
        rtn->field("low_limit", pData->low_limit);
        rtn->field("pre_close", pData->pre_close);
        rtn->field("bid_price_1", pData->bid_price[0]);
        rtn->field("bid_volume_1", pData->bid_volume[0]);
        rtn->field("ask_price_1", pData->ask_price[0]);
        rtn->field("ask_volume_1", pData->ask_volume[0]);
        rtn->timestamp(pData->timestamp);


        if ((pData->time - m_cout_future_time) > 5000)
        {
            m_influxdb_client->write("ti_tick_snapshot", "ti_quant", "ms");
            printf("[OnL2FutureSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_future_time = pData->time;
        }
    };

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        /*
        auto iter = m_selected_symbols.find(pData->symbol);
        if(iter == m_selected_symbols.end()){
            return;
        }
        */

        auto rtn = m_influxdb_client->add_point();
        rtn->meas("stock");
        rtn->tag("symbol", pData->symbol);
        rtn->tag("exchange", pData->exchange);
        rtn->field("volume", pData->volume);
        rtn->field("turnover", pData->turnover);
        rtn->field("acc_volume", pData->acc_volume);
        rtn->field("acc_turnover", pData->acc_turnover);
        rtn->field("match_items", pData->match_items);
        rtn->field("open", pData->open);
        rtn->field("high", pData->high);
        rtn->field("low", pData->low);
        rtn->field("last", pData->last);
        rtn->field("high_limit", pData->high_limit);
        rtn->field("low_limit", pData->low_limit);
        rtn->field("pre_close", pData->pre_close);
        rtn->field("bid_price_1", pData->bid_price[0]);
        rtn->field("bid_volume_1", pData->bid_volume[0]);
        rtn->field("ask_price_1", pData->ask_price[0]);
        rtn->field("ask_volume_1", pData->ask_volume[0]);
        rtn->field("match_items", pData->match_items);
        rtn->field("total_ask_qty", pData->total_ask_qty);
        rtn->field("total_bid_qty", pData->total_bid_qty);
        rtn->field("wavg_ask", pData->wavg_ask);
        rtn->field("wavg_bid", pData->wavg_bid);
        rtn->field("iopv", pData->iopv);
        
        rtn->timestamp(pData->timestamp);

        if ((pData->time - m_cout_snap_time) > 5000)
        {
            m_influxdb_client->write("ti_tick_snapshot", "ti_quant", "ms");
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            //json j;
            //TiQuoteFormater::FormatSnapshot(pData, j);
            //std::cout << j << std::endl;
            m_cout_snap_time = pData->time;
        }
    };
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        //return;
        if ((pData->time - m_cout_match_time) > 5000)
        {     
            printf("[OnL2StockMatchesRtn] %s, %s, %s, %d, %d, %f, %d, %c, %c\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time, pData->seq, pData->price, pData->volume, 
                pData->bs_flag, pData->function_code);
            m_cout_match_time = pData->time;
        }
    };
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){
        //return;
        if ((pData->time - m_cout_order_time) > 5000)
        {
            printf("[OnL2StockOrderRtn] %s, %s, %s, %d, %d, %f, %d, %c, %c\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time,  pData->seq, pData->price, pData->volume, 
                pData->order_type, pData->function_code);
            m_cout_order_time = pData->time;
        }
    };

    
    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids){};
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData){
        //return;
        if ((pData->time - m_cout_order_book_time) > 1000)
        {
            printf("[OnDepthOrderBookLevelRtn] %s, %s, %s, %d, %d, ask:[%f, %d, %d], bid:[%f, %d, %d], ask order {%d, %d, %d, %d}, bid order {%d, %d, %d, %d}\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time, pData->level,
                pData->ask_price, pData->ask_volume, pData->ask_order_count, pData->bid_price, pData->bid_volume, pData->bid_order_count,
                pData->ask_orders[0].seq, pData->ask_orders[0].order_orino, pData->ask_orders[0].volume, pData->ask_orders[0].dealt_vol,
                pData->bid_orders[0].seq, pData->bid_orders[0].order_orino, pData->bid_orders[0].volume, pData->bid_orders[0].dealt_vol
            );
            
            m_cout_order_book_time = pData->time;
        }
    };
    
    static void onTimer(uv_timer_t* handle){
        UserCallback* pThis = (UserCallback*)handle->data;
        pThis->OnTimer();
    };
};

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    UserCallback cb(loop);

    TiQuoteIpcClient client("./config.ini", loop, &cb, &cb);
	
    if (argc == 2)
    {
        client.run(argv[1]);
    }else{
        client.run(NULL);
    }
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
