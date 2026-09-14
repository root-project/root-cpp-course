#include <TCanvas.h>
#include <TH1.h>

#include <memory>

// We create a unique_ptr to a canvas at global scope to avoid losing the canvas
// object after the end of the main function
auto canvas = std::make_unique<TCanvas>("c", "c", 800, 600);

void example_01_interactive_canvas() {
  TH1D h{"h", "h", 10, -5, 5};
  h.FillRandom("gaus");

  // We call DrawClone which creates a clone of the histogram and attaches that
  // to the canvas. This way, even after we leave this function scope, the
  // canvas still holds the histogram that has been drawn on it.
  h.DrawClone();
  canvas->Draw();
}
