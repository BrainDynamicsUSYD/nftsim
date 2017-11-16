# Neurofield

Neurofield is written in C++ and implements streamlined standard methods to solve hyperbolic partial differential equations such as the damped 2D wave equation; time stepping methods to solve ordinary differential equations; and procedures for delay differential equations. Careful numerical analysis has resulted in a suite of methods that is fast, accurate, and robust. Thus the present work obviates the need for users to become familiar with all these techniques and underlying theory, program them, and then debug several thousand lines of code. The code has been tailored to use these state equations to solve an unlimited number of user-defined continuum neural field models. The input and output files are both plain-text, so NeuroField can be easily integrated into existing workflows and analyses written in other programming languages.
NeuroField comes with a collection of custom made Matlab functions that allow for an easy access to the compiled code, for users without prior knowledge of C++. End-users need only to write plain text files as described in the User Manual.


## Installation

1. Fork and clone; or,

2. download the latest [release](https://github.com/BrainDynamicsUSYD/neurofield/releases) as a `.zip` or `tar.gz` file and extract the folder.

3. After doing 1) or 2) type `cd neurofield`.

## Requirements

Make sure you have a compiler that supports the `C++11` standard.  
On Linux: `gcc` 4.8 or higher; `clang 3.7` or higher.
On MacOS: `clang` 3.3 or later version;

Additionally, to build the developer reference manual you'll need `doxygen` and `graphviz`. The latter provides the DOT language.

The user manual is compiled using `pdflatex` an it is also provided as a pdf (`doc/NeurofieldManual.pdf`).


### Compiling and building from the source code

To build the executable on Linux or MacOS, open a terminal in the `neurofield` directory and type:

    make

this produces the executable `bin/neurofield` and autogenerates the documentation. 

To build only the reference manual type:

    make reference-manual

The html files for the reference-manual will be placed under `doc/html`, point your browser at the index.html file in that directory.

For a brief description of available Makefile targets, type:

    make help

Are you on Windows and want to use `NeuroField`. Check [some options here](https://github.com/BrainDynamicsUSYD/neurofield/wiki/All:-Cross-platform-support). 


## Demos

Example configurations including examples for published results are available in the `configs` folder.

To run the basic example type

    ./bin/neurofield -i ./configs/eirs-corticothalamic.conf -o eirs-corticothalamic.output

## Troubleshooting

Problems compiling? First check [the following page](https://github.com/BrainDynamicsUSYD/neurofield/wiki/All:-Cross-platform-support) for the latest information. If that doesn't solve the problem please drop us a message using Github's issues. Don't forget to mention the following info: operating system, compiler, compiler version, `NeuroField` version. 

## How to contribute code

If you intend to contribute to NeuroField development, please make a private fork of this repository and follow the [instructions for contributors.](https://github.com/BrainDynamicsUSYD/neurofield/wiki/How-to-contribute-code-to-NeuroField)


## How to cite `NeuroField`

Liked the code? Did you use it in your research and got awesome results?

Please cite us as follows:
   
   Sanz-Leon, P., Robinson, PA., Knock, SA., Drysdale, PM., Fung, PK., Abeysuriya, GM.,
   Rennie, CJ and Zhao, XL.
   
   _NeuroField: Theory and Simulation of Multiscale Neural Field Dynamics_
   
   presubmitted to PLoS Computational Biology 


