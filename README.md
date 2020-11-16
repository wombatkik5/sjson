# SJSON

This repository contains a preliminary implementation of the paper

*SJSON: A Succinct Representation for JSON Documents*

which is an improved version of [the conference paper](https://doi.org/10.1109/ICDIM.2016.7829787).

### Requisites

This suite is tested on the Linux environment.

This suite requires [the GNU Compiler Collection](http://gcc.gnu.org) for compilation.

This suite assumes that [the SDSL library](https://github.com/simongog/sdsl-lite) is pre-installed.

### Compilation

Run `./compile.sh` in the repository.

This script generates two executable files: `sjson` and `sjson-csa`.

The core representation of those files is identical, except that the string compression is enabled in the latter file (`sjson-csa`).

### Usage

`./sjson <JSON file>`

Once the representation is constructed, type operations such as

- c: navigate to child.

- p: navigate to parent.

- n: list names (valid for objects).

- s: get size (valid for arrays).

- v: get value,

# License

Some of the codes are based on [the RapidJSON library](https://rapidjson.org). Its code is licensed under the MIT license.

In addition, [the Half-precision Floating-point library](https://half.sourceforge.net) is integrated in the representation, which is also licensed under the MIT license.

