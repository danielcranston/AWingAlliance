# AWingAlliance

A toy OpenGL project in C++ using SDL2 and Eigen3

## Dependencies

Here's a non-exhaustive list of external libraries used in this project

* [SDL2](https://github.com/libsdl-org/SDL): Simple Direct Media Layer (for OpenGL context)
* [GLEW:](http://glew.sourceforge.net/) The OpenGL Extension Wrangler Library
* [Eigen3](https://gitlab.com/libeigen/eigen): C++ template library for linear algebra
* [Assimp](Open-Asset-Importer-Library): Open-Asset-Importer-Library
* [Alure](https://github.com/kcat/alure): A C++ utility library for OpenAL
* [stb_image](https://github.com/nothings/stb): Single-file public domain (or MIT licensed) C/C++ library for loading images
* [yaml-cpp](https://github.com/jbeder/yaml-cpp): A YAML parser and emitter in C++ 

## Building

This assumes you are on a Linux system using the apt package manager

### Installing dependencies:

#### Main project dependencies

```
sudo apt install build-essential cmake libsdl2-dev libglew-dev libeigen3-dev libassimp-dev libyaml-cpp-dev
```

#### Alure

Unfortunately there is no apt package for `Alure`, so it needs to be cloned, built and _installed_ manually.

```
sudo apt install libopenal-dev libvorbis-dev libopusfile-dev libsndfile1-dev
git clone git@github.com:kcat/alure.git /tmp/alure && cd /tmp/alure/build
cmake -DCMAKE_INSTALL_INCLUDEDIR=/usr/include -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
sudo cmake --install .
```

---

> **_NOTE:_**  Alure requires its installed headers to land in the same directory as the headers from `libopenal-dev`. On Ubuntu the headers of the latter are installed in `/usr/include/AL` whereas the former lands in `/usr/local/include`. Therefore the `-DCMAKE_INSTALL_INCLUDEDIR=/usr/include` above is necessary, but depending on the system you're using might not be.

---

#### Finally, clone the repository and build the project

```
git clone https://github.com/danielcranston/AWingAlliance
mkdir AWingAlliance/build && cd AWingAlliance/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Run the main executable with `./awing`.

