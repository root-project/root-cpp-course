# I/O of Custom Classes
With this exercise, we will learn how to create custom classes for IO, exercising the dictionary generation in various ways. We will also see schema evolution in action, as well as how the behaviour of ROOT's I/O can be steered with metadata.

## A minimal data model
We'll create a minimal data model that then we'll write on disk.
Create a class, `myVector`, that represent a three-dimensional vector. For the components, please use a single precision floating point number.
You can make this exercise even more realistic, by creating a mechanism to fill such classes with random numbers, e.g. a function. Do not make the numbers truly random, but reproducible, i.e. by choosing a seed. It will become handy when it's time to read our instances back and check they are sane.
Moreover, you can add a `print()` method to the class, in order to be able to quickly verify its content.

> [!TIP]
> Remember that using the `ClassDef` macro is not strictly necessary to perform I/O, but strongly recommended.

## Write and read custom objects
Write now a ROOT C++ macro to write a `myVector` instance on disk.

*Does this work if you do not create a dictionary? If yes, what could be the problems one may encounter in more complicated setups, for example in presence of evolution of the layout of classes?*

Repeat the above also with a `std::vector<myVector>`. *Is the behaviour the identical as before? Why?*

## Dictionaries
In order to proceed, we'll need now to create dictionaries for the `myVector` and `std::vector<myVector>` classes.
We also want that ROOT is able to autoload the library containing `myVector` implementation and dictionary. Therefore, if you use

1. `rootcling` remember to specify the arguments `-rmf myVector.rootmap --rml libmyVector.so`
2. `genereflex` remember to specify the arguments `--rootmap myVector.rootmap --rootmap-lib libmyVector.so`

where `libmyVector.so` is the name of the library we will use to collect the dictionary and `myVector` implementation.

### Compiling the data model library
Now that we created the dictionary and the associated artifacts, it's time to create the library containing our data model and the associated dictionaries.
In this example, the data model is made of two classes, `myVector` and `std::vector<myVector>`, and the associated dictionaries.
To build a shared library made of [position independent code](https://en.wikipedia.org/wiki/Position-independent_code) with GCC, the command looks like this one

```.bash
g++ -shared -fPIC `root-config --libs --cflags` -o libmyVector.so <source files separated by a space>
```
(In case you prefer to use Clang, no worries: the invocation is identical!)

> [!NOTE]
> The `root-config --libs --cflags` command simply prompts all the necessary flags to correctly obtain binary code using ROOT components (try it, and see what is the result to know more).

Remember: the library, the pcm file, and the rootmap should be either in the `./` directory or in the `LD_LIBRARY_PATH`

## Write and read custom objects, with dictionaries this time
Now we should have all the pieces necessary to write down our custom classes. Try to re-run the macro created to write a `myVector` and `std::vector<myVector>` instances on disk.

*Is ROOT still prompting any error?*

You can list the content of the file easily, by using the `rootls -l myFile.root`. You should be able to see the type of the objects written as well as their names in the file.

If you want to understand more in depth what is going on, you can have a look to the two sections below

### Autoloading
ROOT is using a library we prepared without us explicitly loading it or linking it to any executable program. This is possible thanks to *autoloading*.
You can verify what library is being autoloaded when by slightly increasing the verbosity of ROOT, either with an environment variable, `ROOTDEBUG=1`, or programmatically, by setting the `gDebug` variable to 1 or higher.

You can also remove the rootmap now. Writing will not work any more: the library cannot be loaded. You can re-enable the loading of the dictionaries at least in two ways:
1. Transform the macro into an executable, to which the library containing the dictionary is linked
2. Manually load the library in the code, with the invocation `gSystem->Load("libmyVector.so")`

> [!NOTE]
> ROOT is also used within the software stacks of HEP experiments. At runtime, these systems can easily end up dealing with hundreds of shared libraries. This is also why ROOT provides such a rich support to deal with shared libraries.

## Reading back from files
Now that we have our instances in a ROOT file, we can read them back.
Write a simple macro or a compiled program to read the file we just wrote, and extract the `myVector` and `std::vector<myVector>` instances from it. You can use the `print()` method to easily inspect the value of the coordinates.

### Evolve the on-disk layout of the data model
It often happens that the data model needs to be updated. Even after several datasets have been written. For these cases, requiring a so-called *schema evolution*, ROOT provides all the necessary interfaces. In this exercise, we'll see schema evolution in its simplest form: *automatic schema evolution*, without requiring any action from the user.

We'll apply two transformation to our data model:
1. Add a data member, `mag`, that is a cache for the magnitude of the vector, defaulted to `-1`. Make this member `transient`, i.e. not for writing.
2. Transform the single precision floating point numbers into double precision numbers.

Once the two steps above are complete, new dictionaries and a new library will have to be created. Once this is done

Write a simple macro or program to read back the file we have just written. 

*Are the double precision floating point numbers representing what was written in single precision?*

If you did not forget to increase the class version via the `ClassDef` pre-processor macro, try to use the previous one and read the file: *what is ROOT complaining about?*

### Bonus: More about ROOT's Type System
Let's go back to the evolved class, the one containing a transient data member (`m_mag`).
We'll now use ROOT's type system, *Meta*, to verify that property.

Write a simple program that (auto)loads the library containing the dictionary for the evolved `myVector`, and, through the interface of `TClass`, get to the `m_mag` data member and verify that it is indeed not persistent.
