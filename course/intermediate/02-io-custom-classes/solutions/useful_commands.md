Create the dictionary:
```
rootcling -f dict.cc -rmf myVector.rootmap -rml libmyVector.so myVector.h LinkDef.h
```

Compile the library:
```
g++ -shared -fPIC -o libmyVector.so dict.cc myVector.cc `root-config --cflags --libs`
```

Check the content of the ROOT file:
```
rootls -l myVector_file.root
```
