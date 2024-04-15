#ifndef _TI_NUMBER_TOOL_
#define _TI_NUMBER_TOOL_
#include <limits>

namespace TiNumberTool {

    // 判断是否是无穷大, 无穷小, NaN 等特殊值，如果是则返回0，否则返回原值
    template <typename T>
    T checkSpecialValue(T value) {
        if (std::numeric_limits<T>::max() == value) {
            return 0;
        }
        if (std::numeric_limits<T>::min() == value) {
            return 0;
        }
        if (std::numeric_limits<T>::has_infinity && std::numeric_limits<T>::infinity() == value) {
            return 0;
        }
        if (std::numeric_limits<T>::has_infinity && -std::numeric_limits<T>::infinity() == value) {
            return 0;
        }
        if (std::numeric_limits<T>::has_quiet_NaN && std::numeric_limits<T>::quiet_NaN() == value) {
            return 0;
        }
        if (std::numeric_limits<T>::has_signaling_NaN && std::numeric_limits<T>::signaling_NaN() == value) {
            return 0;
        }
        return value;
    }


};

#endif