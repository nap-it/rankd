# Rank Protocol Prelude Library

This is a library containing all the collection of names and structures that are required for all instances of Rank during execution or instantiation. This is an internal library since it is not made for external use without the help of other libraries that conditions some of its artifacts.

## Internal Structure of this Library

The `rank-prelude` library is composed mainly by the common data structures that are designed and specified in Rank protocol, as well as utility functions. Moreover, as this repository is an official implementation of the specified architecture (still in validation), this library also contains some external dependencies on some tools and structures that were not designed and implemented by Rank's team of developers: such as JSON's representation, YANG parsing, and other mechanisms that currently resides within the context of the `vendor/` directory in the root of this project.

The structure of the project, as for the rest of the Rank repository, follows the same structure as of the Go's project layout, using the following naming convention:

* `assets`: this directory has some files that are served as a convenient repository or other code examples;
* `docs`: this directory has some documentation artifacts such as articles on the internals of the Rank protocol, as well as some diagrams;
* `include`: this directory has all the header files within the Rank project, directly related to the Rank protocol main library;
* `structs`: this directory has all the implementation of the main structures and enumerations of the Rank's data model;
* `tests`: this directory has some functional tests to units implemented in this library such as utils;
* `utils`: this directory has utility and helper functions that functions as companion tools to the Rank protocol;
* `vendor`: this directory has external tools and dependencies, implementation-wise, to this project.