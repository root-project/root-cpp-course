#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <memory>
#include <random>

void write(std::string_view dataset_name, std::string_view file_name,
           unsigned n_entries) {
  std::unique_ptr<TFile> file{TFile::Open(file_name.data(), "RECREATE")};
  auto tree = std::make_unique<TTree>(dataset_name.data(), dataset_name.data());

  std::mt19937 rng{std::random_device{}()};

  std::normal_distribution<float> gaus{15, 5};
  std::uniform_int_distribution<std::size_t> n_particles{0, 5};

  // Writing API: TTree::Branch. Creates a new top-level branch with a certain
  // name, taking values from the object pointed to by the address passed as
  // second parameter. The pointer can be typed: TTree will infer from the type
  // the information to properly store the data to disk.
  std::uint64_t event_id{0};
  std::vector<float> muon_pt{};
  tree->Branch("event_id", &event_id);
  tree->Branch("muon_pt", &muon_pt);

  for (auto i = 0; i < n_entries; i++) {
    event_id = i;
    auto n = n_particles(rng);
    muon_pt.clear();
    for (auto j = 0; j < n; j++) {
      muon_pt.push_back(gaus(rng));
    }

    // Writing API: once all values are properly set for the current
    // event, TTree::Fill ensures the data is written to the TTree in-memory
    tree->Fill();
  }

  // Must write all data to disk before leaving the function
  file->Write();
}

void read(std::string_view dataset_name, std::string_view file_name,
          unsigned n_entries) {
  std::unique_ptr<TFile> file{TFile::Open(file_name.data())};
  // No unique_ptr here: TFile retains ownership of the object by default
  auto *tree = file->Get<TTree>(dataset_name.data());

  // Reading API: TTree::SetBranchAddress. The on-disk data will be read into
  // the memory location pointed at by the second parameter.
  // POD type: pass by pointer to value
  std::uint64_t event_id{0};
  // Class type: pass by pointer to pointer
  std::vector<float> *muon_pt{nullptr};
  tree->SetBranchAddress("event_id", &event_id);
  tree->SetBranchAddress("muon_pt", &muon_pt);

  for (auto i = 0; i < n_entries; i++) {
    // Reading API: TTree::GetEntry. Moves a cursor to the entry index
    // parameter, reading data into the addresses previously set
    tree->GetEntry(i);

    std::cout << "event_id=" << event_id << ", muon_pt={";
    for (const auto &v : *muon_pt)
      std::cout << v << ",";
    std::cout << "}\n";
  }
}

void example_01_ttree() {
  constexpr auto file_name{"example_01_ttree.root"};
  constexpr auto dataset_name{"dataset"};
  constexpr auto n_entries{10};

  write(dataset_name, file_name, n_entries);
  read(dataset_name, file_name, n_entries);
}

int main() {
  example_01_ttree();
  return 0;
}
