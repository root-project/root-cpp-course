// fit.C — programmatic HistFactory model + fit.
//
// Model:
//   channel "channel", 8 bins in |eta|
//     sample "signal"     (h_sig)       scaled by POI "mu" (NormFactor)
//     sample "background" (h_bkg)       with HistoSys "theta" built from the
//                                       RDF varied histograms
//                                       h_bkg_theta_down / h_bkg_theta_up
//   both samples carry their MC statistical uncertainty (ActivateStatError)
//
// HistFactory names the HistoSys nuisance parameter "alpha_theta": a unit
// Gaussian-constrained interpolation parameter, so that
//     theta = kThetaNom + alpha_theta * kThetaDelta
// with kThetaNom = 1.0, kThetaDelta = 0.4. Data were generated with
// kThetaTrue = 1.3, i.e. alpha_true = +0.75. The fit should land near that.
//
// Run: root -b -q fit.C

#include <iostream>
#include <memory>

#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooFitResult.h"
#include "RooMsgService.h"
#include "RooRealVar.h"
#include "RooStats/HistFactory/MakeModelAndMeasurementsFast.h"
#include "RooStats/HistFactory/Measurement.h"
#include "RooWorkspace.h"
#include "TSystem.h"

#include "physics.h"

void fit() {
  using namespace RooStats::HistFactory;

  gSystem->mkdir("results", /*recursive=*/true);

  Measurement meas("meas", "rdf_histfactory_toy");
  meas.SetOutputFilePrefix("results/meas");
  meas.SetPOI("mu");
  meas.SetLumi(1.0);
  meas.SetLumiRelErr(0.01);
  meas.AddConstantParam("Lumi"); // no luminosity uncertainty in this toy

  Channel chan("channel");
  chan.SetData("h_data", "hists.root");

  Sample sig("signal", "h_sig", "hists.root");
  sig.SetNormalizeByTheory(false);
  sig.AddNormFactor("mu", 1.0, 0.0, 5.0); // POI: multiplies the signal template
  sig.ActivateStatError();

  Sample bkg("background", "h_bkg", "hists.root");
  bkg.SetNormalizeByTheory(false);
  bkg.AddHistoSys("theta", "h_bkg_theta_down", "hists.root", "",
                  "h_bkg_theta_up", "hists.root", "");
  bkg.ActivateStatError();

  chan.AddSample(sig);
  chan.AddSample(bkg);
  meas.AddChannel(chan);
  meas.CollectHistograms();

  auto wsPtr = MakeModelAndMeasurementFast(meas);
  RooWorkspace &w = *wsPtr;

  auto *pdf = w.pdf("simPdf");
  auto *data = w.data("obsData");
  auto *mu = w.var("mu");
  auto *alpha = w.var("alpha_theta");
  if (!pdf || !data || !mu || !alpha) {
    std::cerr << "workspace is missing pdf/data/parameter, dumping it:\n";
    w.Print();
    return;
  }

  std::cout << "\n===== model built, fitting =====\n";
  alpha->setVal(0.); // start the fit at the nominal nuisance value
  RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);
  std::unique_ptr<RooFitResult> res(pdf->fitTo(*data, RooFit::Save(),
                                               RooFit::PrintLevel(-1),
                                               RooFit::PrintEvalErrors(-1)));
  RooMsgService::instance().setGlobalKillBelow(RooFit::DEBUG);

  const double thetaHat = toy::kThetaNom + alpha->getVal() * toy::kThetaDelta;
  const double thetaErr = alpha->getError() * toy::kThetaDelta;

  res->Print("v");
  std::cout << "\n===== fit summary =====\n"
            << "  mu          = " << mu->getVal() << " +/- " << mu->getError()
            << "   (true: " << toy::kMuNom << ")\n"
            << "  alpha_theta = " << alpha->getVal() << " +/- "
            << alpha->getError() << "\n  ==> theta   = " << thetaHat << " +/- "
            << thetaErr
            << "   (true value used for the data: " << toy::kThetaTrue << ")\n";
}
