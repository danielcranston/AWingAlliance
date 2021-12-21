# AWingAlliance

A toy OpenGL project in C++ using SDL2 and Eigen3

[![Build Status][ci-badge]][ci-url]

## Dependencies

Here's a list of the main external libraries used in this project.

| Library      | Description |
| :---------: | :---------: |
| [SDL2](https://github.com/libsdl-org/SDL)      | Simple Direct Media Layer |
| [GLEW](http://glew.sourceforge.net/)           | The OpenGL Extension Wrangler Library |
| [Eigen3](https://gitlab.com/libeigen/eigen)    | C++ template library for linear algebra |
| [Assimp](https://github.com/assimp/assimp)     | Open-Asset-Importer-Library |
| [Alure](https://github.com/kcat/alure)         | A C++ utility library for OpenAL |
| [stb_image](https://github.com/nothings/stb)   | Single-file public domain C/C++ library for loading images |
| [yaml-cpp](https://github.com/jbeder/yaml-cpp) | A YAML parser and emitter in C++  |


## Installation

This assumes you are on a Linux system using the apt package manager.

### Installing main dependencies

Dependencies (except Alure) can be installed easily via apt.

```
sudo apt install build-essential cmake libsdl2-dev libglew-dev libeigen3-dev libassimp-dev libyaml-cpp-dev
```

### Installing Alure

Unfortunately there is no apt package for Alure, so it needs to be cloned, built and _installed_ manually.

```
sudo apt install libopenal-dev libvorbis-dev libopusfile-dev libsndfile1-dev
git clone https://github.com/kcat/alure /tmp/alure && cd /tmp/alure/build
cmake -DCMAKE_INSTALL_INCLUDEDIR=/usr/include -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
sudo cmake --install .
```

**_NOTE:_**  Alure requires its installed headers to land in the same directory as the headers from `libopenal-dev`. On Ubuntu the headers of the latter are installed in `/usr/include/AL` whereas thoes of the former land in `/usr/local/include`. Therefore the `-DCMAKE_INSTALL_INCLUDEDIR=/usr/include` above is necessary, but depending on the system you're using might not be.


## Cloning and building

Execute the following to clone and build the project.

```
git clone https://github.com/danielcranston/AWingAlliance
mkdir AWingAlliance/build && cd AWingAlliance/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```


## Running

Run the main executable with `./awing` from the `build` folder.


[ci-badge]: https://img.shields.io/github/workflow/status/danielcranston/AWingAlliance/CMake/master
[ci-url]: https://github.com/danielcranston/AWingAlliance/actions/workflows/cmake.yml
