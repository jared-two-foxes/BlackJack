
#include <blackjack/timer.h>

void wait(timer_t& t, float secondsToWait) {
  time_t now = std::time(nullptr);
  struct tm buf = *std::localtime(&now);
  buf.tm_sec += secondsToWait;
  t.expiresAt = std::mktime(&buf);
}

bool complete(timer_t& t) {
  std::time_t now = std::time(nullptr);
  return (std::difftime(now, t.expiresAt) > 0);
}
