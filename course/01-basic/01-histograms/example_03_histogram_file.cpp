#include <TFile.h>
#include <TH1D.h>

#include <iostream>

void write_histo(std::string_view histoname, std::string_view filename) {
  // Opens the file and immediately wraps it around a std::unique_ptr. The
  // memory will be automatically managed by it. Removes the necessity to write
  // explicitly `new` and `delete` and it is exception-safe
  std::unique_ptr<TFile> f{TFile::Open(filename.data(), "RECREATE")};

  TH1D h{histoname.data(), histoname.data(), 10, -5, 5};
  h.FillRandom("gaus");

  f->WriteObject(&h, h.GetName());
}

void read_histo(std::string_view histoname, std::string_view filename) {
  std::unique_ptr<TFile> f{TFile::Open(filename.data())};

  // We provide the template argument type to get a pointer of the correct type
  // No need to manage its memory, it is a responsibility of the file
  auto *h = f->Get<TH1D>(histoname.data());
  // If a function returns a pointer, we check it is not nullptr
  if (!h)
    throw std::runtime_error("Failure reading histogram from the file.");

  std::cout << "Histogram (" << h->GetEntries()
            << " entries): mean=" << h->GetMean() << " +- " << h->GetStdDev()
            << "\n";
}

void example_03_histogram_file() {
  const auto histoname{"h"};
  const auto filename{"myfile.root"};
  write_histo(histoname, filename);
  read_histo(histoname, filename);
}

int main() {
  example_03_histogram_file();
  return 0;
}
