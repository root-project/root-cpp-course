#ifdef __ROOTCLING__
#pragma link C++ class analysis::Muon + ;
// Also request dictionary for vector so that it can be written to disk
#pragma link C++ class std::vector < analysis::Muon> + ;
#endif
