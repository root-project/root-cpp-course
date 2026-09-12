void write() {
    
    myVector v(1, 2, 3);
    std::vector<myVector> vs{{1,2,3},{4,5,6},{7,8,9}};

    auto file = std::unique_ptr<TFile>(TFile::Open("myVector_file.root", "RECREATE"));
    file->WriteObject(&v, "v");
    file->WriteObject(&vs, "vs");
    file->Close();

}
