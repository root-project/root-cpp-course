#include "exercise_01_split_impl_solution_helpers.hpp"

#include <random>
#include <vector>

inline std::mt19937 rng{std::random_device{}()};
inline std::normal_distribution<double> gaus(0., 1.);

double get_normal_global() { return gaus(rng); }

inline std::vector<std::mt19937> generators;
inline std::vector<std::normal_distribution<double>> gaussians;

void reinitialize_generators(unsigned int n_slots) {
  std::random_device rd;
  generators.resize(n_slots);
  for (auto &gen : generators)
    gen.seed(rd());
  gaussians.resize(n_slots, std::normal_distribution<double>(0., 1.));
}

double get_normal_per_slot(unsigned int slot) {
  return gaussians[slot](generators[slot]);
}

double get_normal_per_slot_and_entry(unsigned int slot,
                                     unsigned long long entry) {
  gaussians[slot].reset();
  generators[slot].seed(entry);
  return gaussians[slot](generators[slot]);
}
