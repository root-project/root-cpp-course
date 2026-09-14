#include <ROOT/RNTupleInspector.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <TSystem.h>

#include <iostream>
#include <random>

void write(std::string_view dataset_name, std::string_view file_name,
           unsigned n_entries) {

  auto model = ROOT::RNTupleModel::Create();
  auto muon_pt = model->MakeField<std::vector<float>>("muon_pt");
  auto muon_eta = model->MakeField<std::vector<float>>("muon_eta");
  auto muon_phi = model->MakeField<std::vector<float>>("muon_phi");

  auto writer =
      ROOT::RNTupleWriter::Recreate(std::move(model), dataset_name, file_name);

  std::mt19937 rng{std::random_device{}()};

  std::normal_distribution<float> gaus_pt{15, 5};
  std::normal_distribution<float> gaus_eta{0, 2};
  std::normal_distribution<float> gaus_phi{0, 6};
  std::uniform_int_distribution<std::size_t> n_particles{0, 5};

  for (auto i = 0; i < n_entries; i++) {
    auto n = n_particles(rng);
    muon_pt->clear();
    muon_eta->clear();
    muon_phi->clear();
    for (auto j = 0; j < n; j++) {
      muon_pt->push_back(gaus_pt(rng));
      muon_eta->push_back(gaus_eta(rng));
      muon_phi->push_back(std::max(0.f, gaus_phi(rng)));
    }
    writer->Fill();
  }
}

void exercise_01_rntuple_inspector() {
  // In this exercise, take a deeper look at the documentation of the
  // RNTupleInspector
  // https://root.cern/doc/v640/classROOT_1_1Experimental_1_1RNTupleInspector.html
  // and explore the dataset created with the write function. Can you:
  // * Print the storage information of the various column types present?
  // * Get the storage information inspector for each field, and print the
  // compression factor for each field separately?
  // * Get the page size distribution for different column types and plot it?
  // Try also to modify the input dataset by creating more fields of different
  // types
}

int main() {
  exercise_01_rntuple_inspector();
  return 0;
}
