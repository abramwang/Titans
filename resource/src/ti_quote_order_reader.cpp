#include "ti_quote_order_reader.h"
#include "datetime.h"
#include <iostream>

TiQuoteOrderReader::TiQuoteOrderReader(const char* path, char* field_list[], size_t len)
    : TiQuoteParquetReader(TI_QUOTE_DATA_TYPE_ORDER)
{
    std::cout << "Reading first column of " << path << std::endl;
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(
        infile,
        arrow::io::ReadableFile::Open(path,
                                    arrow::default_memory_pool()));
    PARQUET_THROW_NOT_OK(
        parquet::arrow::OpenFile(infile, arrow::default_memory_pool()));
    
    PARQUET_THROW_NOT_OK(m_reader->GetSchema(&m_file_schema));

    memset(&m_data, 0, sizeof(TiQuoteMatchesField));
    nIndex = 0;

    m_date_array          = 0;
    m_time_array          = 0;
    m_seq_array           = 0;
    m_order_orino_array   = 0;
    m_order_type_array    = 0;
    m_function_code_array = 0;
    m_price_array         = 0;
    m_volume_array        = 0;
    m_channel_array       = 0;
    m_symbol_array        = 0;
    m_timestamp_array     = 0;
    m_exchange_array      = 0;

    initField(field_list, len);
    loadArray(path);
}

void TiQuoteOrderReader::initField(char* field_list[], size_t len){
    for(size_t i = 0; i < len; i++){
        std::shared_ptr<arrow::Field> _field = m_file_schema->GetFieldByName(field_list[i]);
        if(_field){
            m_field_set.insert(field_list[i]);
        }
    }
};

