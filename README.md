# README

The code is implemented as a Microsoft Visual Studio 2019 project. To use, open it in Visual Studio and, possibly, install any necessary add-ons through Visual Studio Installer.

## CONFIGURATION

Project configuration "Release4" is set up to run the code for the experiments.
To run experiments on a subset of the graph families, comment out others in Release4main.cpp.
To run experiments on a subset of the data for each graph family, see mains.cpp.
To run experiments on a subset of the algorithms, comment out others in configure_bm in mains.cpp.

## DATA

Random forest graphs are generated as needed.
Random hyperbolic graphs and real world graphs are stored at "./DATA/".
Random hyperbolic graphs are pregenerated using [NetworKIT](https://networkit.github.io/).
The files are named {a}-{b}-{c}+{d}.txt where the graph has 2^a vertices, average degree 2^b and power law factor c. The number d distinguishes graphs generated with the same parameters.
The exact random hyperbolic graphs used up to 2^17 vertices are included, but not 2^18 vertices.
One out of three real world graphs is included. The other two can be found [here](https://snap.stanford.edu/data/soc-LiveJournal1.html) and [here](https://snap.stanford.edu/data/web-BerkStan.html).
