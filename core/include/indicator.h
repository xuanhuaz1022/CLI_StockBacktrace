#ifndef INDICATOR_H
#define INDICATOR_H
#include "types.h"

std::vector<double> simple_moving_average(const std::vector<Bar> &bars, size_t period);
std::vector<double> calculate_ema(const std::vector<Bar> &bars, size_t period);

#endif
