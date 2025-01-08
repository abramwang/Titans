#include <fstream>
#include <string>
#include <iomanip> // for std::setprecision
#include <cstdio> // 提供 std::remove 和 std::fopen
#include <iostream>
#include <unistd.h>
#include <set>
#include <glog/logging.h>

#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_ipc_client.h"
#include "ti_quote_formater.h"

#include "datetime.h"

#define Enable_QuoteDataOutput 1

bool is_file_exist(const char* filePath)
{
    FILE* file = std::fopen(filePath, "r");
    if (file) {
        std::fclose(file); // 关闭文件
        return true;
    } else {
        return false;
    }
}


class UserCallback : 
    public TiQuoteCallback, public TiQuoteDepthCallback
{
private:
    std::string m_topic;
    uv_loop_s* m_loop;
    uv_timer_t m_timer;

    int64_t m_cout_future_time;
    int64_t m_cout_snap_time;
    int64_t m_cout_match_time;
    int64_t m_cout_order_time;
    int64_t m_cout_order_book_time;
    std::set<std::string> m_selected_symbols;
    
    TI_ISODateTimeType m_recv_time_str;
    int64_t m_recv_timestamp;
public:
    UserCallback(std::string topic, uv_loop_s* loop){
        m_topic = topic;
        m_loop = loop;
        m_cout_future_time = 0;
        m_cout_snap_time = 0;
        m_cout_match_time = 0;
        m_cout_order_time = 0;
        m_cout_order_book_time = 0;

            
        m_timer.data = this;
        uv_timer_init(loop, &m_timer);
        uv_timer_start(&m_timer, onTimer, 1000, 500);
    };
    virtual ~UserCallback(){};
public:
    static void onTimer(uv_timer_t* handle)
    {
        UserCallback* pThis = (UserCallback*)handle->data;
        pThis->OnTimer();
    };

    virtual void OnTimer()
    {
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        if (localTime->tm_hour >= 15 )
        {
            if (localTime->tm_hour == 15 && localTime->tm_min < 30)
            {
                return;
            }
            printf("terminate\n");
            std::terminate();
        }
    };;

    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){
        auto iter = m_selected_symbols.find(pData->symbol);
        if(iter == m_selected_symbols.end()){
            return;
        }
        printf("[OnL2IndexSnapshotRtn] %s, %s, %d, %s, %f, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->pre_close);

    };

    
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
    {
        if ((pData->time - m_cout_future_time) > 5000)
        {
            printf("[OnL2FutureSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_future_time = pData->time;
        }
    };

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
#if Enable_QuoteDataOutput
        try {
            WriteToCSV(*pData, m_topic + ".quote_snap_data.csv");
        } catch (const std::exception& e) {
            // 捕获异常并打印错误
            std::cerr << "Error writing to CSV: " << e.what() << std::endl;
        }
#endif
        if ((pData->time - m_cout_snap_time) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f, %f, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover,
                pData->high_limit, pData->low_limit);
            //json j;
            //TiQuoteFormater::FormatSnapshot(pData, j);
            //std::cout << j << std::endl;
            m_cout_snap_time = pData->time;
        }
    };
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
#if Enable_QuoteDataOutput
        try {
            WriteToCSV(*pData, m_topic + ".quote_matches_data.csv");
        } catch (const std::exception& e) {
            // 捕获异常并打印错误
            std::cerr << "Error writing to CSV: " << e.what() << std::endl;
        }
