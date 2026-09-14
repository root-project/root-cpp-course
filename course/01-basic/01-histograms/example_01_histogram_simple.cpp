#include <TH1D.h>

#include <iostream>

void example_01_histogram_simple() {
  // Parameter meaning: name, title, number of bins, minimum x, maximum x
  TH1D h{"h", "h", 10, -5, 5};
  h.FillRandom("gaus");

  std::cout << "Histogram (" << h.GetEntries()
            << " entries): mean=" << h.GetMean() << " +- " << h.GetStdDev()
            << "\n";
}

int main() {
  example_01_histogram_simple();
  return 0;
}
