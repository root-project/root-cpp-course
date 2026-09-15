// exercise.cpp
// Higgs -> ZZ -> 4 leptons analysis (adapted from
// https://root.cern.ch/doc/v634/df106__HiggsToFourLeptons_8C.html),
// rewritten with the FULLY TYPED RDataFrame C++ API: no textual
// expressions are passed to Filter/Define (which RDataFrame would JIT
// via Cling), and histogram actions use explicit template arguments
// (Histo1D<float>) instead of relying on runtime type deduction.

#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RLogger.hxx>
#include <ROOT/RVec.hxx>

#include <Math/Vector4D.h>

#include <TCanvas.h>
#include <TGraph.h>
#include <TH1.h>
#include <THStack.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>

#include <cmath>
#include <string>
#include <vector>

using ROOT::RVecF;
using ROOT::RVecI;
using ROOT::RVecU;
using ROOT::RDF::RSampleInfo;
using PtEtaPhiEVectorF =
    ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<float>>;

void plot(TH1D &h_other, TH1D &h_zz, TH1D &h_higgs, TH1D &h_mc_nominal,
          TH1D &h_mc_weight_up, TH1D &h_mc_weight_down, TH1D &h_data_mass) {
  gROOT->SetStyle("ATLAS");

  TCanvas c{"c", " ", 600, 600};
  TPad pad{"upper_pad", "", 0, 0, 1, 1};
  pad.SetTickx(0);
  pad.SetTicky(0);
  pad.Draw();
  pad.cd();

  h_other.SetFillColor(kViolet - 9);
  h_zz.SetFillColor(kAzure - 9);
  h_higgs.SetFillColor(kRed + 2);

  THStack stack{"stack", ""};
  stack.Add(&h_other);
  stack.Add(&h_zz);
  stack.Add(&h_higgs);

  stack.Draw("HIST");
  stack.SetMaximum(35);

  auto *histo_frame = stack.GetHistogram();
  histo_frame->SetTitle("");
  histo_frame->GetXaxis()->SetLabelSize(0.035);
  histo_frame->GetXaxis()->SetTitleSize(0.045);
  histo_frame->GetXaxis()->SetTitleOffset(1.3);
  histo_frame->GetXaxis()->SetTitle("m_{4l}^{H#rightarrow ZZ} [GeV]");
  histo_frame->GetYaxis()->SetLabelSize(0.035);
  histo_frame->GetYaxis()->SetTitleSize(0.045);
  histo_frame->GetYaxis()->SetTitle("Events");
  histo_frame->GetYaxis()->ChangeLabel(1, -1, 0);

  h_mc_nominal.SetFillColor(kBlack);
  h_mc_nominal.SetFillStyle(3254);
  h_mc_nominal.Draw("E2 same");
  h_mc_weight_up.SetLineColor(kGreen + 2);
  h_mc_weight_up.Draw("HIST same");
  h_mc_weight_down.SetLineColor(kBlue + 2);
  h_mc_weight_down.Draw("HIST same");

  h_data_mass.SetMarkerStyle(20);
  h_data_mass.SetMarkerSize(1.);
  h_data_mass.SetLineWidth(2);
  h_data_mass.SetLineColor(kBlack);
  h_data_mass.SetStats(false);
  h_data_mass.Draw("E sames");

  TLegend legend{0.57, 0.65, 0.94, 0.94};
  legend.SetTextFont(42);
  legend.SetFillStyle(0);
  legend.SetBorderSize(0);
  legend.SetTextSize(0.025);
  legend.SetTextAlign(32);
  legend.AddEntry(&h_data_mass, "Data", "lep");
  legend.AddEntry(&h_higgs, "Higgs MC", "f");
  legend.AddEntry(&h_zz, "ZZ MC", "f");
  legend.AddEntry(&h_other, "Other MC", "f");
  legend.AddEntry(&h_mc_weight_down, "Total MC Variations Down", "l");
  legend.AddEntry(&h_mc_weight_up, "Total MC Variations Up", "l");
  legend.AddEntry(&h_mc_nominal, "Total MC Uncertainty", "f");
  legend.Draw();

  TLatex atlas_label;
  atlas_label.SetTextFont(70);
  atlas_label.SetTextSize(0.04);
  atlas_label.DrawLatexNDC(0.19, 0.85, "ATLAS");
  TLatex data_label;
  data_label.SetTextFont(42);
  data_label.DrawLatexNDC(0.19 + 0.13, 0.85, "Open Data");
  TLatex header;
  header.SetTextFont(42);
  header.SetTextSize(0.035);
  header.DrawLatexNDC(0.21, 0.8, "#sqrt{s} = 13 TeV, 10 fb^{-1}");

  c.SaveAs("exercise_03_higgs_to_four_leptons_solution.png");
}

