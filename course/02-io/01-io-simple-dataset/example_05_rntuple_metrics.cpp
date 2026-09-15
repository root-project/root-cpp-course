#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>
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

void read(std::string_view dataset_name, std::string_view file_name) {

  auto reader = ROOT::RNTupleReader::Open(dataset_name, file_name);

  // Collect I/O runtime counters when processing the data set.
  // Maintaining the counters comes with a small performance overhead, so it has
  // to be explicitly enabled
  reader->EnableMetrics();
  auto muon_pt_view = reader->GetView<std::vector<float>>("muon_pt");
  for (auto entry_idx : *reader) {
    // Reading API: RNTupleView::operator(). Requests the value of the given
    // field at the given location and exposes it as a const T &
    muon_pt_view(entry_idx);
  }

  // Display the I/O operation statistics performed by the RNTuple reader
  reader->PrintInfo(ROOT::ENTupleInfo::kMetrics);

  const auto &metrics = reader->GetMetrics();
  float nbytesRead =
      metrics.GetCounter("RNTupleReader.RPageSourceFile.szReadPayload")
          ->GetValueAsInt() +
      metrics.GetCounter("RNTupleReader.RPageSourceFile.szReadOverhead")
          ->GetValueAsInt();
  float fileSize = metrics.GetCounter("RNTupleReader.RPageSourceFile.szFile")
                       ->GetValueAsInt();

  // We have read one vector field out of three so we expect to have read around
  // one third of the dataset
  std::cout << "File size:      " << fileSize / 1024. << " KiB" << std::endl;
  std::cout << "Read from file: " << nbytesRead / 1024. << " KiB" << std::endl;
  std::cout << "Ratio:          " << nbytesRead / fileSize << std::endl;
}

void example_05_rntuple_metrics() {
  constexpr auto file_name{"example_05_rntuple_metrics.root"};
  constexpr auto dataset_name{"dataset"};
  constexpr auto n_entries{10000};

  write(dataset_name, file_name, n_entries);
  read(dataset_name, file_name);
}

int main() {
  example_05_rntuple_metrics();
  return 0;
}
