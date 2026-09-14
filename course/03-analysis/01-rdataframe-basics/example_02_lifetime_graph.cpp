#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>

#include <random>

static double GetRandomGaus() {
  static std::mt19937 rng{std::random_device{}()};
  static std::normal_distribution<double> gaus{10, 1};
  return gaus(rng);
}

auto create_df() { return ROOT::RDataFrame{1000}; }

auto create_column(ROOT::RDF::RNode node) {
  return node.Define("x", [] { return GetRandomGaus(); });
}

auto create_histo(ROOT::RDF::RNode node) {
  return node.Histo1D<double>({"h", "h", 100, 5, 15}, "x");
}

void example_02_lifetime_graph() {
  // The computation graph is shared among nodes. Even if a node is created
  // inside a function scope, the computation graph survives if there is at
  // least one node attached to it. For example, the next line creates a
  // histogram node, which can still be run even if the RDataFrame object
  // created within 'create_df' goes out of scope.
  auto h = create_histo(create_column(create_df()));

  TCanvas c{"c", "c", 800, 600};
  h->Draw();
  c.SaveAs("example_02_lifetime_graph.png");
}

int main() {
  example_02_lifetime_graph();
  return 0;
}
