#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <thread>

// Global generator, not thread-safe
inline std::mt19937 rng{std::random_device{}()};
inline std::normal_distribution<double> gaus(0., 1.);

double get_normal_global() { return gaus(rng); }

void single_thread(unsigned n_entries, TCanvas &canvas) {

  // Single thread for reference
  auto df = ROOT::RDataFrame(n_entries).Define("x", get_normal_global);
  auto h = df.Histo1D({"h1", "Single thread (no MT)", 1000, -4, 4}, {"x"});
  canvas.cd(1);
  h->DrawClone();
}

// One generator per slot — initialized once before the event loop
// An alternative to these global vectors could be to have thread_local
// variables within a function scope and then call that function from RDataFrame
inline std::vector<std::mt19937> generators;
inline std::vector<std::normal_distribution<double>> gaussians;

void reinitialize_generators(unsigned int n_slots) {
  std::random_device rd;
  generators.resize(n_slots);
  for (auto &gen : generators)
    gen.seed(rd());
  gaussians.resize(n_slots, std::normal_distribution<double>(0., 1.));
}

double get_normal_per_slot(unsigned int slot) {
  return gaussians[slot](generators[slot]);
}

double get_normal_per_slot_and_entry(unsigned int slot,
                                     unsigned long long entry) {
  // We want to generate a random number distributed according to a normal
  // distribution in a thread-safe way and such that it is reproducible across
  // different RDataFrame runs, i.e. given the same input to the generator it
  // will produce the same value. This is one way to do it. It assumes that the
  // input argument represents a unique entry ID, such that any thread
  // processing an RDataFrame task will see this number once throughout the
  // entire execution of the computation graph Calling both `reset` and `seed`
  // methods is fundamental here to ensure reproducibility: without them the
  // same generator could be seeded by a different entry (depending on which is
  // the first entry ID seen by a thread) or could be at a different step of the
  // sequence (depending how many entries this particular thread is processing).
  // Alternatively, if both the generator and the distribution objects were
  // recreated from scratch at every function call (i.e. by removing the global
  // std::vector stores), then the next two method calls would not be necessary,
  // at the cost of a possible performance degradation.
  gaussians[slot].reset();
  generators[slot].seed(entry);
  return gaussians[slot](generators[slot]);
}

void mt_random_seeding(unsigned n_entries, TCanvas &canvas) {
  // One generator per RDataFrame slot, with random_device seeding
  // Notes and Caveats:
  // - How many numbers are drawn from each generator is not deterministic
  //   and the result is not deterministic between runs.

  auto df = ROOT::RDataFrame(n_entries).DefineSlot("x", get_normal_per_slot);
  auto h = df.Histo1D(
      {"h2", "Thread-safe (MT, non-deterministic)", 1000, -4, 4}, {"x"});
  canvas.cd(2);
  h->DrawClone();
}

void mt_deterministic_seeding(unsigned n_entries, TCanvas &canvas) {
  // One generator per RDataFrame slot, with entry seeding
  // Notes and Caveats:
  // - With RDataFrame(INTEGER_NUMBER) constructor (as in the example),
  //   the result is deterministic and identical on every run
  // - With RDataFrame(TTree) constructor, the result is not guaranteed to be
  // deterministic.
  //   To make it deterministic, use something from the dataset to act as the
  //   event identifier instead of rdfentry_, and use it as a seed.

  auto df = ROOT::RDataFrame(n_entries).DefineSlotEntry(
      "x", get_normal_per_slot_and_entry);
  auto h =
      df.Histo1D({"h3", "Thread-safe (MT, deterministic)", 1000, -4, 4}, {"x"});

  canvas.cd(3);
  h->DrawClone();
}

void example_04_threadsafe_rng() {
  TCanvas canvas{"c", "c", 1000, 500};
  canvas.Divide(3, 1);

  constexpr unsigned n_entries{10000};

  single_thread(n_entries, canvas);

  unsigned int n_slots{std::max(2u, std::thread::hardware_concurrency() / 4u)};
  ROOT::EnableImplicitMT(n_slots);

  // Before running the RDataFrame computation graph, we reinitialize the
  // generators (one per slot), so they can be used accordingly during the
  // execution.
  reinitialize_generators(n_slots);
  mt_random_seeding(n_entries, canvas);

  // Before running the RDataFrame computation graph, we reinitialize the
  // generators (one per slot), so they can be used accordingly during the
  // execution.
  reinitialize_generators(n_slots);
  mt_deterministic_seeding(n_entries, canvas);

  canvas.SaveAs("example_04_threadsafe_rng.png");
}

int main() {
  example_04_threadsafe_rng();
  return 0;
}
