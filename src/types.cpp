#include "types.h"

double Portfolio::get_total_value() const {
  double total_value = cash;
  for (const auto& position : positions) {
    total_value += position.quantity * position.average_price;
  }
  return total_value;
}
