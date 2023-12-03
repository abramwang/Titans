#include "ti_quote_reader.h"
#include <map>


void TiQuoteReader::init_reader_work(uv_work_t* req){
    ReadWorkReqInfo* req_info = (ReadWorkReqInfo*)req->data;
    if(req_info->type == TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK)
    {
        req_info->m_file_reader = new TiQuoteSnapshotReader(req_info->szParquetPath, NULL, 0);
        ((TiQuoteSnapshotReader*)req_info->m_file_reader)->SetQrySymbol(req_info->m_symbol_list);
    }
    if(req_info->type == TI_QUOTE_DATA_TYPE_ORDER)
    {
        req_info->m_file_reader = new TiQuoteOrderReader(req_info->szParquetPath, NULL, 0);
        ((TiQuoteOrderReader*)req_info->m_file_reader)->SetQrySymbol(req_info->m_symbol_list);
    }
    if(req_info->type  == TI_QUOTE_DATA_TYPE_MATCH)
    {
        req_info->m_file_reader = new TiQuoteMatchReader(req_info->szParquetPath, NULL, 0);
        ((TiQuoteMatchReader*)req_info->m_file_reader)->SetQrySymbol(req_info->m_symbol_list);
    }
};
void TiQuoteReader::after_reader_work(uv_work_t* req, int status){
    ReadWorkReqInfo* req_info = (ReadWorkReqInfo*)req->data;
    printf("after_reader_work loaded %s\n", req_info->szParquetPath);

    req_info->m_quote_reader->m_readers.push_back(std::shared_ptr<TiQuoteParquetReader>(req_info->m_file_reader));

    delete req_info;
    req->data = 0;
    free(req);
};


TiQuoteReader::TiQuoteReader(const char* rootPath){
    uv_loop_init(&m_loop);
    szRootPath = rootPath;
}

void TiQuoteReader::ReadQuoteData(int dayNum, TI_ExchangeType ex, TI_QUOTE_DATA_TYPE datatype_list[], size_t datatype_len)
{
    m_readers.clear();
    std::list<std::string> symbol_list;
    
    if(!strcmp(ex, "ALL")){
        TI_ExchangeType ex_list[2] = {"SH", "SZ"};
        for (size_t j = 0; j < 2; j++)
        {
            for (size_t i = 0; i < datatype_len; i++)
            {
                ReadWorkReqInfo* req_info = new ReadWorkReqInfo;
                req_info->type = datatype_list[i];
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK)
                    sprintf(req_info->szParquetPath, "%s/ti.snapshot.%s.%d.parquet", szRootPath.c_str(), ex_list[j], dayNum);
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_ORDER)
                    sprintf(req_info->szParquetPath, "%s/ti.order.%s.%d.parquet", szRootPath.c_str(), ex_list[j], dayNum);
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_MATCH)
                    sprintf(req_info->szParquetPath, "%s/ti.matches.%s.%d.parquet", szRootPath.c_str(), ex_list[j], dayNum);
                req_info->m_symbol_list = symbol_list;
                req_info->m_file_reader = 0;
                req_info->m_quote_reader = this;

                uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
                memset(req, 0, sizeof(uv_work_t));
                req->data = req_info;

                uv_queue_work(&m_loop, req, TiQuoteReader::init_reader_work, TiQuoteReader::after_reader_work);
            }
        }
    }else if(!strcmp(ex, "SH") || !strcmp(ex, "SZ")){
        for (size_t i = 0; i < datatype_len; i++)
        {
            ReadWorkReqInfo* req_info = new ReadWorkReqInfo;
            req_info->type = datatype_list[i];
            if(datatype_list[i] == TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK)
                sprintf(req_info->szParquetPath, "%s/ti.snapshot.%s.%d.parquet", szRootPath.c_str(), ex, dayNum);
            if(datatype_list[i] == TI_QUOTE_DATA_TYPE_ORDER)
                sprintf(req_info->szParquetPath, "%s/ti.order.%s.%d.parquet", szRootPath.c_str(), ex, dayNum);
            if(datatype_list[i] == TI_QUOTE_DATA_TYPE_MATCH)
                sprintf(req_info->szParquetPath, "%s/ti.matches.%s.%d.parquet", szRootPath.c_str(), ex, dayNum);
            req_info->m_symbol_list = symbol_list;
            req_info->m_file_reader = 0;
            req_info->m_quote_reader = this;

            uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
            memset(req, 0, sizeof(uv_work_t));
            req->data = req_info;

            uv_queue_work(&m_loop, req, TiQuoteReader::init_reader_work, TiQuoteReader::after_reader_work);
        }

    }
    uv_run(&m_loop, UV_RUN_DEFAULT);
    printf("work over\n");
};
    
