#pragma once
#include <string>

namespace ProjectStockMarket {

struct Record {
  int dateTime;
  int price;

  Record(int dt, int pr) : dateTime(dt), price(pr) {}
  Record(int pr) : dateTime(0), price(pr) {}
  Record() : dateTime(0), price(0) {}  // Default constructor with current time
};

}  // namespace ProjectStockMarket
