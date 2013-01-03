/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef AU_ExecesiveTimeAlarm_H_
#define AU_ExecesiveTimeAlarm_H_

#include "au/statistics/Cronometer.h"
#include <string>

namespace au {
class ExecesiveTimeAlarm {
public:

  ExecesiveTimeAlarm(int log_channel, const std::string& title, double max_time = 0.5);
  ~ExecesiveTimeAlarm();

private:

  Cronometer cronometer_;
  std::string title_;
  double max_time_;
  int log_channel_;
};
}

#endif  // ifndef AU_ExecesiveTimeAlarm_H_