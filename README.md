# ECE-6122 Advanced Programming

[![Build](https://github.com/CandiedCode/ECE-6122/actions/workflows/build.yml/badge.svg)](https://github.com/CandiedCode/ECE-6122/actions/workflows/build.yml)
[![clang-format](https://img.shields.io/badge/code%20style-clang--format-blue)](https://clang.llvm.org/docs/ClangFormat/)
[![clang-tidy](https://img.shields.io/badge/linter-clang--tidy-green)](https://clang.llvm.org/extra/clang-tidy/)
[![cpp-linter](https://img.shields.io/badge/cpplint-enabled-brightgreen)](https://github.com/cpp-linter/cpp-linter-action)

## Load modules on [Pace](https://ondemand-ice.pace.gatech.edu/)

```shell
# Load latest version of gcc available on Pace
module load zstd/1.5.6-5ktxetg
module load gcc/14.2.0
module load cmake/3.26.3
```

## Build

To build code in this repo run

```shell
# For debug build
make build/debug

# For release build
make build/release
```
