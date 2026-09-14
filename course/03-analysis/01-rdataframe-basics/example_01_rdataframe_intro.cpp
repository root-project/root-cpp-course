#include <ROOT/RDataFrame.hxx>
#include <ROOT/RLogger.hxx>
#include <TCanvas.h>

#include <algorithm>
#include <random>

void example_01_rdataframe_intro() {
  // We enable the RDataFrame logging which will tell us if there is anything
  // to JIT compile before running the computation graph
  auto verbosity = ROOT::RLogScopedVerbosity(ROOT::Detail::RDF::RDFLogChannel(),
                                             ROOT::ELogLevel::kInfo);

  // Create an empty dataframe with 100 sequential entries
  ROOT::RDataFrame df{1000};

  std::mt19937 rng{std::random_device{}()};

  std::uniform_real_distribution<double> gen_uniform_val{-1, 1};
  std::uniform_int_distribution<std::size_t> gen_n_elems{0, 16};

  auto generate_vector = [&](std::size_t length) {
    ROOT::RVecD values(length);
    std::transform(values.begin(), values.end(), values.begin(),
                   [&](auto &) { return gen_uniform_val(rng); });
    return values;
  };

  // RDataFrame transformations such as Define and Filter accept any type
  // of C++ function pointer, in this example we shou usage through C++ lambdas
  // capturing an external object
  auto node = df.Define("length", [&] { return gen_n_elems(rng); })
                  .Define("x", generate_vector, {"length"})
                  .Define("y", generate_vector, {"length"})
                  .Define("r",
                          [](const ROOT::RVecD &x, const ROOT::RVecD &y) {
                            return sqrt(x * x + y * y);
                          },
                          {"x", "y"});

  auto select_rvec = [](const ROOT::RVecD &vec, const ROOT::RVecI &cond) {
    return vec[cond];
  };
  auto ring_h =
      node.Define(
              "condition",
              [](const ROOT::RVecD &r, const ROOT::RVecD &x,
                 const ROOT::RVecD &y) { return r > .5 && r < 1 && x * y < 0; },
              {"r", "x", "y"})
          .Define("valid_x", select_rvec, {"x", "condition"})
          .Define("valid_y", select_rvec, {"y", "condition"})
          // RDataFrame actions accept column types as template arguments
          // This avoids the need to JIT compile the type information
          .Histo2D<ROOT::RVecD, ROOT::RVecD>(
              {"fig", "Two quarters of a ring", 64, -1.1, 1.1, 64, -1.1, 1.1},
              "valid_x", "valid_y");

  TCanvas canvas{"c", "c", 800, 600};
  ring_h->Draw("colz");
  canvas.SaveAs("example_01_rdataframe_intro.png");
}

int main() {
  example_01_rdataframe_intro();
  return 0;
}
