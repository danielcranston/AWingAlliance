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
sudo apt-get install build-essential cmake freeglut3-dev libglew-dev libglm-dev
```

Cloning the repository, building the project and running the executable:

```
git clone --recurse-submodules https://github.com/danielcranston/AWingAlliance
cd AWingAlliance
mkdir build
./run.sh
```

## Acknowledgments

The following (mostly header-only) libraries are used:

* https://github.com/syoyo/tinyobjloader for loading models.
* https://github.com/nothings/stb for loading textures.
* https://github.com/Reputeless/PerlinNoise for generating random terrain.
* https://github.com/nlohmann/json for parsing scenario config files.
* https://github.com/BehaviorTree/BehaviorTree.CPP for AI behavior
