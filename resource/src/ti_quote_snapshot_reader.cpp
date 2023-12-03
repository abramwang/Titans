#include "ti_quote_snapshot_reader.h"
#include "datetime.h"
#include <iostream>

TiQuoteSnapshotReader::TiQuoteSnapshotReader(const char* path, char* field_list[], size_t len)
    : TiQuoteParquetReader(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK)
{
    std::cout << "Reading first column of " << path << std::endl;
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(
        infile,
        arrow::io::ReadableFile::Open(path,
                                    arrow::default_memory_pool()));
    PARQUET_THROW_NOT_OK(
        parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &m_reader));
    
    PARQUET_THROW_NOT_OK(m_reader->GetSchema(&m_file_schema));

    memset(&m_data, 0, sizeof(TiQuoteMatchesField));
    nIndex = 0;

    m_date_array                              = 0;
    m_time_array                              = 0;
    m_symbol_array                            = 0;
    m_last_array                              = 0;
    m_volume_array                            = 0;
    m_turnover_array                          = 0;
    m_match_items_array                       = 0;
    m_interest_array                          = 0;
    m_acc_volume_array                        = 0;
    m_acc_turnover_array                      = 0;
    m_high_array                              = 0;
    m_low_array                               = 0;
    m_open_array                              = 0;
    m_pre_close_array                         = 0;
    m_ask_price_array[TI_STOCK_ARRAY_LEN]     = {0};
    m_bid_price_array[TI_STOCK_ARRAY_LEN]     = {0};
    m_ask_order_num_array[TI_STOCK_ARRAY_LEN] = {0};
    m_bid_order_num_array[TI_STOCK_ARRAY_LEN] = {0};
    m_ask_volume_array[TI_STOCK_ARRAY_LEN]    = {0};
    m_bid_volume_array[TI_STOCK_ARRAY_LEN]    = {0};
    m_wavg_ask_array                          = 0;
    m_wavg_bid_array                          = 0;
    m_total_ask_qty_array                     = 0;
    m_total_bid_qty_array                     = 0;
    m_symbol_array                            = 0;
    m_timestamp_array                         = 0;
    m_exchange_array                          = 0;

    initField(field_list, len);
    loadArray(path);
}

void TiQuoteSnapshotReader::initField(char* field_list[], size_t len){
    for(size_t i = 0; i < len; i++){
        std::shared_ptr<arrow::Field> _field = m_file_schema->GetFieldByName(field_list[i]);
        if(_field){
            m_field_set.insert(field_list[i]);
        }else{  // 兼容 ask_price1 ask_volume1 这样的数组字段
            char _field_name[32] = {0};
            sprintf(_field_name, "%s1", field_list[i]);
            _field = m_file_schema->GetFieldByName(_field_name);
            if(_field){
                m_field_set.insert(field_list[i]);
            }
        }
    }
};

