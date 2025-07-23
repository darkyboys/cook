# Cook Build System
<img src="img/cook Banner.png">

*A Perfect Build Systen For Your C And C++ Projects!*. Cook is an Open Source Fast Build System designed for linux in mind, But can seamlessly work on Windows with the help of WSL, And even natively but with some features missing like pkg_in won't be supported , Incremental builds won't be supported as both of these relies on linux / UNIX Environment , Cook also can't be installed on windows directly as it's installation script strictly follows the Linux Standards. So if you are using WSL you may have some limitations but not too much compared to natively compiling from source code. So with that's in mind let's get started

---

## Why Would You Need Cook As Your Build System When Make And CMake Already Exists?
You need cook when you are working on a very complex project and you just want to write very simple build files but also very functional with very high redality , Cook comes in there as it uses the [H699](https://github.com/darkyboys/hell6.99mo) as it's scripting language for it's build files , Which makes it insanely redable for large scale applications , Then cook build system is roughlt 50% faster than CMake + Make at compiling the same project and the difference increases with the source files, Also Cook is fully open source and highly optimised for compiling C and C++ programs especially with GNU GCC as it is fully written with the C++ STL and H699 Library for it's Scripting Language.

---

## When You May Avoid Using Cook
When your project is outside of C and C++ , Cook can't be used as it's designed specifically for C and C++ compilers but it can work with other compilers who shares similar structure. Cook can't compile libraries means if you want to use libraries then don't use Cook , It just can't make any libraries as of it's current version , In future library support may be added , Small and Passion driven project (Cook don't have a big community and is alive due to it's passionate developer community which makes this build system awesome) But if your need is a community or something more "premium" then Cook might not be suitable for you.

---

# BenchMarks
These benchmarks directly compares the Cook Build System with existing build systems like CMake and Make.

## Speed Test - Cook Vs CMake + Make
This test compiles two files inside the src directory named `main.cc` and `cli.cc` , Main.cc is a gtk application and cli.cc is a normal C++ iostream / STL Application.

`main.cc`:
```cxx
#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Hello GTK3");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);

    GtkWidget *label = gtk_label_new("Hello, World!");
    gtk_container_add(GTK_CONTAINER(window), label);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

```

`cli.cc`
```cxx
#include <iostream>

int main(int argc, char *argv[]){
    std::cout << "Hello world 2025\n";
}

```

### Cook
```recipe
src/cli.cc:
    out = "cli"

src/main.cc:
    pkg_in = "gtk+-3.0"
    out = "main"
```

**Time taken:**
*First Build*
```bash
real    0m0.418s
user    0m0.346s
sys     0m0.068s
```

*Second Build*
```bash
real    0m0.008s
user    0m0.004s
sys     0m0.004s
```

### CMake
This is slightly different as cmake always makes a platform dependent build file for various operating systems , In My case it made a `MakeFile` which is for the make build system as i used Arch Linux for this bench mark and i needed solid results so i directly tested the build time but writting a `build.sh` containing just: 
```bash
cmake .
make
```

Now i ran the `build.sh` with time command to meassure the performance and it was insaane , Here's the bench marks.

**Time taken:**
*First Build*
```bash
real    0m0.800s
user    0m0.578s
sys     0m0.221s
```

*Second Build*
```bash
real    0m0.075s
user    0m0.040s
sys     0m0.034s
```

This happened because CMake roughly took 0.394s to generate a Make File but then with Make it was almost close to `cook` and slightly faster around 0.015s something.

*So this test shows that cook is faster than CMake + Make combined , by almost 52% on normal builds and i guess almost 60% in incremental builds, Overall this is rough calculation.*

---

## Documentation
This documentation expects that you knows the basics of how to write a `H699` File , In case you don't know then it's always a good idea to check it's documentation which is [here](https://github.com/darkyboys/hell6.99mo).

### Structure of Cook Project
The Cook project is seperated into 2 parts:
 - Recipe
 - CookShell

The `Recipe` is a file named `recipe` which tells the Cook Build System to how to compile C++ source files and the `CookShell` is a file named `cookshell` giving you a virtual shell which also supports Cook Syntax and it is very powerful in right hands. 

*You can use `recipe` to make C++ files and `cookshell` to making custom commands like installs etc*

For example:
```txt
|- src (directory)
|-- C++ Source Files ...
|- install (directory)
|-- cookshell (CookShell File containing normal shell commands for installation like ls, cd or anything for your real shell)
|- recipe (The File telling cook build system what to build and how to build it)
```

Now you can just write `cook` in there shell to execute the `recipe` and `cook install` to execute the `cookshell`.

### Build types & Argument Format
The Cook build system official implementation allows you to build in 2 ways!
 - Normal Build
 - Incremental Build

**Normal Build** is just normally the build where all your source files will be compiled no matter what.
**Incremental Build** is a build type where only the required files will be build , Other files won't, For example you just updated a single file then only that file will be built others won't. (This dramatically saves a lots of time in big projects)

Now the default build type the Cook Build System uses is incremental build but in case you want Normal Build , You can say `cook path/to/recipe -i or --increment false , Because false turns off the incremental builds and true turns them on and by default it's set to true. 

**Argument Format**, Cook follows a simple argument format , Where `cook` as the first argument is compulsory to launch it and 2nd argument of your shell is the path to the recipe file cook will look for , In case it can't find recipe , It will look for cookshell , if cookshell is also not there then it will throw error. And the remaining arguments are just optional arguments like `-i`.

### Targets
Targets are your complete source file configuration block which are converted to equivalent shell commands by the Cook Build System. (In case incremental build is applied to a target then the entire block will be ignored)

### Defining Targets and Source Files
This is the part where we will learn about how can we tell cook about out files, Please remember that you must know the basics of `H699` in order to understand this part.

**Any Scope is the file and a target**: This means that any scope of `H699` is considered as the file name for the source file and defines the entry point of a target.
**All the children keys of a scope are the values to the target**: This means that any known key / value pair you will pass in `H699` is considered a configuration for the source file. (If this feels confusing please remember that you must know the `H699` basics at the least).

Here's an example:
```recipe
cli.cc:
    out = "cli"
```

Ignore out but this defines a simple file named `cli.cc` with the output name `cli` , And this is what the cook build system will make.

### Global target
`global` is a special target in the cook recipe file which tells the cook build system that this is not a source file but still you must remember the configurations from here , Means anything inside the global key will be applied to all the scopes , Means you can define the config once and just say `filename.cc: out = "filename.out"` in your recipe for every file because anything defined in global will be automatically applied to other scopes too.


#### Known Keys
These are very important as they defines the main rules your compiler will follow to compile your files.

##### Types of Known Keys
These are in 2 types:
- Static
- Dynamic

**Static** keys are those who only accepts a single type eg string.
**Dynamic** keys are those who supports more than one types. eg a key can store both string and array.

*Let's start learning these keys and write our first recipe*

##### out - Static string key
`out` is a key which tells the Cook Build System the name of your executable, This is optional as if you don't pass it the Cook Build System is intellegent enough to fetch the name from your source file's name without `.any extension`.

##### bin - Static string key
`bin` is a key which tells the Cook Build System the name of the directory where all the compiled binaries will be stored , Default is `bin`.

##### compiler - Static string key
`compiler` is a key which tells the Cook Build System the compiler you want to use to compile all your source files , Default is `g++`.

##### compiler_arguments - Static string key
`compiler_arguments` is a key which tells the Cook Build System the arguments you want to give to your compiler , Default is empty.

##### system - Static string key
`system` is a key which tells the Cook Build System the command you want to execute when a target is built , This won't run if the target is skipped in incremental builds , Default is empty.

##### psystem - Static string key
`psystem` is a key which tells the Cook Build System the command you want to execute when a target is built , This won't run if the target is skipped in incremental builds , The only difference in between this and system is that this runs before the compiler and system runs after the compiler which is why it stands for primary system, Default is empty.

##### show_logs - Static boolean key
`show_logs` is a key which tells the Cook Build System to show the logs or not. Default is false.

**Dynamic Array Type** These types supports _rem and _add as suffix to remove anything or add anything to them respectively.

##### pkg_in - Dynamic String / Array Key
`pkg_in` is a key which tells the Cook Build System to look for pkg-config packages in your system if the packages aren't found the Cook Build System will throw error for package not found and hault the compilation. If you want a single package use string othervise for multiple packages feel free to use array.

##### include - Dynamic String / Array Key
`include` is a key which tells the Cook Build System to mark include directories for the compiler via `-I` flag. This also allows single string and arrays of strings.

##### lib - Dynamic String / Array Key
`lib` is a key which tells the Cook Build System to mark libraries for the compiler via `-l` flag. This also allows single string and arrays of strings.

##### combine - Dynamic String / Array Key
`combine` is a key which tells the Cook Build System to combine multiple C++ source files together to form a single binary , Remember incremental build will only apply to main binary not on combines ones , This also allows single string and arrays of strings.

*Note: Feel free to use _rem and _add to modify the array values like , pkg_in = ["gtk+-3.0", "webkit2gtk-4.0"] now you wanna remove gtk+-3.0 use pkg_in_rem = "gtk+-3.0" and if you want to add use pkg_in_add instead and feel free to give these array or single string as they are dynamic keys*.

This documentation covers the basics of Cook 1 Build System , for future updates check other docs.


## Contributing ?
Fork this repo , And add new features with only the C++ STL and make a seperate branch for you contribution and make a pull request!

## Thanks for reading
This docs was written by ghgltggamer at 9:55PM IST, 7/21/25.