# Neurofield

This library implements methods to simulate neural field models,
based on Robinson's Neural Field Theory.


## Installation

1. Fork and clone; or,

2. download the latest [release](https://github.com/BrainDynamicsUSYD/neurofield/releases) as a `.zip` or `tar.gz` file and extarct the folder.

3. After doing 1) or 2) type `cd neurofield`.

### Build/compilation

To build the executable on Linux or Mac, open a terminal in the neurofield directory and type:

    make

this produces the executable (`bin/neurofield`). The user-manual is available as `doc/NeurofieldManual.pdf`.

To build the reference manual, generated from the code, type:

    make reference-manual

The html files for the reference-manual will be placed under `doc/html`, point your browser at the index.html file in that directory.

For a brief description of available Makefile targets, type:

    make help



**NOTE**: Ensure you have a compiler that supports the `C++11` standard.  
On Linux: g++ 4.8 or higher; clang 3.7 or higher.
On MacOS: TBD.

## Demos

Example configurations including examples for published results are available in the `configs` folder.

To run the basic example type

    ./bin/neurofield -i ./configs/example.conf -o example.output

## Troubleshooting

Problems compiling? Check [the following page](https://github.com/BrainDynamicsUSYD/neurofield/wiki/Troubleshooting) for the latest information.

## How to contribute code

If you intend to contribute to NeuroField development, please make a private fork of this repository and follow the [instructions for contributors.](https://github.com/BrainDynamicsUSYD/neurofield/wiki/How-to-contribute-code-to-NeuroField)
