### Quick start

    make ./bin/neurofield -i Configs/example.conf -o example.output
    doxygen Doxyfile
    
The html files will be place under `Documentation/html`
### Setup guide

**Please check the [NeuroField wiki](https://github.com/BrainDynamicsUSYD/neurofield/wiki) for the setup walkthrough. The instructions below are a standalone summary if you do not have internet access. More detailed instructions are also included in the PDF documentation**

1. Ensure you have a compiler that supports the `C++11` standard. We recommend GCC 4.8 or higher.

2. Type `make` to build the binary `Release/NeuroField`. More information about [cross platform for non-Linux](https://github.com/BrainDynamicsUSYD/neurofield/wiki/Cross-platform-support) systems is also available. 

3. Example configurations including examples for published results are available in the `Configs` folder.

### Troubleshooting

Problems compiling? Check [the following page](https://github.com/BrainDynamicsUSYD/neurofield/wiki/Troubleshooting) for the latest information.

### Guidelines for contributing code

If you intend to contribute to NeuroField development, please make a private fork of this repository and follow the [instructions for contributors.](https://github.com/BrainDynamicsUSYD/neurofield/wiki/How-to-contribute-code-to-NeuroField)
