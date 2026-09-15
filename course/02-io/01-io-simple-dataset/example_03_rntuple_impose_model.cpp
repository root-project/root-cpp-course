#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleWriter.hxx>

#include <iostream>
#include <random>

void write(std::string_view dataset_name, std::string_view file_name,
           unsigned n_entries) {

  // The RNTupleModel represents the dataset schema
  auto model = ROOT::RNTupleModel::Create();
  // New RNTuple fields can be created via MakeField: a unified API for all
  // data types stored in the RNTuple
  std::shared_ptr<std::vector<float>> muon_pt =
      model->MakeField<std::vector<float>>("muon_pt");
  std::shared_ptr<std::uint64_t> event_id =
      model->MakeField<std::uint64_t>("event_id");

  // Writing API: Create an RNTupleWriter passing the model we created above
  // The RNTupleWriter takes ownership of the model, and the values in the
  // fields are shared between the writer and the users
  auto writer =
      ROOT::RNTupleWriter::Recreate(std::move(model), dataset_name, file_name);

  std::mt19937 rng{std::random_device{}()};

  std::normal_distribution<float> gaus{15, 5};
  std::uniform_int_distribution<std::size_t> n_particles{0, 5};

  for (auto i = 0; i < n_entries; i++) {
    *event_id = i;
    auto n = n_particles(rng);
    muon_pt->clear();
    for (auto j = 0; j < n; j++) {
      muon_pt->push_back(gaus(rng));
    }

    // Writing API: once all values are properly set for the current
    // event, RNTupleWriter::Fill ensures the data is written to the RNTuple
    // in-memory
    writer->Fill();
  }

  // The writer goes out of scope here.  On destruction, the writer flushes
  // unwritten data to disk and closes the attached ROOT file.
}

void read(std::string_view dataset_name, std::string_view file_name) {

  // The RNTupleModel represents the dataset schema
  auto model = ROOT::RNTupleModel::Create();
  // The model establishes what RNTuple will attempt to read from disk. If
  // less than the whole dataset schema is needed, one can just create less
  // fields. Only the active fields will be read. Through the model we can
  // also impose reading a field in memory with a different type than the
  // on-disk counterpart, as long as they are compatible
  auto muon_pt = model->MakeField<std::vector<double>>("muon_pt");

  auto reader =
      ROOT::RNTupleReader::Open(std::move(model), dataset_name, file_name);

  for (auto entry_idx : *reader) {
    // Reading API: RNTupleReader::LoadEntry. Moves a cursor to the entry index
    // parameter, reading data into the shared memory locations created via the
    // RNTupleModel
    reader->LoadEntry(entry_idx);
    std::cout << "muon_pt={";
    for (const auto &v : *muon_pt)
      std::cout << v << ",";
    std::cout << "}\n";
  }
}

void example_03_rntuple_impose_model() {
  constexpr auto file_name{"example_03_rntuple_impose_model.root"};
  constexpr auto dataset_name{"dataset"};
  constexpr auto n_entries{10};

  write(dataset_name, file_name, n_entries);
  read(dataset_name, file_name);
}

int main() {
  example_03_rntuple_impose_model();
  return 0;
}
