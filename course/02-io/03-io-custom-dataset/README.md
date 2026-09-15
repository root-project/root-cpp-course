# I/O of a custom columnar dataset
In the previous section, we have learned how to create dictionaries for custom classes and make ROOT use them, for example to write their instances on files as single objects, i.e. *row-wise*. In following part of the course, we will learn how to do that *column-wise*, with `TTree`and `RNTuple`, mimicking what LHC and other experiments do to save their data.

> [!IMPORTANT]
> Prepare your setup to use the library you have created in the previous section, or, if that is not possible, look at the solutions and prepare it from scratch.

## Preparing a random dataset
It is never easy to generate a random dataset, which is not complex but interesting enough. You can rely on your experience to accomplish this task at best, or produce points on the surface of a tridimensional sphere.
If you like the idea, this can be a way to proceed:

1. Create two random numbers, `a` and `b`, between 0 and 1.
2. Calculate `$\theta$ = 2$pi$a`
3. Calculate `$\phi$ = arrcos(2b-1)`
4. Calculate the coordinates as follows:
  - `x = sin($\phi$)cos($\theta$)`
  - `y = sin($\phi$)sin($\theta$)`
  - `z = cos($\phi$)`

You can encapsulate this code in a function, producing the `myVector` instances we'll then write on disk.
> [!TIP]
> You can pass an instance of `myVector` by reference to the aforementioned function, which is then filled with the correct coordinates.

If there is not a lot of time for this task, feel free to use the code in the `solutions`directory, namely the `sphereRandomGen.[cpp,h]` files.

## Writing the dataset to a TTree and RNTuple
The title says it all! Prepare a simple compiled program that creates a reasonable number of `myVector` instances, say 100.000, and then writes them in a `TTree`. **This program needs to be linked to the library containing the dictionary and implementation of `myVector`**.
If you are not sure about how to proceed, you can revisit the first exercise of the `I/O` section of this course.

Now we can do the same with RNTuple.

> [!TIP]
> You can inspect the result by creating a `TGraph2D` programmatically, or, if you prefer, inspect the files in `TBrowser`. If you are curious or you cannot access graphical interfaces on the platform on which you are working, just copy the file locally and inspect it with [JSROOT](https://jsroot.gsi.de/latest/) (yes, JSROOT allows you to visulalise local files, too!)

We can get rid of the `rootmap` file and everything works just the same: can you explain why?

## Reading the datasets
We can now write two simple programs, or functions, reading the datasets you have just written. In order to visually check the result, we could plot the points we have just written using the `TGraph2D` class, by drawing an adequately filled instance of it on a `TCanvas` instance, saved as a png image.

Are the results equivalent?

## Bonus: Conversion, compression, file sizes, *splitting* and all that

A dataset in the `TTree` format can be easily converted into the new `RNTuple` format. That can be done with the `RNTupleImporter`class:

```c++
auto importer = ROOT::Experimental::RNTupleImporter::Create(
      "myTreeFile.root", "myTree", "myNTupleFile_imported.root");
  importer->Import();
```
What is the size of the files containing the `RNTuple` and `TTree` instance, before and after the conversion?

The `TFile` class provides a method, `TFile::GetCompressionSettings()`, to programmatically access the compression settings used to write the file. Those are explained [here](https://root.cern.ch/doc/master/structROOT_1_1RCompressionSetting_1_1EDefaults.html#a47faae5d3e4bb7b1941775f764730596aa27e7f29058cc84d676f20aea9b86c30). 

Are those settings the same for TTree and RNTuple? Can you imagine why?

### Splitting one object into multiple columns
When writing in a columnar format, by default, ROOT attempts to *split* objects across multiple columns, i.e. by assigning one column per data member, recursively. This, in the context of `TTree` is referred as to *splitting*. This mechanism provides several advantages, for example the ability to read only certain columns, i.e. not the complete objects, as well as compressing individual columns, which in real-life use cases often contain the value of the same physical quantity.

The `TTree::Branch` method, among its overloads, offers this signature:
```c++
template <class T>
TBranch *TTree::Branch(const char *name, T *obj, Int_t bufsize = 32000,
                       Int_t splitlevel = 99);
```
What happens to the size of the file if the value of `splitlevel`is zero?