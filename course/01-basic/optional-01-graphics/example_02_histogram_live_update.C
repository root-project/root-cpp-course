#include <Rtypes.h> // ROOT colours and markers
#include <TCanvas.h>
#include <TH1.h>

#include <memory>

// We create a unique_ptr to a canvas at global scope to avoid losing the canvas
// object after the end of the main function
auto canvas = std::make_unique<TCanvas>("c", "c", 800, 600);

void example_02_histogram_live_update() {
  // Generate and draw the first histogram
  TH1D h_1{"h_1", "h_1", 100, 0, 10};
  h_1.SetFillColor(kGray);
  TH1D h_2{"h_2", "h_2", 100, 0, 10};
  h_2.SetMarkerStyle(kFullCross);

  h_1.Draw();
  // Draw error bars and markers with E1P
  // Overlay histogram on same canvas with same
  h_2.Draw("E1P same");

  std::mt19937 rng{std::random_device{}()};

  std::normal_distribution<double> gaus{6, 0.5};
  std::exponential_distribution<double> exp{0.5};

  constexpr auto n_fills{10000};
  constexpr auto n_update{10};
  for (auto i = 0; i < n_fills; i++) {
    auto val_gaus = gaus(rng);
    auto val_exp = exp(rng);
    h_1.Fill(val_gaus);
    h_1.Fill(val_exp);
    h_2.Fill(val_gaus);

    if (i % n_update == 0) {
      canvas->ModifiedUpdate();
    }
  }

  // We draw them once more after the update loop to ensure they will survive
  // past the end of the function
  h_1.DrawClone();
  h_2.DrawClone();
  canvas->Draw();
}