#endif
        if ((pData->time - m_cout_match_time) > 5000)
        {     
            printf("[OnL2StockMatchesRtn] %s, %s, %s, %d, %d, %f, %d, %c, %c\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time, pData->seq, pData->price, pData->volume, 
                pData->bs_flag, pData->function_code);
            m_cout_match_time = pData->time;
        }
    };
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){
#if Enable_QuoteDataOutput
        try {
            WriteToCSV(*pData, m_topic + ".quote_order_data.csv");
        } catch (const std::exception& e) {
            // 捕获异常并打印错误
            std::cerr << "Error writing to CSV: " << e.what() << std::endl;
        }
#endif
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

    void WriteToCSV(const TiQuoteSnapshotStockField& data, const std::string& fileName) {
        bool isHeaderWritten = is_file_exist(fileName.c_str());
        // 打开文件（如果文件不存在，会创建新文件）
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + fileName);
        }

        // 如果是新文件，写入标题行
        if (!isHeaderWritten) {
            // 写入标题
            file << "exchange,symbol,date,time,timestamp,time_str,recv_time_str,recv_timestamp,last,pre_close,open,high,low,high_limit,low_limit,"
                << "volume,turnover,acc_volume,acc_turnover,match_items,total_ask_qty,total_bid_qty,wavg_ask,wavg_bid,interest,"
                << "ask_price,bid_price,ask_volume,bid_volume,ask_order_num,bid_order_num,iopv,pre_close_iopv\n";
        }

        m_recv_timestamp  = datetime::get_now_timestamp_ms();
        datetime::get_format_now_time_us(m_recv_time_str, TI_TIME_STR_LEN);

        // 写入数据
        file << data.exchange << "," << data.symbol << "," << data.date << "," << data.time << "," << data.timestamp << ","
            << data.time_str << "," << m_recv_time_str << "," << m_recv_timestamp << "," << data.last << "," << data.pre_close << ","
            << data.open << "," << data.high << "," << data.low << "," << data.high_limit << "," << data.low_limit << ","
            << data.volume << "," << data.turnover << "," << data.acc_volume << "," << data.acc_turnover << ","
            << data.match_items << "," << data.total_ask_qty << "," << data.total_bid_qty << "," << data.wavg_ask << ","
            << data.wavg_bid << "," << data.interest << ",";

        // 写入数组数据
        for (int i = 0; i < TI_STOCK_ARRAY_LEN; ++i) {
            file << data.ask_price[i];
            if (i < TI_STOCK_ARRAY_LEN - 1) file << "|"; // 用 '|' 分隔数组元素
        }
        file << ",";

        for (int i = 0; i < TI_STOCK_ARRAY_LEN; ++i) {
            file << data.bid_price[i];
            if (i < TI_STOCK_ARRAY_LEN - 1) file << "|";
        }
        file << ",";

        for (int i = 0; i < TI_STOCK_ARRAY_LEN; ++i) {
            file << data.ask_volume[i];
            if (i < TI_STOCK_ARRAY_LEN - 1) file << "|";
        }
        file << ",";

        for (int i = 0; i < TI_STOCK_ARRAY_LEN; ++i) {
            file << data.bid_volume[i];
            if (i < TI_STOCK_ARRAY_LEN - 1) file << "|";
        }
        file << ",";

        for (int i = 0; i < TI_STOCK_ARRAY_LEN; ++i) {
            file << data.ask_order_num[i];
            if (i < TI_STOCK_ARRAY_LEN - 1) file << "|";
        }
        file << ",";

        for (int i = 0; i < TI_STOCK_ARRAY_LEN; ++i) {
            file << data.bid_order_num[i];
            if (i < TI_STOCK_ARRAY_LEN - 1) file << "|";
        }
        file << ",";

        file << data.iopv << "," << data.pre_close_iopv << "\n";

        file.close();
    }


    // 定义一个函数，将结构体数据写入 CSV 文件
    void WriteToCSV(const TiQuoteOrderField& data, const std::string& fileName) {
        bool isHeaderWritten = is_file_exist(fileName.c_str());

        // 打开文件（如果文件不存在，会创建新文件）
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + fileName);
        }

        if (!isHeaderWritten) {
            file << "Exchange,Symbol,Date,Time,Timestamp,TimeStr,RecvTimeStr,RecvTimestamp,Channel,Seq,OrderOrino,Price,Volume,OrderType,FunctionCode" << std::endl;
        }

        // 写入数据行
        file << data.exchange << ","
            << data.symbol << ","
            << data.date << ","
            << data.time << ","
            << data.timestamp << ","
            << data.time_str << ","
            << data.recv_time_str << ","
            << data.recv_timestamp << ","
            << data.channel << ","
            << data.seq << ","
            << data.order_orino << ","
            << std::fixed << std::setprecision(2) << data.price << ","
            << data.volume << ","
            << static_cast<char>(data.order_type) << ","
            << static_cast<char>(data.function_code) << std::endl;

        file.close();
    }

    void WriteToCSV(const TiQuoteMatchesField& data, const std::string& fileName) {
        bool isHeaderWritten = is_file_exist(fileName.c_str());
        // 打开文件（如果文件不存在，会创建新文件）
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + fileName);
        }

        // 如果是新文件，写入标题行
        if (!isHeaderWritten) {
            file << "Exchange,Symbol,Date,Time,Timestamp,TimeStr,RecvTimeStr,RecvTimestamp,Channel,Seq,Price,Volume,BsFlag,FunctionCode,AskOrderSeq,BidOrderSeq" << std::endl;
        }

        // 写入数据行
        file << data.exchange << ","
            << data.symbol << ","
            << data.date << ","
            << data.time << ","
            << data.timestamp << ","
            << data.time_str << ","
            << data.recv_time_str << ","
            << data.recv_timestamp << ","
            << data.channel << ","
            << data.seq << ","
            << std::fixed << std::setprecision(2) << data.price << ","
            << data.volume << ","
            << static_cast<char>(data.bs_flag) << ","
            << static_cast<char>(data.function_code) << ","
            << data.ask_order_seq << ","
            << data.bid_order_seq << std::endl;

        file.close();
    }
};

void remove_file(const char* filePath)
{
    FILE* file = std::fopen(filePath, "r");
    if (file) {
        std::fclose(file); // 关闭文件

        // 删除文件
        if (std::remove(filePath) == 0) {
            std::cout << "文件已删除: " << filePath << std::endl;
        } else {
            std::cerr << "删除文件失败: " << filePath << std::endl;
        }
    } else {
        std::cout << "文件不存在: " << filePath << std::endl;
    }

}

int main(int argc, char* argv[]) {
    std::string topic = "quote_data";
    if (argc == 2)
    {
        topic = argv[1];
    }

    remove_file((topic + ".quote_snap_data.csv").c_str());
    remove_file((topic + ".quote_matches_data.csv").c_str());
    remove_file((topic + ".quote_order_data.csv").c_str());

	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    UserCallback cb(topic, loop);

    TiQuoteIpcClient client("./config.ini", loop, &cb, &cb);
	
    if (argc == 2)
    {
        client.run(argv[1]);
    }else{
        client.run(NULL);
    }
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
