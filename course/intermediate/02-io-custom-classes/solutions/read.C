void read()
{
   auto file = std::unique_ptr<TFile>(TFile::Open("myVector_file.root"));
   auto  v  = file->Get<myVector>("v");
   auto  vs= file->Get<std::vector<myVector>>("vs");

   cout << "Print single object:" << endl;
   v->Print();

   cout << "Print collection:" << endl;
   for (auto &iv : *vs) {
      iv.Print();
   }
}