void TiQuoteSnapshotReader::loadArray(std::string file_path){
    // 全部加载table
    if((m_field_set.size() >= (m_file_schema->num_fields()*0.6))
        || m_field_set.empty()
    ){
        //std::shared_ptr<arrow::Table> table;
        //PARQUET_THROW_NOT_OK(m_reader->ReadTable(&table));
        
        loadAllArray(file_path);
        
        if(m_file_schema->GetFieldByName("date"))
            m_date_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["date"]);
        if(m_file_schema->GetFieldByName("time"))
            m_time_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["time"]);
        if(m_file_schema->GetFieldByName("last"))
            m_last_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["last"]);
        if(m_file_schema->GetFieldByName("volume"))
            m_volume_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["volume"]);
        if(m_file_schema->GetFieldByName("turnover"))
            m_turnover_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["turnover"]);
        if(m_file_schema->GetFieldByName("match_items"))
            m_match_items_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["match_items"]);
        if(m_file_schema->GetFieldByName("interest"))
            m_interest_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["interest"]);
        if(m_file_schema->GetFieldByName("acc_volume"))
            m_acc_volume_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["acc_volume"]);
        if(m_file_schema->GetFieldByName("acc_turnover"))
            m_acc_turnover_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["acc_turnover"]);
        if(m_file_schema->GetFieldByName("high"))
            m_high_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["high"]);
        if(m_file_schema->GetFieldByName("low"))
            m_low_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["low"]);
        if(m_file_schema->GetFieldByName("open"))
            m_open_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["open"]);
        if(m_file_schema->GetFieldByName("pre_close"))
            m_pre_close_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["pre_close"]);
        if(m_file_schema->GetFieldByName("ask_price1")){
            char _field_name[32] = {0};
            for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
            {
                sprintf(_field_name, "ask_price%ld", i + 1);
                m_ask_price_array[i] = std::static_pointer_cast<arrow::DoubleArray>(m_array_map[_field_name]);
            }
        }
        if(m_file_schema->GetFieldByName("bid_price1")){
            char _field_name[32] = {0};
            for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
            {
                sprintf(_field_name, "bid_price%ld", i + 1);
                m_bid_price_array[i] = std::static_pointer_cast<arrow::DoubleArray>(m_array_map[_field_name]);
            }
        }
        if(m_file_schema->GetFieldByName("ask_order_num1")){
            char _field_name[32] = {0};
            for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
            {
                sprintf(_field_name, "ask_order_num%ld", i + 1);
                m_ask_order_num_array[i] = std::static_pointer_cast<arrow::Int64Array>(m_array_map[_field_name]);
            }
        }
        if(m_file_schema->GetFieldByName("bid_order_num1")){
            char _field_name[32] = {0};
            for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
            {
                sprintf(_field_name, "bid_order_num%ld", i + 1);
                m_bid_order_num_array[i] = std::static_pointer_cast<arrow::Int64Array>(m_array_map[_field_name]);
            }
        }
        if(m_file_schema->GetFieldByName("ask_volume1")){
            char _field_name[32] = {0};
            for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
            {
                sprintf(_field_name, "ask_volume%ld", i + 1);
                m_ask_volume_array[i] = std::static_pointer_cast<arrow::Int64Array>(m_array_map[_field_name]);
            }
        }
        if(m_file_schema->GetFieldByName("bid_volume1")){
            char _field_name[32] = {0};
            for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
            {
                sprintf(_field_name, "bid_volume%ld", i + 1);
                m_bid_volume_array[i] = std::static_pointer_cast<arrow::Int64Array>(m_array_map[_field_name]);
            }
        }
        if(m_file_schema->GetFieldByName("wavg_ask"))
            m_wavg_ask_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["wavg_ask"]);
        if(m_file_schema->GetFieldByName("wavg_bid"))
            m_wavg_bid_array = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["wavg_bid"]);
        if(m_file_schema->GetFieldByName("total_ask_qty"))
            m_total_ask_qty_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["total_ask_qty"]);
        if(m_file_schema->GetFieldByName("total_bid_qty"))
            m_total_bid_qty_array = std::static_pointer_cast<arrow::Int64Array>(m_array_map["total_bid_qty"]);
        if(m_file_schema->GetFieldByName("symbol"))
            m_symbol_array = std::static_pointer_cast<arrow::StringArray>(m_array_map["symbol"]);
        if(m_file_schema->GetFieldByName("timestamp"))
            m_timestamp_array = std::static_pointer_cast<arrow::TimestampArray>(m_array_map["timestamp"]);
        if(m_file_schema->GetFieldByName("exchange"))
            m_exchange_array = std::static_pointer_cast<arrow::StringArray>(m_array_map["exchange"]);

        return;
    }
    std::shared_ptr<arrow::ChunkedArray> _chuncked_array;
    std::shared_ptr<arrow::Field> _field;
    int _field_index = 0; 
    std::set<std::string>::iterator iter = m_field_set.begin();
    iter = m_field_set.find("date");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("date");
        _field_index = m_file_schema->GetFieldIndex("date");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_date_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("time");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("time");
        _field_index = m_file_schema->GetFieldIndex("time");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_time_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("last");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("last");
        _field_index = m_file_schema->GetFieldIndex("last");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_last_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("volume");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("volume");
        _field_index = m_file_schema->GetFieldIndex("volume");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_volume_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("turnover");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("turnover");
        _field_index = m_file_schema->GetFieldIndex("turnover");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_turnover_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("match_items");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("match_items");
        _field_index = m_file_schema->GetFieldIndex("match_items");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_match_items_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("interest");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("interest");
        _field_index = m_file_schema->GetFieldIndex("interest");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_interest_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("acc_volume");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("acc_volume");
        _field_index = m_file_schema->GetFieldIndex("acc_volume");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_acc_volume_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("acc_turnover");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("acc_turnover");
        _field_index = m_file_schema->GetFieldIndex("acc_turnover");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_acc_turnover_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("high");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("high");
        _field_index = m_file_schema->GetFieldIndex("high");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_high_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("low");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("low");
        _field_index = m_file_schema->GetFieldIndex("low");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_low_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("open");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("open");
        _field_index = m_file_schema->GetFieldIndex("open");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_open_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("pre_close");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("pre_close");
        _field_index = m_file_schema->GetFieldIndex("pre_close");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_pre_close_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("ask_price");
    if(iter != m_field_set.end())
    {
        char _field_name[32] = {0};
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            sprintf(_field_name, "ask_price%ld", i + 1);
            _field       = m_file_schema->GetFieldByName(_field_name);
            _field_index = m_file_schema->GetFieldIndex(_field_name);
            if(_field->type()->ToString() == "double"){
                PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
                m_ask_price_array[i] = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
            }
        }
    }
    iter = m_field_set.find("bid_price");
    if(iter != m_field_set.end())
    {
        char _field_name[32] = {0};
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            sprintf(_field_name, "bid_price%ld", i + 1);
            _field       = m_file_schema->GetFieldByName(_field_name);
            _field_index = m_file_schema->GetFieldIndex(_field_name);
            if(_field->type()->ToString() == "double"){
                PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
                m_bid_price_array[i] = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
            }
        }
    }
    iter = m_field_set.find("ask_order_num");
    if(iter != m_field_set.end())
    {
        char _field_name[32] = {0};
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            sprintf(_field_name, "ask_order_num%ld", i + 1);
            _field       = m_file_schema->GetFieldByName(_field_name);
            _field_index = m_file_schema->GetFieldIndex(_field_name);
            if(_field->type()->ToString() == "int64"){
                PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
                m_ask_order_num_array[i] = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
            }
        }
    }
    iter = m_field_set.find("bid_order_num");
    if(iter != m_field_set.end())
    {
        char _field_name[32] = {0};
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            sprintf(_field_name, "bid_order_num%ld", i + 1);
            _field       = m_file_schema->GetFieldByName(_field_name);
            _field_index = m_file_schema->GetFieldIndex(_field_name);
            if(_field->type()->ToString() == "int64"){
                PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
                m_bid_order_num_array[i] = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
            }
        }
    }
    iter = m_field_set.find("ask_volume");
    if(iter != m_field_set.end())
    {
        char _field_name[32] = {0};
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            sprintf(_field_name, "ask_volume%ld", i + 1);
            _field       = m_file_schema->GetFieldByName(_field_name);
            _field_index = m_file_schema->GetFieldIndex(_field_name);
            if(_field->type()->ToString() == "int64"){
                PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
                m_ask_volume_array[i] = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
            }
        }
    }
    iter = m_field_set.find("bid_volume");
    if(iter != m_field_set.end())
    {
        char _field_name[32] = {0};
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            sprintf(_field_name, "bid_volume%ld", i + 1);
            _field       = m_file_schema->GetFieldByName(_field_name);
            _field_index = m_file_schema->GetFieldIndex(_field_name);
            if(_field->type()->ToString() == "int64"){
                PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
                m_bid_volume_array[i] = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
            }
        }
    }
    iter = m_field_set.find("wavg_ask");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("wavg_ask");
        _field_index = m_file_schema->GetFieldIndex("wavg_ask");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_wavg_ask_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("wavg_bid");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("wavg_bid");
        _field_index = m_file_schema->GetFieldIndex("wavg_bid");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_wavg_bid_array = std::static_pointer_cast<arrow::DoubleArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("total_ask_qty");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("total_ask_qty");
        _field_index = m_file_schema->GetFieldIndex("total_ask_qty");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_total_ask_qty_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("total_bid_qty");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("total_bid_qty");
        _field_index = m_file_schema->GetFieldIndex("total_bid_qty");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_total_bid_qty_array = std::static_pointer_cast<arrow::Int64Array>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("symbol");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("symbol");
        _field_index = m_file_schema->GetFieldIndex("symbol");
        if(_field->type()->ToString() == "string"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_symbol_array = std::static_pointer_cast<arrow::StringArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("timestamp");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("timestamp");
        _field_index = m_file_schema->GetFieldIndex("timestamp");
        if(_field->type()->ToString() == "timestamp[us]"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_timestamp_array = std::static_pointer_cast<arrow::TimestampArray>( _chuncked_array->chunk(0));
        }
    }
    iter = m_field_set.find("exchange");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("exchange");
        _field_index = m_file_schema->GetFieldIndex("exchange");
        if(_field->type()->ToString() == "string"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chuncked_array));
            m_exchange_array = std::static_pointer_cast<arrow::StringArray>( _chuncked_array->chunk(0));
        }
    }
};

