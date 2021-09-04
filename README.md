# signals

A simple [signals and slots](https://en.wikipedia.org/wiki/Signals_and_slots) library
inspired by [boost::signals2](https://www.boost.org/doc/libs/release/libs/signals2/)

[![pipeline status](https://gitlab.com/antenous/signals/badges/master/pipeline.svg)](https://gitlab.com/antenous/signals/-/commits/master)

## Getting started

The following tools are needed to configure and build the project:
* [CMake](https://cmake.org/) version 3.14 or later
* [gcc](https://gcc.gnu.org/) version 5 or later
* [git](https://git-scm.com/)
* [Make](https://www.gnu.org/software/make/) or [Ninja](https://ninja-build.org/)

Other tools used in this project:
* [GoogleTest](https://github.com/google/googletest) (fetched at configure time)

## Building

These instructions assume that the project has been cloned into a directory
named `signals`. To configure and build a project, run the commands below. 

```sh
$ cd signals
$ cmake -S . -B build/
$ cmake --build build/
```

This will create a static library named `libsignals.a` in the `build/src/` folder. 

The project builds a static library by default. To build a shared library,
configure the project with `BUILD_SHARED_LIBS=On`.

```sh
$ cmake -DBUILD_SHARED_LIBS=On build/
```

## Testing

To run unit tests, build the `check` target.

```sh
$ cmake --build build/ --target check
```

This builds and runs unit tests and shows a summary of the results.
A more detailed message is shown if any of the tests fail.

> **NOTE!** Unit tests are disabled by defauld when used as a subproject.
To enable unit tests, configure the project with `SIGNALS_TEST=On`.

## License

signals is distributed under the MIT
[license](https://gitlab.com/antenous/signals/-/blob/master/LICENSE).
