# signals

A simple [signals and slots](https://en.wikipedia.org/wiki/Signals_and_slots) library
inspired by [boost::signals2](https://www.boost.org/doc/libs/release/libs/signals2/)

[![pipeline status](https://gitlab.com/antenous/signals/badges/master/pipeline.svg)](https://gitlab.com/antenous/signals/-/commits/master)
[![coverage report](https://gitlab.com/antenous/signals/badges/master/coverage.svg)](https://gitlab.com/antenous/signals/-/commits/master)

## Getting started

The following tools are needed to configure and build the project:
* [CMake](https://cmake.org/) version 3.15 or later
* [gcc](https://gcc.gnu.org/) version 5 or later
* [git](https://git-scm.com/)
* [Make](https://www.gnu.org/software/make/) or [Ninja](https://ninja-build.org/)

Tools used to measure code coverage:
* [lcov](http://ltp.sourceforge.net/coverage/lcov.php)
* [gcovr](https://gcovr.com)

Other tools used in this project:
* [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) version 14
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

Both `GNU` and `Clang` support colored diagnostics but depending on the build
environment the colored output may not be enabled by default. To force colored
diagnostics configure the project with `COLOR_DIAGNOSTICS=On`.

```sh
$ cmake -DCOLOR_DIAGNOSTICS=On build/
```

### Building with MSVC and Ninja on Windows

Install [Ninja](https://ninja-build.org/) (and [ccache](https://ccache.dev/)) in
`/usr/bin/` (or `C:/Program Files/Git/usr/bin`). Prepare the build environment
by running the `vcvarsall.bat` script.

> **NOTE!** In [Git BASH](https://gitforwindows.org/), the environment variables
set by the batch script are not propagated to the running shell. To work around
this, run the command below.

```sh
$ eval "$(MSYS_NO_PATHCONV=1 cmd "/C "C:/Program Files \(x86\)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" amd64 > nul && \
  bash -c 'export -p'")"
```

To build the project run the below commands.

```sh
$ cmake -S . -B build/msvc -G Ninja
$ cmake --build build/msvc
```

## Installing

By default, the `install` target installs the library in the `/usr/local/lib/` folder.
To change the installation directory, configure the project with `CMAKE_INSTALL_PREFIX`.

```sh
$ cmake -DCMAKE_INSTALL_PREFIX=build/ -DCMAKE_BUILD_TYPE=Release build/
$ cmake --build build/ --target install
```

The above command installs the library in the `build/lib` subdirectory.

> **NOTE!** `install` target may require root permissions and is only available
with `Release` builds

To uninstall, build the `uninstall` target or run
`xargs rm < build/install_manifest.txt`.

To use the installed library with `my-target`, add the following lines to the
`CMakeLists.txt` file.

```cmake
find_package(signals REQUIRED)
target_link_libraries(my-target signals::signals)
```

## Packaging

CMake comes with [CPack](https://cmake.org/cmake/help/latest/module/CPack.html),
a cross-platform software packaging tool that makes it easy to create distributable
packages. To create a binary package, run the command below.

```sh
$ cmake --build build --target package
```

To build a source package, run the command below.

```sh
$ cmake --build build --target package_source
```

## Testing

This project is being developed using [Test-driven development (TDD)](https://en.wikipedia.org/wiki/Test-driven_development).
Tests are written using the [GoogleTest](https://github.com/google/googletest)
C++ test framework, which is fetched at configure time and built along with the tests.

To run unit tests, build the `check` target.

```sh
$ cmake --build build/ --target check
```

This builds and runs unit tests and shows a summary of the results.
A more detailed message is shown if any of the tests fail.

> **NOTE!** Unit tests are disabled by defauld when used as a subproject.
To enable unit tests, configure the project with `SIGNALS_TEST=On`.

### Code coverage

To measure code coverage, configure the project with
`CODE_COVERAGE=On` and then build the `code-coverage` target.

```sh
$ cmake -DCODE_COVERAGE=On build/
$ cmake --build build/ --target check-coverage
```

This generates a code coverage report in the `build/signals-coverage`
folder. View `build/signals-coverage/index.html` to see the results.

> **NOTE!** Enabling code coverage forces the build type to be `Debug`

## License

signals is distributed under the MIT
[license](https://gitlab.com/antenous/signals/-/blob/master/LICENSE).
