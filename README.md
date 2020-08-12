# Jinx ![CI](https://github.com/JamesBoer/Jinx/workflows/CI/badge.svg)


Jinx is a lightweight embeddable scripting language, written in modern C++, intended to be compiled and used from within a host application.  The Jinx language is clean and simple, looking like a cross between pseudo-code and natural language phrases, thanks to its highly flexible function syntax and identifiers that can incorporate whitespace. 

It was inspired by languages such as Lua, Python, C++, AppleScript, and a few others.  A significant design goal was to make Jinx not only simple to use as a language, but also to make it straightforward to integrate and use via the native API.  Jinx scripts are designed to be executed asynchronously as co-routines, or even on separate threads, since the runtime is thread-safe.  The language also uses no garbage collection, ensuring predictable CPU usage.  This makes it a potential candidate for real-time applications like videogames.

You can [visit the main Jinx website here](http://www.jinx-lang.org/), or [see recent changes](https://github.com/JamesBoer/Jinx/blob/master/Docs/Changelog.md)

## Getting Started with Jinx

Jinx requires a C++ 17 compliant compiler to build, and it is available both as a traditional or header-only library.  

Jinx supports the CMake build system, so for CMake users, adding Jinx should be as simple as pointing your build scripts to the root folder of the project.  Stand-alone build scripts for several platforms/IDEs are found in the ```Bin``` folder.  Alternatively, to build the library manually, copy all the files within the ```Source``` folder to your project, add them via your IDE or build system of choice, and include ```Source/Jinx.h``` from your own source files.  This should be reasonably simple, since the entire library is contained in a single folder.

Using the header-only version is even simple.  Include the header file ```Include/Jinx.hpp``` from your own source, which contains the entire library in a single amalgamated header.

## Your First Jinx Script

Jinx is incredibly simple to use, requiring only a few lines of C++ code to compile and execute your first script.  Here's a Hello, World that not only demonstrates the Jinx script, but all the C++ code required as well.

``` c++
#include "Jinx.h"
    
// Create the Jinx runtime object
auto runtime = Jinx::CreateRuntime();
    
// Text containing our Jinx script
const char * scriptText =
u8R"(
    
-- Use the core library
import core
    
-- Write to the debug output
write line "Hello, world!"
    
)";
    
// Create and execute a script object
auto script = runtime->ExecuteScript(scriptText);
```

There is a [more comprehensive tutorial](https://www.jinx-lang.org/Tutorial.pdf) on the Jinx website for anyone who wants to dive a bit deeper into the language.

## Jinx Status

Jinx was released as 1.0.0 (stable) on January 1, 2019, after three years of development, refinement, and testing.  Additional work, for the immediate future, will be restricted to bug fixing or critical refinements, with the intention of retaining backwards compatibility.

