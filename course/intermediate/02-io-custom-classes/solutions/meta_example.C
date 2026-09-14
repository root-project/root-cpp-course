void meta_example(){
    auto c = TClass::GetClass("myVector");
    auto dataMembers = c->GetListOfDataMembers();
    auto m = dynamic_cast<TDataMember*>(dataMembers->FindObject("m_mag"));
    bool isPersistent = m->IsPersistent();
    cout << "Data member " << m->GetName() << " of class " << c->GetName() << " is " << (isPersistent ? "not " : "") << "transient." << endl;
}
