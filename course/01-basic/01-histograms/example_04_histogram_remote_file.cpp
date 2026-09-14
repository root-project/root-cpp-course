#include <TFile.h>
#include <TH1F.h>

#include <iostream>

void example_04_histogram_remote_file() {
  // Opens the file and immediately wraps it around a std::unique_ptr. The
  // memory will be automatically managed by it. TFile::Open allows to
  // seamlessly read remote files
  std::unique_ptr<TFile> f{
      TFile::Open("root://eospublic.cern.ch//eos/root-eos/hsimple.root")};

  // We provide the template argument type to get a pointer of the correct type
  // No need to manage its memory, it is a responsibility of the file
  // The type of the histogram is TH1F in this case, meaning the bins are of
  // type `float`
  auto *h = f->Get<TH1F>("hpx");
  // If a function returns a pointer, we check it is not nullptr
  if (!h)
    throw std::runtime_error("Failure reading histogram from the file.");

  std::cout << "Histogram (" << h->GetEntries()
            << " entries): mean=" << h->GetMean() << " +- " << h->GetStdDev()
            << "\n";
}

int main() {
  example_04_histogram_remote_file();
  return 0;
}