// Lepton quality selection, applied to the already-"good" (pt/eta/iso)
// subset of leptons in an event.
bool select_electrons_and_muons(const RVecI &type, const RVecF &pt,
                                const RVecF &eta, const RVecF &phi,
                                const RVecF &e, const RVecF &trackd0pv,
                                const RVecF &tracksigd0pv, const RVecF &z0) {
  for (size_t i = 0; i < type.size(); ++i) {
    PtEtaPhiEVectorF p(0.001f * pt[i], eta[i], phi[i], 0.001f * e[i]);
    if (type[i] == 11) {
      if (pt[i] < 7000 || abs(eta[i]) > 2.47 ||
          abs(trackd0pv[i] / tracksigd0pv[i]) > 5 ||
          abs(z0[i] * std::sin(p.Theta())) > 0.5)
        return false;
    } else {
      if (abs(trackd0pv[i] / tracksigd0pv[i]) > 5 ||
          abs(z0[i] * std::sin(p.Theta())) > 0.5)
        return false;
    }
  }
  return true;
}

// Invariant mass of the four selected leptons, in GeV.
float ComputeInvariantMass(const RVecF &pt, const RVecF &eta, const RVecF &phi,
                           const RVecF &e) {
  const auto n_particles{pt.size()};
  assert(n_particles == 4); // We assume only four leptons
  std::vector<PtEtaPhiEVectorF> ps(n_particles);
  for (auto i = 0; i < n_particles; i++) {
    // Convert MeV to GeV
    ps[i] = PtEtaPhiEVectorF{0.001f * pt[i], eta[i], phi[i], 0.001f * e[i]};
  }
  return std::accumulate(ps.begin(), ps.end(), PtEtaPhiEVectorF{}).M();
}

