# brandy0
brandy0 is a fluid flow simulator.

brandy0 has been started as a maturita project candidate. brandy0 is currently in development and a maturita project candidate (i.e. whether it becomes a maturita project is TBD by October 31, 2020).

## dependencies
Eigen 3.3.7 -- available [here](https://gitlab.com/libeigen/eigen/-/releases)
CImg -- available [here](http://cimg.eu/download.shtml)

## running a simulation
1. install the dependencies (for CImg, download the header file into the source directory)
2. compile one of source which you want to run. E.g.
```
g++ -pthread XXX.cpp -O3 -o XXX -lm -lX11
```
3. run the compiled executable and it will generate output image files into `../output` by default (that folder should exits)
* note: older experimetal versions output image files into `.`