void TiQuoteSnapshotReader::SetQrySymbol(std::list<std::string> symbol_list){
    m_symbol_set.clear();
    auto iter = symbol_list.begin();
    for (; iter != symbol_list.end(); iter++)
    {
        m_symbol_set.insert(*iter);
    }
};

void TiQuoteSnapshotReader::SetTime(int hour, int min, int sec, int ms){
    if(!m_time_array){
        return;
    }
    int _time_tag = hour * 10000000 + min * 100000 + sec* 1000 + ms;
    // 二分法查找index
    int64_t first = 0, last = m_time_array->length() - 1, mid = 0;
	int64_t counter = 0;
	while(first <= last)
	{
		counter ++;
		mid = (first + last) / 2;           //确定中间元素	
		if(m_time_array->GetView(mid) > _time_tag)
		{
			last = mid - 1;                 //mid已经交换过了,last往前移一位
		}
		else if(m_time_array->GetView(mid) < _time_tag)
		{
			first = mid + 1;                //mid已经交换过了,first往后移一位
		}	
		else //判断是否相等
		{
			break;
		}
	}
    nIndex = mid;
    /*
    for (nIndex = 0; nIndex < m_table->num_rows(); nIndex ++)
    {
        if (_time_tag < m_time_array->GetView(nIndex)){
            break;
        }
    }
    */
    //printf("SetTime: %d, %ld\n", _time_tag, nIndex);
}; 

