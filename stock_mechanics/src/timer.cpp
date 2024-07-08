#include "timer.hpp"

namespace ProjectStockMarket {

Timer::Timer() : running(false), currentTime(0) {}

Timer::~Timer() {
  stop();
}

void Timer::start() {
  running = true;
  timerThread = std::thread(&Timer::run, this);
}

void Timer::stop() {
  running = false;
  if (timerThread.joinable()) {
    timerThread.join();
  }
}

int Timer::getCurrentTime() const {
  return currentTime.load();
}

void Timer::setCallback(std::function<void()> callback) {
  this->callback = callback;
}

void Timer::run() {
  while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ++currentTime;

    if (callback) {
      callback();
    }
  }
}

}  // namespace ProjectStockMarket
