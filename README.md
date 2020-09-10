# SQEE Readme

[![Travis Status](https://travis-ci.org/jagoly/sqee.svg?branch=master)](https://travis-ci.org/jagoly/sqee)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/7ha0l295301mfgvr?branch=master&svg=true)](https://ci.appveyor.com/project/jagoly/sqee)

##### About LibSQEE
LibSQEE is library containing all sorts of useful stuff for 3D applications, particularly games. Features include C++ wrappers for various OpenGL components, a simple text rendering system, some nice classes for Windows and Scenes, a scripting console, loaders for custom model, material and animation formats, a generic resource manager, sound system, simple event handling system, physics bodies and more. A lot more, I don't update this readme very often.

LibSQEE intergrates a few third party libraries. In no particular order, they are:

 * [SFML](https://www.sfml-dev.org/)
 * [Wren](https://wren.io/)
 * [MiniAudio](https://github.com/mackron/miniaudio/)
 * [DearImGui](https://github.com/ocornut/imgui/)
 * [STB_Image](https://github.com/nothings/stb/)
 * [GLLoadGen](https://bitbucket.org/alfonse/glloadgen/)
 * [OptionParser](http://optionparser.sourceforge.net/)
 * [FmtLib](https://github.com/fmtlib/fmt/)
 * [JSON](https://github.com/nlohmann/json/)

##### About SQEE RPG
SQEE RPG is a not a library, but a complete game engine. It's formost aim is to allow the creation of a high quality 3D Action Role Playing Game. It is however being designed such that it should be usuable in other 3D games as well - most of the logic is contained within chaiscript scripts.

##### About SQEE Sokoban
SQEE Sokoban is, along with SQEE RPG, an example application for LibSQEE. It is also a fun, nice looking game that can provide hours of puzzle fun :)

### Building SQEE

To build SQEE, you will need [SFML >= 2.5.1](https://www.sfml-dev.org/download.php). Everything else is included in the repo. On linux, you can grab SFML from your distro's repos, if available and up to date. Otherwise, you can build and/or install it manually from the links above.

A C++ 17 capable compiler is required. SQEE is primarily tested with GCC and Clang, but should build with MSVC as well (see [appveyor](https://ci.appveyor.com/project/jagoly/sqee)).



##### SQEE CMake options:
 * `SQEE_STATIC_LIB` - Build LibSQEE and its dependencies as a static library. Otherwise, build as shared. Defaults to `False`.

### Licence Information

##### SQEE Licences
The SQEE Project is Copyright (c) 2020 James Gangur (jagoly). All code created by me in the repository is released under the GNU GPL Version 3, which can be read [here](http://www.gnu.org/licenses/gpl.html). Any assets created by me in this repository are released under the Creative Commons CC-BY-SA licence, which can be read [here](https://creativecommons.org/licenses/by-sa/2.0).

##### Bundled Source Licences
For licence information for the bundled libraries, see the links above.