int64_t TiQuoteSnapshotReader::GetTime(){
    if (!m_time_array)
    {
        return 0;
    }
    if(nIndex >= m_time_array->length()){
        return 0;
    }
    if (!m_symbol_set.empty())
    {
        std::set<std::string>::iterator iter = m_symbol_set.find(
            static_cast<std::string>(m_symbol_array->GetView(nIndex)));
        while(iter == m_symbol_set.end()){
            nIndex += 1;
            if(nIndex >= m_time_array->length()){
                return 0;
            }
            iter = m_symbol_set.find(
                static_cast<std::string>(m_symbol_array->GetView(nIndex)));
        }
    }
    return m_time_array->GetView(nIndex);
};

bool TiQuoteSnapshotReader::GetNext(TiQuoteSnapshotStockField* out){
    if(!out){
        return false;
    }
    if(!m_time_array){
        return false;
    }
    if(nIndex >= m_time_array->length()){
        return false;
    }
    if (!m_symbol_set.empty())
    {
        std::set<std::string>::iterator iter = m_symbol_set.find(
            static_cast<std::string>(m_symbol_array->GetView(nIndex)));
        while(iter == m_symbol_set.end()){
            nIndex += 1;
            if(nIndex >= m_time_array->length()){
                return false;
            }
            iter = m_symbol_set.find(
                static_cast<std::string>(m_symbol_array->GetView(nIndex)));
        }
    }
    if (m_date_array)
        out->date = m_date_array->GetView(nIndex);
    if (m_time_array){
        out->time = m_time_array->GetView(nIndex);
    }
    if (m_last_array)
        out->last = m_last_array->GetView(nIndex);
    if (m_volume_array)
        out->volume = m_volume_array->GetView(nIndex);
    if (m_turnover_array)
        out->turnover = m_turnover_array->GetView(nIndex);
    if (m_match_items_array)
        out->match_items = m_match_items_array->GetView(nIndex);
    if (m_interest_array)
        out->interest = m_interest_array->GetView(nIndex);
    if (m_acc_volume_array)
        out->acc_volume = m_acc_volume_array->GetView(nIndex);
    if (m_acc_turnover_array)
        out->acc_turnover = m_acc_turnover_array->GetView(nIndex);
    if (m_high_array)
        out->high = m_high_array->GetView(nIndex);
    if (m_low_array)
        out->low = m_low_array->GetView(nIndex);
    if (m_open_array)
        out->open = m_open_array->GetView(nIndex);
    if (m_pre_close_array)
        out->pre_close = m_pre_close_array->GetView(nIndex);
    if (m_ask_price_array[0]){
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            out->ask_price[i] = m_ask_price_array[i]->GetView(nIndex);
        }
    }
    if (m_bid_price_array[0]){
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            out->bid_price[i] = m_bid_price_array[i]->GetView(nIndex);
        }
    }
    if (m_ask_order_num_array[0]){
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            out->ask_order_num[i] = m_ask_order_num_array[i]->GetView(nIndex);
        }
    }
    if (m_bid_order_num_array[0]){
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            out->bid_order_num[i] = m_bid_order_num_array[i]->GetView(nIndex);
        }
    }
    if (m_ask_volume_array[0]){
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            out->ask_volume[i] = m_ask_volume_array[i]->GetView(nIndex);
        }
    }
    if (m_bid_volume_array[0]){
        for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
        {
            out->bid_volume[i] = m_bid_volume_array[i]->GetView(nIndex);
        }
    }
    if (m_wavg_ask_array)
        out->wavg_ask = m_wavg_ask_array->GetView(nIndex);
    if (m_wavg_bid_array)
        out->wavg_bid = m_wavg_bid_array->GetView(nIndex);
    if (m_total_ask_qty_array)
        out->total_ask_qty = m_total_ask_qty_array->GetView(nIndex);
    if (m_total_bid_qty_array)
        out->total_bid_qty = m_total_bid_qty_array->GetView(nIndex);
    if (m_symbol_array)
        strcpy(out->symbol, static_cast<std::string>(m_symbol_array->GetView(nIndex)).c_str());
    if (m_timestamp_array)
        out->timestamp = m_timestamp_array->GetView(nIndex)/1000 - (8 * 3600 * 1000)  ;  //东八区，转成毫秒
        //这个转换太过耗时
        //datetime::get_format_timestamp_ms(out->timestamp, out->time_str, TI_TIME_STR_LEN);
    if (m_exchange_array)
        strcpy(out->exchange, static_cast<std::string>(m_exchange_array->GetView(nIndex)).c_str());

    nIndex ++;
    return true;
}