void TiQuoteReader::ReadQuoteData(int dayNum, TiSymbolInfo symbol_list[], size_t symbol_len, TI_QUOTE_DATA_TYPE datatype_list[], size_t datatype_len)
{
    m_readers.clear();
    std::map<std::string, std::list<std::string>> _symbol_map;  //exchange, sub_symbol;
    std::map<std::string, std::list<std::string>>::iterator _symbol_map_iter = _symbol_map.end();
    for (size_t i = 0; i < symbol_len; i++)
    {
        _symbol_map_iter = _symbol_map.find(symbol_list[i].exchange);
        if (_symbol_map_iter == _symbol_map.end())
        {
            _symbol_map[symbol_list[i].exchange] = std::list<std::string>();
        }
        _symbol_map[symbol_list[i].exchange].push_back(symbol_list[i].symbol);
    }
    
    for (_symbol_map_iter = _symbol_map.begin(); _symbol_map_iter != _symbol_map.end(); _symbol_map_iter++)
    {
        // 过滤只加载上交所和深交所数据
        if(_symbol_map_iter->first == "SH" || _symbol_map_iter->first == "SZ"){
            for (size_t i = 0; i < datatype_len; i++)
            {
                ReadWorkReqInfo* req_info = new ReadWorkReqInfo;
                req_info->type = datatype_list[i];
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK)
                    sprintf(req_info->szParquetPath, "%s/ti.snapshot.%s.%d.parquet", szRootPath.c_str(), _symbol_map_iter->first.c_str(), dayNum);
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_ORDER)
                    sprintf(req_info->szParquetPath, "%s/ti.order.%s.%d.parquet", szRootPath.c_str(), _symbol_map_iter->first.c_str(), dayNum);
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_MATCH)
                    sprintf(req_info->szParquetPath, "%s/ti.matches.%s.%d.parquet", szRootPath.c_str(), _symbol_map_iter->first.c_str(), dayNum);
                req_info->m_symbol_list = _symbol_map_iter->second;
                req_info->m_file_reader = 0;
                req_info->m_quote_reader = this;

                uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
                memset(req, 0, sizeof(uv_work_t));
                req->data = req_info;

                uv_queue_work(&m_loop, req, TiQuoteReader::init_reader_work, TiQuoteReader::after_reader_work);
            }
            
            /*
            char** _symbol_list = new char*[_symbol_map_iter->second.size()];
            for (size_t i = 0; i < _symbol_map_iter->second.size(); i++)
            {
                _symbol_list[i] = (char*)_symbol_map_iter->second[i].c_str();
            }
            for (size_t i = 0; i < datatype_len; i++)
            {
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_ORDER)
                {
                    sprintf(_parquet_path, "%s/ti.order.%s.%d.parquet", szRootPath.c_str(), _symbol_map_iter->first.c_str(), dayNum);
                    TiQuoteOrderReader* _reader = new TiQuoteOrderReader(_parquet_path, NULL, 0);
                    _reader->SetQrySymbol(_symbol_list, _symbol_map_iter->second.size());
                    m_readers.push_back(std::shared_ptr<TiQuoteParquetReader>(_reader));
                }
                if(datatype_list[i] == TI_QUOTE_DATA_TYPE_MATCH)
                {
                    sprintf(_parquet_path, "%s/ti.matches.%s.%d.parquet", szRootPath.c_str(), _symbol_map_iter->first.c_str(), dayNum);
                    TiQuoteMatchReader* _reader = new TiQuoteMatchReader(_parquet_path, NULL, 0);
                    _reader->SetQrySymbol(_symbol_list, _symbol_map_iter->second.size());
                    m_readers.push_back(std::shared_ptr<TiQuoteParquetReader>(_reader));
                }
                printf("loaded '%s'\n", _parquet_path);
            }
            */
        }
    }
    uv_run(&m_loop, UV_RUN_DEFAULT);
    printf("work over\n");
};

bool TiQuoteReader::GetNext(TiQuoteData* pData){
    auto iter = m_readers.begin();
    if(iter == m_readers.end()){
        return false;
    }
    for (auto i = m_readers.begin(); i != m_readers.end(); ++i)
    {
        //i->get()->SetTime(9,15,0,0);
        if((*i)->GetTime() < (*iter)->GetTime()){
            iter = i;
        }
    }
    switch ((*iter)->GetDataType())
    {
    case TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK:{
        pData->type = TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK;
        return std::static_pointer_cast<TiQuoteSnapshotReader>(*iter)->GetNext(&pData->m_snapshot_stock);
        break;
    }  
    case TI_QUOTE_DATA_TYPE_MATCH:{
        pData->type = TI_QUOTE_DATA_TYPE_MATCH;
        return std::static_pointer_cast<TiQuoteMatchReader>(*iter)->GetNext(&pData->m_matches);
        break;
    }   
    case TI_QUOTE_DATA_TYPE_ORDER:{
        pData->type = TI_QUOTE_DATA_TYPE_ORDER;
        return std::static_pointer_cast<TiQuoteOrderReader>(*iter)->GetNext(&pData->m_order);
        break;
    }
    default:
        break;
    }
    return false;
}; 