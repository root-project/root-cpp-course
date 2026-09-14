#include <ROOT/RDataFrame.hxx>

#include <iostream>

/// An action helper that follows the interface as described at
/// https://root.cern/doc/v640/classROOT_1_1RDF_1_1RInterface.html#a49987953d1879d7b8d8d8d37dc62c9d6
template <typename T>
class MaxActionHelper final
    : public ROOT::Detail::RDF::RActionImpl<MaxActionHelper<T>> {
  std::shared_ptr<T> fMax{std::make_shared<T>()}; // final result
public:
  using Result_t = T;
  std::shared_ptr<Result_t> GetResultPtr() const { return fMax; }
  MaxActionHelper() {}

  // Move constructor required by the RDataFrame specs, other special member
  // methods are implemented to follow rule of five
  MaxActionHelper(MaxActionHelper &&) = default;
  MaxActionHelper &operator=(MaxActionHelper &&) = default;
  MaxActionHelper(const MaxActionHelper &) = delete;
  MaxActionHelper &operator=(const MaxActionHelper &) = delete;
  ~MaxActionHelper() = default;

  // value is the value of the column being read by RDataFrame and given to
  // this action helper. This is the core of the computation done by this helper
  void Exec(unsigned int /*slot*/, T value) {
    if (value > *fMax)
      *fMax = value;
  }

  // Called once before starting the computation graph execution, nothing to do
  // for this class
  void Initialize() {}
  // Called within a work task before processing entries, nothing to do for this
  // class
  void InitTask(TTreeReader *, unsigned int) {}
  // Called once at the end of the computation graph execution, nothing to do
  // for this class
  void Finalize() {}

  std::string GetActionName() { return "MaxActionHelper"; }
};

void example_05_custom_action() {
  ROOT::RDataFrame df{100};

  auto max = df.Book<ULong64_t>(MaxActionHelper<ULong64_t>{}, {"rdfentry_"});

  std::cout << "Max entry is " << *max << "\n";
}

int main() {
  example_05_custom_action();
  return 0;
}
