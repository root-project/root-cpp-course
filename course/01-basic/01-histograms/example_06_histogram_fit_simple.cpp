#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>

#include <iostream>

void histogram_fit_native() {
  TH1D h{"h", "h", 10, -5, 5};
  h.FillRandom("gaus");

  TF1 f("f", "gaus", -5, 5);
  f.SetParameters(1000, 0, 1);
  h.Fit(&f);

  TCanvas c{"c", "c", 800, 600};
  h.Draw();
  c.SaveAs("06_histogram_fit_simple_native.png");
}

void histogram_fit_custom_function() {
  TH1D h{"h", "h", 10, -5, 5};
  h.FillRandom("gaus");

  // TF1 constructor accepts a function pointer with signature
  // double (double *, double *)
  // The first parameter is the array of input values, the second parameter
  // is an array of parameters for the fit
  // We are using a C++ lambda in this case just to show that anything that
  // can be converted to a C++ function pointer may work
  auto gaus = [](double *values, double *parameters) {
    // We are fitting a 1D histogram, so there is only one value at a time
    const auto &x = values[0];
    // The gaussian curve needs three parameters
    const auto &constant = parameters[0];
    const auto &mean = parameters[1];
    const auto &sigma = parameters[2];
    return constant * std::exp(-0.5 * std::pow((x - mean) / sigma, 2));
  };

  // The 5th parameter in this case, npar, is used to signal to TF1 how many
  // parameters are needed for the fit with the input custom function.
  TF1 f("f", gaus, /*xmin*/ -5, /*xmax*/ 5, /*npar*/ 3);
  // The values of the three parameters are initialized to help with the fit
  f.SetParameters(1000, 0, 1);
  h.Fit(&f);

  TCanvas c{"c", "c", 800, 600};
  h.Draw();
  c.SaveAs("06_histogram_fit_simple_custom_function.png");
}

void example_06_histogram_fit_simple() {
  std::cout << "Fitting gaussian distribution with native ROOT function\n";
  histogram_fit_native();

  std::cout << "Fitting gaussian distribution with custom C++ function\n";
  histogram_fit_custom_function();
}

int main() {
  example_06_histogram_fit_simple();
  return 0;
}
