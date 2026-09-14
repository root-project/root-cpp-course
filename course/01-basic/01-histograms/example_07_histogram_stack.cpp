#include <TCanvas.h>
#include <TH1.h>
#include <THStack.h>

#include <random>

void example_07_histogram_stack() {
  // We create three different gaussian distributions so the histograms
  // will look different in the final plot
  std::mt19937 rng{std::random_device{}()};

  constexpr auto n_fills{10000};

  constexpr auto n_histos{3};
  constexpr auto n_bins{100};
  std::array<TH1D, 3> histos;
  std::array colours{kRed, kGreen, kBlue};
  std::array distributions{std::normal_distribution<double>{8, 1},
                           std::normal_distribution<double>{7, 1},
                           std::normal_distribution<double>{6, 1}};

  THStack stack{"stack", "Stacked histograms;x;counts"};
  for (auto i = 0; i < n_histos; i++) {
    auto h_label = "h_" + std::to_string(i);
    histos[i] = TH1D{h_label.c_str(), h_label.c_str(), 100, 0, 15};
    auto &h = histos[i];
    for (auto j = 0; j < n_fills; j++)
      h.Fill(distributions[i](rng));
    h.SetFillColor(colours[i]);
    // THStack::Add takes a 'TH1 *' argument, we shouldn't assume it takes
    // ownership of the histograms. If they had been created in another
    // function, the stack would not keep them alive and we would see an
    // empty canvas later.
    stack.Add(&h);
  }

  TCanvas c{"c", "c", 800, 600};
  stack.Draw();
  c.SaveAs("example_07_histogram_stack.png");
}

int main() {
  example_07_histogram_stack();
  return 0;
}
