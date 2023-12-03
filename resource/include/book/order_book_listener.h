// Copyright (c) 2018 - 2020 Quantstage, Inc.
// All rights reserved.
// See the file license.txt for licensing information.
#pragma once

#include "order_book.h"

namespace ti_book{ namespace book {

/// @brief generic listener of order book events
template <class OrderBook >
class OrderBookListener {
public:
  /// @brief callback for change anywhere in order book
  virtual void on_order_book_change(const OrderBook* book) = 0;
};

} }
