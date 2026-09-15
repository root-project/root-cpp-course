#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TLatex.h>
#include <TStyle.h>

void example_06_opendata_dimuon() {
  // This example is an adaptation of
  // https://root.cern.ch/doc/v640/df102__NanoAODDimuonAnalysis_8C.html
  // employing the fully-typed C++ RDataFrame API

  // Enable multi-threading
  ROOT::EnableImplicitMT();

  constexpr auto dataset_name{"Events"};
  constexpr auto dataset_path{
      "root://eospublic.cern.ch//eos/opendata/cms/derived-data/"
      "AOD2NanoAODOutreachTool/Run2012BC_DoubleMuParked_Muons.root"};

  ROOT::RDataFrame df{dataset_name, dataset_path};

  // Selection: Exactly two muons with opposite charge
  auto df_2mu =
      df.Filter([](unsigned int n_muon) { return n_muon == 2; }, {"nMuon"});
  auto df_os = df_2mu.Filter(
      [](const ROOT::RVecI &muon_charge) {
        return muon_charge[0] != muon_charge[1];
      },
      {"Muon_charge"});

  // Compute invariant mass of the dimuon system
  auto df_mass = df_os.Define("Dimuon_mass", ROOT::VecOps::InvariantMass<float>,
                              {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});

  // Make histogram of dimuon mass spectrum. Note how we can set title and axis
  // labels in one go
  auto h = df_mass.Histo1D<float>({"Dimuon_mass",
                                   "Dimuon mass;m_{#mu#mu} (GeV);N_{Events}",
                                   30000, 0.25, 300},
                                  "Dimuon_mass");

  // Request cut-flow report
  auto report = df.Report();

  // Produce plot
  gStyle->SetOptStat(0);
  gStyle->SetTextFont(42);
  TCanvas c{"c", "", 800, 700};
  c.SetLogx();
  c.SetLogy();

  h->GetXaxis()->SetTitleSize(0.04);
  h->GetYaxis()->SetTitleSize(0.04);
  h->DrawClone();

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

  c.SaveAs("example_06_opendata_dimuon.png");

  // Print cut-flow report
  report->Print();
}

int main() {
  example_06_opendata_dimuon();
  return 0;
}
