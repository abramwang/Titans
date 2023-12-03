#include "ti_quote_parquet_reader.h"

void TiQuoteParquetReader::init_read_array_work(uv_work_t* req){
    ReadArrayWorkReqInfo* req_info = (ReadArrayWorkReqInfo*)req->data;
    std::unique_ptr<parquet::arrow::FileReader> _reader;
    std::shared_ptr<arrow::Schema> _schema;
    std::shared_ptr<arrow::io::ReadableFile> _infile;
    PARQUET_ASSIGN_OR_THROW(
        _infile,
        arrow::io::ReadableFile::Open(req_info->m_file_path.c_str(),
                                    arrow::default_memory_pool()));
    PARQUET_THROW_NOT_OK(
        parquet::arrow::OpenFile(_infile, arrow::default_memory_pool(), &_reader));
    PARQUET_THROW_NOT_OK(_reader->GetSchema(&_schema));
    
    int _field_index = _schema->GetFieldIndex(req_info->m_field); 
    std::shared_ptr<arrow::ChunkedArray> _chuncked_array;
    PARQUET_THROW_NOT_OK(_reader->ReadColumn(_field_index, &_chuncked_array));
    req_info->m_array = _chuncked_array->chunk(0);
     
};

void TiQuoteParquetReader::after_read_array_work(uv_work_t* req, int status){
    ReadArrayWorkReqInfo* req_info = (ReadArrayWorkReqInfo*)req->data;
    req_info->m_reader->m_array_map[req_info->m_field] = req_info->m_array;

    delete req_info;
    req->data = 0;
    free(req);
};

TiQuoteParquetReader::TiQuoteParquetReader(TI_QUOTE_DATA_TYPE dataType)
{
    m_DataType = dataType;
    uv_loop_init(&m_read_loop);
}

void TiQuoteParquetReader::loadAllArray(std::string file_path)
{
    std::unique_ptr<parquet::arrow::FileReader> _reader;
    std::shared_ptr<arrow::Schema> _schema;
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(
        infile,
        arrow::io::ReadableFile::Open(file_path.c_str(),
                                    arrow::default_memory_pool()));
    PARQUET_THROW_NOT_OK(
        parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &_reader));
    PARQUET_THROW_NOT_OK(_reader->GetSchema(&_schema));

    std::vector<std::string> _field_names_vec = _schema->field_names();

    for (size_t i = 0; i < _field_names_vec.size(); i++)
    {
        ReadArrayWorkReqInfo* _reqInfo = new ReadArrayWorkReqInfo;
        _reqInfo->m_file_path = file_path;
        _reqInfo->m_field     = _field_names_vec[i];
        _reqInfo->m_reader    = this;

        uv_work_t* _req = (uv_work_t*)malloc(sizeof(uv_work_t));
        memset(_req, 0, sizeof(uv_work_t));
        _req->data = _reqInfo;

        uv_queue_work(&m_read_loop, _req, TiQuoteParquetReader::init_read_array_work, TiQuoteParquetReader::after_read_array_work);
    }

    uv_run(&m_read_loop, UV_RUN_DEFAULT);
};

TI_QUOTE_DATA_TYPE TiQuoteParquetReader::GetDataType(){
    return m_DataType;
};

int64_t TiQuoteParquetReader::GetTime(){
    return 0;
};