// Copyright (c) 2018 - 2020 Quantstage, Inc.
// All rights reserved.
// See the file license.txt for licensing information.
#pragma once

#include "order_book.h"

namespace ti_book{ namespace book {

/// @brief listener of depth events.  Implement to build an aggregate depth 
/// feed.
template <class OrderBook >
class DepthListener {
public:
  /// @brief callback for change in tracked aggregated depth
  virtual void on_depth_change(
      const OrderBook* book,
      const typename OrderBook::DepthTracker* depth) = 0;
};

} }

