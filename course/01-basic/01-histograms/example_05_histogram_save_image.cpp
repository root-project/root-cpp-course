#include <TCanvas.h>
#include <TH1D.h>

#include <iostream>

void example_05_histogram_save_image() {
  TH1D h{"h", "h", 10, -5, 5};
  h.FillRandom("gaus");

  // The TCanvas is the C++ class providing the interface to draw plots with
  // ROOT graphics. The canvas interacts with a large variety of other ROOT
  // classes. In this example, we show how to save a plot of the generated
  // histogram to disk as an image.
  // Parameter meaning: name, title, width (pixel), height (pixel)
  TCanvas c{"c", "c", 800, 600};
  h.Draw();
  // Can also output to JPG, PDF, ...
  c.SaveAs("05_histogram_save_image.png");
}

int main() {
  example_05_histogram_save_image();
  return 0;
}
