#pragma once

#include <chrono>
#include <string>

namespace ProjectStockMarket {

using time_point = std::chrono::system_clock::time_point;

struct Record {
  time_point dateTime;
  int price;

  Record(time_point p_dateTime, int p_price)
      : dateTime(p_dateTime), price(p_price) {}

  bool operator==(const Record& other) const {
    return dateTime == other.dateTime && price == other.price;
  }
};

}  // namespace ProjectStockMarket
