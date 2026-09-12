This directory contains examples of basic reading and writing APIs of the ROOT data formats: TTree and RNTuple

Compile and run a single file with:

```
$: g++ -o <example_basename>.out <example_basename>.cpp $(root-config --cflags --glibs)
$: ./<example_basename>.out
```

The `example` programs can also be run with `root` directly as:

```
$: root -l -b -q <example_basename>.cpp
```
