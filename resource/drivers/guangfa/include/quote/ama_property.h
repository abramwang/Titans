#ifndef AMA_PROPERTY_H_
#define AMA_PROPERTY_H_
#include <stdint.h>
#include "ama_export.h"

namespace amd { namespace ama {
class PropertyImpl;
class AMA_EXPORT AMDProperty
{
public:
    /**
    * @brief 构造函数
    */
    AMDProperty();

    /**
    * @brief 析构函数
    */
    ~AMDProperty();

    // AMDProperty(const AMDProperty&) = delete;
    // AMDProperty& operator=(const AMDProperty&) = delete;
public:
    void SetValue(const char* key, const char* value) ;

    void SetValue(const char* key, bool value) ;

    void SetValue(const char* key, char value) ;

    void SetValue(const char* key, uint8_t value) ;

    void SetValue(const char* key, int8_t value) ;

    void SetValue(const char* key, uint16_t value) ;

    void SetValue(const char* key, int16_t value) ;

    void SetValue(const char* key, uint32_t value) ;

    void SetValue(const char* key, int32_t value) ;

    void SetValue(const char* key, uint64_t value) ;

    void SetValue(const char* key, int64_t value) ;

    const char* GetValue(const char* key, const char* default_value) const;

    bool GetValue(const char* key, bool default_value) const;

    char GetValue(const char* key, char default_value) const;

    uint8_t GetValue(const char* key, uint8_t default_value) const;

    int8_t GetValue(const char* key, int8_t default_value) const;

    uint16_t GetValue(const char* key, uint16_t default_value) const;

    int16_t GetValue(const char* key, int16_t default_value) const;

    uint32_t GetValue(const char* key, uint32_t default_value) const;

    int32_t GetValue(const char* key, int32_t default_value) const;

    uint64_t GetValue(const char* key, uint64_t default_value) const;

    int64_t GetValue(const char* key, int64_t default_value) const;

    bool HasKey(const char* key) const;
private:
    amd::ama::PropertyImpl* impl_;
};

}
}

#endif