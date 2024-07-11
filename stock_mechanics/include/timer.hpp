#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

namespace ProjectStockMarket {

class Timer {
 public:
  Timer();
  ~Timer();

  // Starts the timer
  void start();

  // Stops the timer
  void stop();

  // Sets the callback function to be called every second
  void setCallback(std::function<void()> callback);

 private:
  std::atomic<bool> running;
  std::thread timerThread;
  std::function<void()> callback;

  // Function that increments the timer
  void run();
};

}  // namespace ProjectStockMarket
