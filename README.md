# signals

A simple [signals and slots](https://en.wikipedia.org/wiki/Signals_and_slots) library
inspired by [boost::signals2](https://www.boost.org/doc/libs/release/libs/signals2/)

[![pipeline status](https://gitlab.com/antenous/signals/badges/master/pipeline.svg)](https://gitlab.com/antenous/signals/-/commits/master)
[![coverage report](https://gitlab.com/antenous/signals/badges/master/coverage.svg)](https://gitlab.com/antenous/signals/-/commits/master)

## Overview

`signals` is a lightweight C++20 signals/slots library for observer-style communication.

It provides:
* `Signal<Signature>` for connecting and emitting callables
* `Connection` and `ScopedConnection` for manual and RAII disconnection
* `Event<T, Signature>` as a typed event wrapper on top of `Signal`
* customizable result combiners (`DefaultCombiner` returns the last slot result for non-`void` signals)

## Getting started

The following tools are needed to configure and build the project:
* [CMake](https://cmake.org/) (version 3.15 or later)
* [gcc](https://gcc.gnu.org/) (version 11 or later) or
  [Clang](https://clang.llvm.org/) (version 16 or later)
* [git](https://git-scm.com/)
* [Make](https://www.gnu.org/software/make/) or
  [Ninja](https://ninja-build.org/)

Tools used to measure code coverage:
* [lcov](http://ltp.sourceforge.net/coverage/lcov.php)
* [gcovr](https://gcovr.com)

Other tools used in this project:
* [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) (version 16)
* [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) (version 17)
* [GoogleTest](https://github.com/google/googletest) (fetched at configure time)

## Usage

### Signal

Connect one slot and emit a signal with no arguments.

```cpp
#include <signals/Signal.hpp>

auto signal = signals::Signal<void()>{};

signal.connect([] {
    // called on emit
});

signal();
```

### Signal with parameters and connection lifetime

Emit to multiple slots with shared lvalue state, then disconnect explicitly.

```cpp
#include <signals/Signal.hpp>
#include <signals/ScopedConnection.hpp>

auto signal = signals::Signal<void(int&)>{};
auto value = 1;

auto connection = signal.connect([](int& n) {
    n *= 2;
});
signal.connect([](int& n) {
    n += 3;
});

signal(value); // value == 5

connection.disconnect(); // disconnect one slot

signals::ScopedConnection scoped = signal.connect([](int& n) {
    n += 10;
}); // disconnected automatically when scoped goes out of scope
```

### Releasing scoped ownership

Transfer connection ownership from RAII scope back to manual lifetime management.

```cpp
auto signal = signals::Signal<void()>{};
signals::ScopedConnection scoped = signal.connect([] {});

signals::Connection manual = scoped.release();
// `scoped` no longer owns the connection
// `manual` stays connected until it is disconnected or destroyed
```

### Event wrapper

Use a typed event facade with static subscription and instance emission.

```cpp
#include <signals/Event.hpp>

struct NumberEvent : signals::Event<NumberEvent, bool(int)>
{
};

auto subscription = NumberEvent::subscribe([](int n) {
    return n == 42;
});

auto event = NumberEvent{};
const auto ok = event(42); // true
```

### Result combiner

Use the default "last-result wins" behavior or provide a custom aggregation policy.

`Signal<R(...)>` uses `DefaultCombiner<R>` by default. For non-`void` results,
the value from the last connected slot is returned.

```cpp
auto last = signals::Signal<int()>{};
last.connect([] { return 1; });
last.connect([] { return 2; });
const auto result = last(); // 2
```

Custom combiners are supported:

```cpp
template<typename R>
struct Sum
{
    template<typename Slots>
    auto operator()(Slots slots) const
    {
        auto sum = R{};
        for (auto& slot : slots)
            sum += std::invoke(*slot);
        return sum;
    }
};

auto sum = signals::Signal<int(), Sum<int>>{};
```

### Signature restriction

Use by-value or lvalue-reference parameter types for slot signatures.

`signals::Slot<R(Args...)>` does not support rvalue-reference parameters (`T&&`)
in the signature. Use by-value (`T`), const-reference (`const T&`) or lvalue-reference
(`T&`) parameters.

## API behavior

* Slots are invoked in connection order.
* Only currently connected slots are invoked.
* Emission iterates an immutable snapshot of the slot list:
  * connecting a new slot during emission does not invoke it in the same emission;
  * disconnecting a slot during emission prevents subsequent invocation in that same emission.
* `Signal` is move-only (non-copyable); moving transfers slot ownership.
* With `DefaultCombiner<R>`, the return value of the last connected slot is returned for non-`void` signals.

## Thread safety

`Signal`, `Connection`, and `ScopedConnection` are not internally synchronized.
External synchronization is required when using the same signal/connection objects
across threads.

## Configuration options

| Option | Default | Applies when | Description |
| --- | --- | --- | --- |
| `BUILD_SHARED_LIBS` | `OFF` | always | Build shared library instead of static. |
| `SIGNALS_TEST` | `ON` (standalone), `OFF` (subproject) | configure | Enable unit tests and `check` target. |
| `CODE_COVERAGE` | `OFF` | when tests are enabled | Enable coverage instrumentation and `check-coverage` target. |
| `COLOR_DIAGNOSTICS` | `OFF` | GNU/Clang | Force compiler diagnostics color output. |
| `CMAKE_INSTALL_PREFIX` | platform default (for example `/usr/local`) | install | Install location prefix for `install` target. |

## Static analysis

Run `clang-tidy` using the repository config (`.clang-tidy`).

First configure a build directory with `compile_commands.json`:

```sh
$ cmake -S . -B build/tidy -G Ninja -DSIGNALS_TEST=On -DCMAKE_EXPORT_COMPILE_COMMANDS=On
```

Then run clang-tidy:

```sh
$ run-clang-tidy -p build/tidy '.*/(src|tst)/.*\.cpp$'
```

To apply suggested fixes automatically:

```sh
$ run-clang-tidy -fix -p build/tidy '.*/(src|tst)/.*\.cpp$'
```

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
$ cmake -S . -B build/ -DBUILD_SHARED_LIBS=On
```

Both `GNU` and `Clang` support colored diagnostics but depending on the build
environment the colored output may not be enabled by default. To force colored
diagnostics configure the project with `COLOR_DIAGNOSTICS=On`.

```sh
$ cmake -S . -B build/ -DCOLOR_DIAGNOSTICS=On
```

### Building with MSVC and Ninja on Windows

Install [Ninja](https://ninja-build.org/) (and [ccache](https://ccache.dev/)) in
`/usr/bin/` (or `C:/Program Files/Git/usr/bin`). Run the one-liner below once
per shell session to import the MSVC toolchain environment (`PATH`, `INCLUDE`,
`LIB`, ...):

```sh
$ ProgramFiles='Program Files (x86)'; \
  batch=$(find "/c/${ProgramFiles}" -name vcvarsall.bat -printf "C:/${ProgramFiles}/%P\n" -quit 2>/dev/null); \
  eval "$(MSYS_NO_PATHCONV=1 cmd.exe /C "\"${batch}\" amd64 > nul && bash -c 'export -p'")"
```

After that, configure and build with Ninja:

```sh
$ cmake -S . -B build/msvc -G Ninja
$ cmake --build build/msvc
```

## Installing

By default, the `install` target installs the library in the `/usr/local/lib/` folder.
To change the installation directory, configure the project with `CMAKE_INSTALL_PREFIX`.

```sh
$ cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=build/ -DCMAKE_BUILD_TYPE=Release
$ cmake --build build/ --target install
```

The above command installs the library in the `build/lib` subdirectory.

> **NOTE!** `install` may require elevated permissions when installing to
system directories such as `/usr/local`.

To uninstall, build the `uninstall` target or run
`xargs rm < build/install_manifest.txt`.

## Consumer integration

### Use installed package

To use the installed library with `my-target`, add the following lines to
`CMakeLists.txt`:

```cmake
find_package(signals REQUIRED)
target_link_libraries(my-target PRIVATE signals::signals)
```

### Use as a subproject with FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(signals
    GIT_REPOSITORY https://gitlab.com/antenous/signals.git
    GIT_TAG v2.1.1)

set(SIGNALS_TEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(signals)

target_link_libraries(my-target PRIVATE signals::signals)
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

> **NOTE!** Unit tests are disabled by default when used as a subproject.
To enable unit tests, configure the project with `SIGNALS_TEST=On`.

### Code coverage

To measure code coverage, configure the project with
`CODE_COVERAGE=On` and then build the `check-coverage` target.

```sh
$ cmake -S . -B build/ -DCODE_COVERAGE=On
$ cmake --build build/ --target check-coverage
```

This generates a code coverage report in the `build/signals-coverage`
folder. View `build/signals-coverage/index.html` to see the results.

> **NOTE!** Enabling code coverage forces the build type to be `Debug`

## License

signals is distributed under the MIT
[license](https://gitlab.com/antenous/signals/-/blob/master/LICENSE).
