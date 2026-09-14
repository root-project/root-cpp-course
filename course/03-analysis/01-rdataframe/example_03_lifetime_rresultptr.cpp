#include <ROOT/RDFHelpers.hxx> // TH1DModel
#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>
#include <THStack.h>

#include <random>
#include <vector>

void example_03_lifetime_rresultptr() {
  // Create a simple dataframe that fills random numbers into histograms.
  ROOT::RDF::RNode node = ROOT::RDataFrame{10};
  std::mt19937 rng{std::random_device{}()};
  std::normal_distribution<double> gaus{5., 1.};
  node = node.Define("x", [&]() { return gaus(rng); });

  // We will reuse the same histogram binning for all histograms in the example
  ROOT::RDF::TH1DModel histoModel{"Histo", "Histo;x", 10, 0, 10};

  // Keeping the results alive is vital when they are passed to other entities
  // or when they are drawn. Compare the following situations:

  // 1. The wrong way (the result ht is destroyed at the end of the loop body):
  THStack histStack1("histStack1", "Stacking result histograms (wrong way)");
  for (int i = 0; i < 2; i++) {
    auto ht = node.Histo1D(histoModel, {"x"});
    ht->SetFillColor(kBlue + i);
    histStack1.Add(ht.GetPtr()); // Wrong, this histogram will not survive
  }
  TCanvas c1{"c1", "c1"};
  histStack1.Draw();
  c1.SaveAs("example_03_lifetime_rresultptr_wrong.png");

  // 2. The right way: Results survive because we keep them around via the
  //    owning RResultPtr objects
  THStack histStack2("histStack2", "Stacking result histograms (correct way)");
  std::vector<ROOT::RDF::RResultPtr<TH1D>> results;
  for (int i = 0; i < 2; i++) {
    results.push_back(node.Histo1D(histoModel, {"x"}));
    auto &ht = results.back();
    ht->SetFillColor(kBlue + 2 * i);
    histStack2.Add(ht.GetPtr());
  }
  TCanvas c2{"c2", "c2"};
  histStack2.Draw();
  c2.SaveAs("example_03_lifetime_rresultptr_correct.png");
}

int main() {
  example_03_lifetime_rresultptr();
  return 0;
}
