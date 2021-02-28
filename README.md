# brandy0
brandy0 is a fluid dynamics simulator.

brandy0 is a maturita project and is currently in development.

## state of development

#### the start (home) screen as of 2021/02/28
![start screen](./doc/shots/2020-12-04_start_screen.png)

#### the simulation configuration screen as of 2021/02/28
![config screen](./doc/shots/2021-02-28_config_screen.png)

#### the obstacles configuration screen as of 2021/02/28
![obstacles config screen](./doc/shots/2021-02-28_shape_config_screen.png)

#### the simulation screen as of 2021/02/28
![simulation screen](./doc/shots/2021-02-28_simulation_screen.png)

#### the video export screen as of 2021/02/28
![simulation screen](./doc/shots/2021-02-28_video_export_screen.png)

## dependencies
* libavcodec, libavformat, libavutil, libswscale (for video export)
* gtkmm 3 (C++ interfrace for GTK 3)

# the below information relates to older experimental simulations (./src/exper)

## dependencies
* Eigen 3.3.7 -- available [here](https://gitlab.com/libeigen/eigen/-/releases)
* CImg -- available [here](http://cimg.eu/download.shtml)

## running a simulation
1. install the dependencies (for CImg, download the header file into the source directory)
2. compile one of source which you want to run. E.g.
```
g++ -pthread XXX.cpp -O3 -o XXX -lm -lX11
```
3. run the compiled executable and it will generate output image files into `../output` by default (that folder should exits)
* note: older experimetal versions output image files into `.`
