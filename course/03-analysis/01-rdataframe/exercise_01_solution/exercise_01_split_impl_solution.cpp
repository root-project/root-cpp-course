#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>

#include "exercise_01_split_impl_solution_helpers.hpp"
#include <thread>

void single_thread(unsigned n_entries, TCanvas &canvas) {

  // Single thread for reference
  auto df = ROOT::RDataFrame(n_entries).Define("x", get_normal_global);
  auto h = df.Histo1D({"h1", "Single thread (no MT)", 1000, -4, 4}, {"x"});
  canvas.cd(1);
  h->DrawClone();
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

int main() {
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

  canvas.SaveAs("exercise_01_split_impl_solution.png");
  return 0;
}
