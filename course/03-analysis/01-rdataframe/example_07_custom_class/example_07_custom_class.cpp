#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>
#include <TH1.h>
#include <TLatex.h>
#include <TStyle.h>

#include "Muon.hpp"

void plot(TH1D &h) {
  // Produce plot
  gStyle->SetOptStat(0);
  gStyle->SetTextFont(42);
  TCanvas c{"c", "", 800, 700};
  c.SetLogx();
  c.SetLogy();

  h.GetXaxis()->SetTitleSize(0.04);
  h.GetYaxis()->SetTitleSize(0.04);
  h.DrawClone();

  TLatex label;
  label.SetNDC(true);
  label.DrawLatex(0.175, 0.740, "#eta");
  label.DrawLatex(0.205, 0.775, "#rho,#omega");
  label.DrawLatex(0.270, 0.740, "#phi");
  label.DrawLatex(0.400, 0.800, "J/#psi");
  label.DrawLatex(0.415, 0.670, "#psi'");
  label.DrawLatex(0.485, 0.700, "Y(1,2,3S)");
  label.DrawLatex(0.755, 0.680, "Z");
  label.SetTextSize(0.040);
  label.DrawLatex(0.100, 0.920, "#bf{CMS Open Data}");
  label.SetTextSize(0.030);
  label.DrawLatex(0.630, 0.920, "#sqrt{s} = 8 TeV, L_{int} = 11.6 fb^{-1}");

  c.SaveAs("example_07_custom_class.png");
}

void example_07_custom_class() {

  constexpr auto dataset_name{"Events"};
  constexpr auto dataset_path{"../data/skimmed_dimuons.root"};

  ROOT::RDataFrame df{dataset_name, dataset_path};

  auto df_muons = df.Define(
      "Muons",
      [](unsigned n_muons, const ROOT::RVecI &charge, const ROOT::RVecF &pt,
         const ROOT::RVecF &eta, const ROOT::RVecF &phi,
         const ROOT::RVecF &mass) {
        std::vector<analysis::Muon> muons;
        muons.reserve(n_muons);
        for (auto i = 0; i < n_muons; i++) {
          muons.emplace_back(charge[i], pt[i], eta[i], phi[i], mass[i]);
        }
        return muons;
      },
      {"nMuon", "Muon_charge", "Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});

  // Compute invariant mass of the dimuon system
  auto df_mass = df_muons.Define("Dimuon_mass",
                                 [](const std::vector<analysis::Muon> &muons) {
                                   return analysis::invariant_mass(muons);
                                 },
                                 {"Muons"});

  // Make histogram of dimuon mass spectrum. Note how we can set title and axis
  // labels in one go
  auto h = df_mass.Histo1D<float>({"Dimuon_mass",
                                   "Dimuon mass;m_{#mu#mu} (GeV);N_{Events}",
                                   30000, 0.25, 300},
                                  "Dimuon_mass");

  // Request cut-flow report
  auto report = df.Report();

  // Request to save data to disk with
  ROOT::RDF::RSnapshotOptions opts;
  opts.fLazy =
      true; // So that we don't immediately trigger the computation graph
  opts.fVector2RVec = false; // Decide whether to keep the std::vector or
                             // ROOT::RVec in the output file

  // This saves by default to TTree, try also to store an RNTuple output
  auto snap = df_mass.Snapshot("Events", "output.root", {"Muons"}, opts);

  // Plot the histogram, this also triggers the computation graph
  plot(*h);

  // Print cut-flow report
  report->Print();
}

int main() {
  example_07_custom_class();
  return 0;
}
