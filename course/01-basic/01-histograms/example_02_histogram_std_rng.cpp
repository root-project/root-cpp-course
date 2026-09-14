#include <iostream>
#include <random>

#include <TH1D.h>

void example_02_histogram_std_rng() {
  TH1D h{"h", "h", 10, -5, 5};

  // Standard C++ RNG engine, seeded from a random device
  std::mt19937 rng{std::random_device{}()};

  std::normal_distribution<double> gaus{0, 1};

  constexpr auto nFills{5000};

  for (int i = 0; i < nFills; ++i)
    h.Fill(gaus(rng));

  std::cout << "Histogram (" << h.GetEntries()
            << " entries): mean=" << h.GetMean() << " +- " << h.GetStdDev()
            << "\n";
}
int main() {
  example_02_histogram_std_rng();
  return 0;
}
