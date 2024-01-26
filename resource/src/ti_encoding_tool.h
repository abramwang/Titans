#ifndef _TI_ENCODING_TOOL_
#define _TI_ENCODING_TOOL_

#include <string>
#include <iostream>
#include <iconv.h>

namespace TiEncodingTool {
    // 类的成员和方法

    inline std::string GbkToUtf8(const std::string& gbkStr) {
        // 创建转换句柄
        iconv_t cd = iconv_open("UTF-8", "GBK");
        if (cd == (iconv_t)(-1)) {
            std::cerr << "Failed to open iconv" << std::endl;
            return "";
        }

        // 输入参数
        const char* inbuf = gbkStr.c_str();
        size_t inbytesleft = gbkStr.length();

        // 输出参数
        std::string utf8Str;
        const size_t kMaxOutBytes = inbytesleft * 2; // 预留足够的空间
        char outbuf[kMaxOutBytes];
        size_t outbytesleft = kMaxOutBytes;
        char* outptr = outbuf;

        // 进行转换
        size_t result = iconv(cd, (char**)&inbuf, &inbytesleft, &outptr, &outbytesleft);
        if (result == (size_t)(-1)) {
            std::cerr << "Failed to convert" << std::endl;
            iconv_close(cd);
            return "";
        }

        // 关闭转换句柄
        iconv_close(cd);

        // 构造UTF-8字符串
        utf8Str.assign(outbuf, kMaxOutBytes - outbytesleft);

        return utf8Str;
    }
};

#endif