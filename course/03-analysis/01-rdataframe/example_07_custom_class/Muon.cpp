#include "Muon.hpp"
#include <cmath>

float analysis::invariant_mass(const std::vector<analysis::Muon> &muons) {

  float x_sum = 0.;
  float y_sum = 0.;
  float z_sum = 0.;
  float e_sum = 0.;

  const auto n_muons = muons.size();

  for (std::size_t i = 0u; i < n_muons; ++i) {
    // Convert to (e, x, y, z) coordinate system and update sums
    const auto x = muons[i].pt * std::cos(muons[i].phi);
    x_sum += x;
    const auto y = muons[i].pt * std::sin(muons[i].phi);
    y_sum += y;
    const auto z = muons[i].pt * std::sinh(muons[i].eta);
    z_sum += z;
    const auto e =
        std::sqrt(x * x + y * y + z * z + muons[i].mass * muons[i].mass);
    e_sum += e;
  }

  // Return invariant mass with (+, -, -, -) metric
  return std::sqrt(e_sum * e_sum - x_sum * x_sum - y_sum * y_sum -
                   z_sum * z_sum);
}
