#include <cstddef>
#include <iostream>
#include <memory>

#include "ti_df_quote_client.h"
#include "ti_df_trader_client.h"


int main(int argc, char *argv[]) {
    TiDfQuoteClient client;
    client.connect();

    return 0;
}
