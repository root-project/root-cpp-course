Create the dictionary
```
rootcling -f dict.cc -rmf myVector.rootmap -rml libmyVector.so myVector.h LinkDef.h
```
Build the library
```
 g++ -shared -fPIC -o libmyVector.so dict.cc myVector.cc `root-config --cflags --libs`
```
Build the executable
```
clang++ -o main fillRNTuple.cpp fillTree.cpp main.cpp readRNTuple.cpp readTree.cpp sphereRandomGen.cpp `root-config --cflags --libs` -l myVector -L ./
```
