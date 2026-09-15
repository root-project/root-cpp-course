#ifndef EXAMPLE_07_MUON
#define EXAMPLE_07_MUON

#include <Rtypes.h>
#include <vector>

namespace analysis {

struct Muon final {
  int charge{};
  float pt{};
  float eta{};
  float phi{};
  float mass{};

  // Required for proper ROOT I/O
  Muon() {}

  Muon(int charge, float pt, float eta, float phi, float mass)
      : charge(charge), pt(pt), eta(eta), phi(phi), mass(mass) {}

  ClassDef(Muon, 1);
};

float invariant_mass(const std::vector<Muon> &);

} // namespace analysis

#endif // EXAMPLE_07_MUON
