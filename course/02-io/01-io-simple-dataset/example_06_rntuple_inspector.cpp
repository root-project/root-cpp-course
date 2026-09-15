#include <ROOT/RNTupleInspector.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <TSystem.h>

#include <iostream>
#include <random>

void write(std::string_view dataset_name, std::string_view file_name,
           unsigned n_entries) {

  // The RNTupleModel represents the dataset schema
  auto model = ROOT::RNTupleModel::Create();
  // New RNTuple fields can be created via MakeField: a unified API for all
  // data types stored in the RNTuple
  auto muon_pt = model->MakeField<std::vector<float>>("muon_pt");
  auto muon_eta = model->MakeField<std::vector<float>>("muon_eta");
  auto muon_phi = model->MakeField<std::vector<float>>("muon_phi");

  // Writing API: Create an RNTupleWriter passing the model we created above
  // The RNTupleWriter takes ownership of the model, and the values in the
  // fields are shared between the writer and the users
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

    // Writing API: once all values are properly set for the current
    // event, RNTupleWriter::Fill ensures the data is written to the RNTuple
    // in-memory
    writer->Fill();
  }

  // The writer goes out of scope here.  On destruction, the writer flushes
  // unwritten data to disk and closes the attached ROOT file.
}

void inspect(std::string_view dataset_name, std::string_view file_name) {

  // The RNTupleInspector is the entry point to a series of utilities
  // to query on-disk information about RNTuple. See its documentation at
  // https://root.cern/doc/v640/classROOT_1_1Experimental_1_1RNTupleInspector.html
  auto inspector =
      ROOT::Experimental::RNTupleInspector::Create(dataset_name, file_name);

  std::cout << "Information about RNTuple '" << dataset_name << "' in file '"
            << file_name << "'\n\t"
            << "Compressed size (B): " << inspector->GetCompressedSize()
            << "\n\t"
            << "Compression factor: " << inspector->GetCompressionFactor()
            << "\n\t"
            << "Compression settings: "
            << inspector->GetCompressionSettingsAsString() << "\n";
}

void example_06_rntuple_inspector() {
  constexpr auto file_name{"example_06_rntuple_inspector.root"};
  constexpr auto dataset_name{"dataset"};
  constexpr auto n_entries{10000};

  write(dataset_name, file_name, n_entries);
  inspect(dataset_name, file_name);
}

int main() {
  example_06_rntuple_inspector();
  return 0;
}
