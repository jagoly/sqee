# SQEE Readme

##### About LibSQEE
LibSQEE is library containing all sorts of useful stuff for 3D applications, particularly games. Features include C++ wrappers for various OpenGL components, a simple text rendering system, some nice classes for Windows and Scenes, a scripting console, loaders for custom model, material and animation formats, a generic resource manager, sound system, simple event handling system, physics bodies and more. LibSQEE intergrates a few third party libraries, namely:

 * [GLM](http://glm.g-truc.net)
 * [SFML](http://www.sfml-dev.org/)
 * [ChaiScript](http://chaiscript.com)
 * [ReactPhysics3D](http://reactphysics3d.com)
 * [STB_Image](https://github.com/nothings/stb)
 * [GLLoadGen](https://bitbucket.org/alfonse/glloadgen)
 * [Option Parser](http://optionparser.sourceforge.net/)
 * [TinyFormat](https://github.com/c42f/tinyformat)

##### About SQEE RPG
SQEE RPG is a not a library, but a complete game engine. It's formost aim is to allow the creation of a high quality 3D Action Role Playing Game. It is however being designed such that it should be usuable in other 3D games as well - most of the logic is contained withing chaiscript scripts.

##### About SQEE Sokoban
SQEE Sokoban is, along with SQEE RPG, an example application for LibSQEE. It is also a fun, nice looking game that can provide hours of puzzle fun :)

### Building SQEE

To build SQEE, you will need GLM ([>= 0.9.6](https://github.com/g-truc/glm/releases)), SFML ([>= 2.3.1](http://www.sfml-dev.org/download.php)), ChaiScript ([>= 5.7.1](https://github.com/ChaiScript/ChaiScript/releases)) and ReactPhysics3D ([my fork](https://github.com/jagoly/reactphysics3d/)). On linux, you may be able to grab one or more of them from your distro's repos, if available and up to date. Otherwise, you can build and/or install them manually from the links above. 

Alternatively, you may instead grab the library bundle linked below, if it supports your target platform and compiler. It currently provides libraries for Linux with GCC 4.8.4 and Windows with MinGW 5.2.0. 

##### SQEE CMake options:
 * `SQEE_[OS]` - Specify the target to build for. Rather than trying to automatically detect it, you must specify one option manually: `LINUX`, `WINDOWS`, `ANDROID` or `MACOSX`. All default to `False`.
 * `SQEE_BUNDLED_LIBS` - Use the library bundle (extlibs) linked below. Otherwise, search in system paths. Defaults to `True`.
 * `SQEE_STATIC_LINK` - Link with static versions of dependencies, if available. Otherwise, use shared versions. Defaults to `False`.
 * `SQEE_STATIC_LIB` - Build LibSQEE as a static library. Otherwise, build as shared. Defaults to `False`.
 * `SQEE_DEBUG` - Enables a few extra debug features within SQEE. Does not affect `CMAKE_BUILD_TYPE`. Defaults to `True`.

##### SQEE Dependency Library Bundle
You can download a bundle of pre-built libraries for use with sqee from [here](http://jagoly.net/files/sqee_library_bundle.tar.bz2). It can be used to ease the process of building SQEE.

### Licence Information

##### SQEE Licence
The SQEE project is Copyright (c) 2015 James Gangure (jagoly). All code in the repository is released under the GNU GPL Version 3, which can be read [here](http://www.gnu.org/licenses/gpl.html). Any assets in this repository created by myself are released under the Creative Commons CC-BY-SA licence, which can be read [here](https://creativecommons.org/licenses/by-sa/2.0). 

##### Bundled Source Licence 
The bundled STB_Image source, by Sean T. Barrett, is released Public Domain. The bundled TinyFormat source, by Chris Foster, is released under Boost Software Licence 1.0. The bundled Lean Mean C++ Option Parser source, by Matthias S. Benkmann, is released under the MIT licence.
