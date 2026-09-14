#include <random>

#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>

double fit_function(double *values, double *parameters) {
  // We have to fit the exponential background plus the gaussian peak with
  // 5 parameters
  const auto &value = values[0];
  const auto &constant_bkg = parameters[0];
  const auto &rate_bkg = parameters[1];
  const auto &constant_sig = parameters[2];
  const auto &mean_sig = parameters[3];
  const auto &sigma_sig = parameters[4];
  const auto bkg = constant_bkg * std::exp(-rate_bkg * value);
  const auto sig = constant_sig *
                   std::exp(-0.5 * std::pow((value - mean_sig) / sigma_sig, 2));
  return sig + bkg;
}

int main() {
  // Exercise: create a histogram with `double` bins and fill it with values
  // representing a combined distribution with an exponential background and
  // a gaussian peak
  TH1D h{"h", "h", 100, 0, 10};

  // Standard C++ RNG engine, seeded from a random device
  std::mt19937 rng{std::random_device{}()};

  std::normal_distribution<double> sig{6, 0.5};
  std::exponential_distribution<double> bkg{0.5};

  for (auto i = 0; i < 20000; i++)
    h.Fill(bkg(rng));
  for (auto i = 0; i < 5000; i++)
    h.Fill(sig(rng));

  TF1 tf1{"my_fit", fit_function, 0, 10, 5};
  tf1.SetParameters(900, 0.5, 500, 6, 0.5);

  h.Fit(&tf1);

  TCanvas c{"c", "c", 800, 600};
  h.Draw();
  c.SaveAs("exercise_01_fit_solution.png");

  return 0;
}
