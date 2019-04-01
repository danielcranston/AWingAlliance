# AWingAlliance

A toy OpenGL project in C++ using FreeGLUT and the GLM library

### Dependencies


* [GLEW:](http://glew.sourceforge.net/) The OpenGL Extension Wrangler Library
* [FreeGLUT](http://freeglut.sourceforge.net/)
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - OpenGL Mathematics


### Installing

This guide assumes you are on a Linux system using the apt package manager

Installing dependencies:

```
sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install freeglut3-dev
sudo apt-get install libglew-dev
sudo apt-get install libglm-dev
```

Cloning the repository and building the project:

```
git clone https://github.com/danielcranston/AWingAlliance
cd AWingAlliance
mkdir build
cd build
cmake ..
make
```

## Authors

* **Daniel Cranston** - *Initial work* - [https://github.com/danielcranston](https://github.com/danielcranston)

## Acknowledgments

Code for loading shaders and importing .obj and.tga files come from [https://computer-graphics.se/](https://computer-graphics.se/).

