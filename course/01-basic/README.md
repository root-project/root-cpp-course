This directory contains examples of usage of basic ROOT classes such as histograms and files.

Compile and run a single file with:

```
$: g++ -o <example_basename>.out <example_basename>.cpp $(root-config --cflags --glibs)
$: ./<example_basename>.out
```

The `example` programs can also be run with `root` directly as:

```
$: root -l -b -q <example_basename>.cpp
```
