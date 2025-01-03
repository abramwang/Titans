#include <cstddef>
#include <iostream>
#include <memory>

#include "ti_df_quote_client.h"

#include "quote_demo_lv2.h"

int main(int argc, char *argv[]) {
    TiDfQuoteClient client;
    client.Run();

    return 0;
    // 行情lv2相关demo
    QuoteDemoLv2 quote_demo_lv2;
    quote_demo_lv2.Run();

    return 0;
}