void exercise_03_higgs_to_four_leptons_solution() {

  // We enable the RDataFrame logging which will tell us if there is anything
  // to JIT compile before running the computation graph
  auto verbosity = ROOT::RLogScopedVerbosity(ROOT::Detail::RDF::RDFLogChannel(),
                                             ROOT::ELogLevel::kInfo);
  ROOT::EnableImplicitMT();

  ROOT::RDataFrame df = ROOT::RDF::Experimental::FromSpec(
      "exercise_03_higgs_to_four_leptons.json");
  ROOT::RDF::Experimental::AddProgressBar(df);

  // Define metadata in the computation graph through RSampleInfo, need to
  // request:
  // - cross section
  // - luminosity
  // - sum of weights squared
  // - sample category
  // - scale
  // Also, define a boolean column named "reweighting" based on whether the
  // sample is MC or not
  auto df_with_metadata =
      /*definitions here*/;

  // Selections
  auto df_filtered =
      df_with_metadata
          .Filter([](bool trigE, bool trigM) { return trigE || trigM; },
                  {"trigE", "trigM"})
          .Define("good_lep",
                  [](const RVecF &eta, const RVecF &pt, const RVecF &ptcone30,
                     const RVecF &etcone20) {
                    return abs(eta) < 2.5f && pt > 5000.f &&
                           ptcone30 / pt < 0.3f && etcone20 / pt < 0.3f;
                  },
                  {"lep_eta", "lep_pt", "lep_ptcone30", "lep_etcone20"})
          .Filter(
              [](const RVecI &good_lep) {
                return ROOT::VecOps::Sum(good_lep) == 4;
              },
              {"good_lep"})
          .Filter(
              [](const RVecI &charge, const RVecI &good_lep) {
                return ROOT::VecOps::Sum(charge[good_lep]) == 0;
              },
              {"lep_charge", "good_lep"})
          .Define("goodlep_sumtypes",
                  [](const RVecU &type, const RVecI &good_lep) {
                    return ROOT::VecOps::Sum(type[good_lep]);
                  },
                  {"lep_type", "good_lep"})
          .Filter([](unsigned s) { return s == 44 || s == 52 || s == 48; },
                  {"goodlep_sumtypes"})
          .Filter(
              [](const RVecU &type, const RVecF &pt, const RVecF &eta,
                 const RVecF &phi, const RVecF &e, const RVecF &trackd0pv,
                 const RVecF &tracksigd0pv, const RVecF &z0,
                 const RVecI &good_lep) {
                return select_electrons_and_muons(
                    type[good_lep], pt[good_lep], eta[good_lep], phi[good_lep],
                    e[good_lep], trackd0pv[good_lep], tracksigd0pv[good_lep],
                    z0[good_lep]);
              },
              {"lep_type", "lep_pt", "lep_eta", "lep_phi", "lep_E",
               "lep_trackd0pvunbiased", "lep_tracksigd0pvunbiased", "lep_z0",
               "good_lep"});

  // Restrict to four leptons and compute invariant mass
  // Use the "good_lep" mask created above to create 5 new columns where
  // the vectors of pt, eta, phi, E and type are restricted based on the mask
  // Afterwards, create a new column holding the invariant mass of the system
  // and call it "m4l"
  auto df_mass =
      /*definitions here*/;

  // MC branch: apply reweighting and build the total per-event weight.
  ROOT::RDF::RNode df_mc =
      df_mass
          .Filter([](bool reweighting) { return reweighting; }, {"reweighting"})
          .Define("weight",
                  [](float ele, float muon, float trig, float pileup,
                     float mcWeight, float scale, double xsecs, double sumws,
                     double lumi) {
                    return static_cast<float>(ele * muon * trig * pileup *
                                              mcWeight * scale * xsecs / sumws *
                                              lumi);
                  },
                  {"scaleFactor_ELE", "scaleFactor_MUON",
                   "scaleFactor_LepTRIGGER", "scaleFactor_PILEUP", "mcWeight",
                   "scale", "xsecs", "sumws", "lumi"});

  // Book one histogram per category
  auto categoryIs = [](const std::string &wanted) {
    return [wanted](const std::string &cat) { return cat == wanted; };
  };
  // Using the lambda defined above, create one histogram per sample category, all
  // weighted with the "weight" column defined above
  // All histograms share the following TH1DModel
  auto histogram_model = ROOT::RDF::TH1DModel(/*category*/, "m4l", 24, 80, 170);

  auto df_higgs = /*create histogram for higgs*/;
  auto df_zz = /*create histogram for zz*/;
  auto df_other = /*create histogram for other*/;

  // Histogram of data
  // For data, remember to exclude the entries where reweighting is true and
  // also define the weight equal to 1 and the same histogram model as the others
  auto df_h_mass_data = /*create histogram for data*/;

  // Systematic uncertainty: linear interpolation of the electron scale-factor
  // uncertainty vs pT.
  const std::vector<double> x{5.50e3,  5.52e3,  12.54e3, 17.43e3,
                              22.40e3, 27.48e3, 30e3,    10000e3};
  const std::vector<double> y{0.06628, 0.06395, 0.06396, 0.03372,
                              0.02441, 0.01403, 0,       0};
  TGraph graph(x.size(), x.data(), y.data());

  auto df_with_variations_mc =
      df_mc
          .Vary("weight",
                [&graph](float w, const RVecF &pt, const RVecU &type) {
                  const float v = ROOT::VecOps::Mean(
                      ROOT::VecOps::Map(pt[type == 11], [&graph](float p) {
                        return graph.Eval(p);
                      }));
                  return RVecF{(1 + v) * w, (1 - v) * w};
                },
                {"weight", "goodlep_pt", "goodlep_type"}, {"up", "down"})
          .Histo1D<float, float>(
              ROOT::RDF::TH1DModel("Invariant Mass", "m4l", 24, 80, 170), "m4l",
              "weight");

  auto histos_mc =
      ROOT::RDF::Experimental::VariationsFor(df_with_variations_mc);

  for (int i = 1; i <= histos_mc["nominal"].GetXaxis()->GetNbins(); ++i) {
    histos_mc["nominal"].SetBinError(i,
                                     histos_mc["weight:up"].GetBinContent(i) -
                                         histos_mc["nominal"].GetBinContent(i));
  }

  plot(*df_other, *df_zz, *df_higgs, histos_mc["nominal"],
       histos_mc["weight:up"], histos_mc["weight:down"], *df_h_mass_data);
}

int main() {
  exercise_03_higgs_to_four_leptons_solution();
  return 0;
}
