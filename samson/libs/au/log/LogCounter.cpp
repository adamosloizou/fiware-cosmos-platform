
#include "au/log/LogCounter.h"  // Own interface

namespace au {
void LogCounter::Process(LogPointer log) {
  size_t size = log->SerialitzationSize();

  rate_.Push(size);     // Global rate counter

  au::rate::Rate *rate = rates.findOrCreate(log->channel());
  rate->Push(size);     // Individual rate counter
}

std::string LogCounter::str(int c) const {
  au::rate::Rate *rate = rates.findInMap(c);

  if (!rate) {
    return "";
  } else {
    return au::str("%s %s"
                   , au::str(rate->hits()).c_str()
                   , au::str(rate->size(), "B").c_str()
                   );
  }
}

std::string LogCounter::str_rate(int c) const {
  au::rate::Rate *rate = rates.findInMap(c);

  if (!rate) {
    return "";
  } else {
    return au::str("%s", au::str(rate->hits()).c_str());
  }
}

size_t LogCounter::logs() const {
  return rate_.hits();
}

size_t LogCounter::size() const {
  return rate_.size();
}

size_t LogCounter::rate() const {
  return rate_.rate();
}
}