void TiQuoteOrderReader::loadArray(std::string file_path){
    // 全部加载table
    if((m_field_set.size() >= (m_file_schema->num_fields()*0.6))
        || m_field_set.empty()
    ){
        //std::shared_ptr<arrow::Table> table;
        //PARQUET_THROW_NOT_OK(m_reader->ReadTable(&table));
        
        loadAllArray(file_path);
        
        if(m_file_schema->GetFieldByName("date"))
            m_date_array          = std::static_pointer_cast<arrow::Int64Array>(m_array_map["date"]);
        if(m_file_schema->GetFieldByName("time"))
            m_time_array          = std::static_pointer_cast<arrow::Int64Array>(m_array_map["time"]);
        if(m_file_schema->GetFieldByName("seq"))
            m_seq_array           = std::static_pointer_cast<arrow::Int64Array>(m_array_map["seq"]);
        if(m_file_schema->GetFieldByName("order_orino"))
            m_order_orino_array   = std::static_pointer_cast<arrow::Int64Array>(m_array_map["order_orino"]);
        if(m_file_schema->GetFieldByName("order_type"))
            m_order_type_array    = std::static_pointer_cast<arrow::StringArray>(m_array_map["order_type"]);
        if(m_file_schema->GetFieldByName("function_code"))
            m_function_code_array = std::static_pointer_cast<arrow::StringArray>(m_array_map["function_code"]);
        if(m_file_schema->GetFieldByName("price"))
            m_price_array         = std::static_pointer_cast<arrow::DoubleArray>(m_array_map["price"]);
        if(m_file_schema->GetFieldByName("volume"))
            m_volume_array        = std::static_pointer_cast<arrow::Int64Array>(m_array_map["volume"]);
        if(m_file_schema->GetFieldByName("channel"))
            m_channel_array       = std::static_pointer_cast<arrow::Int64Array>(m_array_map["channel"]);
        if(m_file_schema->GetFieldByName("symbol"))
            m_symbol_array        = std::static_pointer_cast<arrow::StringArray>(m_array_map["symbol"]);
        if(m_file_schema->GetFieldByName("timestamp"))
            m_timestamp_array     = std::static_pointer_cast<arrow::TimestampArray>(m_array_map["timestamp"]);
        if(m_file_schema->GetFieldByName("exchange"))
            m_exchange_array      = std::static_pointer_cast<arrow::StringArray>(m_array_map["exchange"]);

        return;
    }
    std::shared_ptr<arrow::ChunkedArray> _chunked_array;
    std::shared_ptr<arrow::Field> _field;
    int _field_index = 0; 
    std::set<std::string>::iterator iter = m_field_set.begin();
    iter = m_field_set.find("date");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("date");
        _field_index = m_file_schema->GetFieldIndex("date");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_date_array = std::static_pointer_cast<arrow::Int64Array>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("time");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("time");
        _field_index = m_file_schema->GetFieldIndex("time");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_time_array = std::static_pointer_cast<arrow::Int64Array>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("seq");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("seq");
        _field_index = m_file_schema->GetFieldIndex("seq");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_seq_array = std::static_pointer_cast<arrow::Int64Array>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("order_orino");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("order_orino");
        _field_index = m_file_schema->GetFieldIndex("order_orino");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_order_orino_array = std::static_pointer_cast<arrow::Int64Array>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("order_type");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("order_type");
        _field_index = m_file_schema->GetFieldIndex("order_type");
        if(_field->type()->ToString() == "string"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_order_type_array = std::static_pointer_cast<arrow::StringArray>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("function_code");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("function_code");
        _field_index = m_file_schema->GetFieldIndex("function_code");
        if(_field->type()->ToString() == "string"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_function_code_array = std::static_pointer_cast<arrow::StringArray>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("price");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("price");
        _field_index = m_file_schema->GetFieldIndex("price");
        if(_field->type()->ToString() == "double"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_price_array = std::static_pointer_cast<arrow::DoubleArray>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("volume");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("volume");
        _field_index = m_file_schema->GetFieldIndex("volume");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_volume_array = std::static_pointer_cast<arrow::Int64Array>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("channel");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("channel");
        _field_index = m_file_schema->GetFieldIndex("channel");
        if(_field->type()->ToString() == "int64"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_channel_array = std::static_pointer_cast<arrow::Int64Array>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("symbol");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("symbol");
        _field_index = m_file_schema->GetFieldIndex("symbol");
        if(_field->type()->ToString() == "string"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_symbol_array = std::static_pointer_cast<arrow::StringArray>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("timestamp");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("timestamp");
        _field_index = m_file_schema->GetFieldIndex("timestamp");
        if(_field->type()->ToString() == "timestamp[us]"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_timestamp_array = std::static_pointer_cast<arrow::TimestampArray>( _chunked_array->chunk(0));
        }
    }
    iter = m_field_set.find("exchange");
    if(iter != m_field_set.end())
    {
        _field       = m_file_schema->GetFieldByName("exchange");
        _field_index = m_file_schema->GetFieldIndex("exchange");
        if(_field->type()->ToString() == "string"){
            PARQUET_THROW_NOT_OK(m_reader->ReadColumn(_field_index, &_chunked_array));
            m_exchange_array = std::static_pointer_cast<arrow::StringArray>( _chunked_array->chunk(0));
        }
    }
};

void TiQuoteOrderReader::SetQrySymbol(std::list<std::string> symbol_list){
    m_symbol_set.clear();
    auto iter = symbol_list.begin();
    for (; iter != symbol_list.end(); iter++)
    {
        m_symbol_set.insert(*iter);
    }
};

void TiQuoteOrderReader::SetTime(int hour, int min, int sec, int ms){
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

int64_t TiQuoteOrderReader::GetTime(){
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

bool TiQuoteOrderReader::GetNext(TiQuoteOrderField* out){
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
    if (m_seq_array)
        out->seq = m_seq_array->GetView(nIndex);
    if (m_order_orino_array)
        out->order_orino = m_order_orino_array->GetView(nIndex);
    if (m_order_type_array)
        memcpy(&out->order_type, static_cast<std::string>(m_order_type_array->GetView(nIndex)).c_str(), 1);
    if (m_function_code_array)
        memcpy(&out->function_code, static_cast<std::string>(m_function_code_array->GetView(nIndex)).c_str(), 1);
    if (m_price_array)
        out->price = m_price_array->GetView(nIndex);
    if (m_volume_array)
        out->volume = m_volume_array->GetView(nIndex);
    if (m_channel_array)
        out->channel = m_channel_array->GetView(nIndex);
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