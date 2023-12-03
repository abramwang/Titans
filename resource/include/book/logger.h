// Copyright (c) 2018 - 2020 Quantstage, Inc.
// All rights reserved.
// See the file license.txt for licensing information.
#pragma once

#include <exception>
#include <string>
namespace ti_book{ namespace book {
/// @brief Interface to allow application to control error logging
class Logger
{
public:
  virtual void log_exception(const std::string & context, const std::exception& ex) = 0;
  virtual void log_message(const std::string & message) = 0;
};

}}
