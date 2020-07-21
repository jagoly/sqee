# SQEE Readme

[![Travis Status](https://travis-ci.org/jagoly/sqee.svg?branch=master)](https://travis-ci.org/jagoly/sqee)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/7ha0l295301mfgvr?svg=true)](https://ci.appveyor.com/project/jagoly/sqee)

##### About LibSQEE
LibSQEE is library containing all sorts of useful stuff for 3D applications, particularly games. Features include C++ wrappers for various OpenGL components, a simple text rendering system, some nice classes for Windows and Scenes, a scripting console, loaders for custom model, material and animation formats, a generic resource manager, sound system, simple event handling system, physics bodies and more. LibSQEE intergrates a few third party libraries, namely:

 * [SFML](https://www.sfml-dev.org/)
 * [Lua](https://www.lua.org/)
 * [Sol3](https://github.com/ThePhD/sol2)
 * [STB_Image](https://github.com/nothings/stb)
 * [GLLoadGen](https://bitbucket.org/alfonse/glloadgen)
 * [Option Parser](http://optionparser.sourceforge.net/)
 * [TinyFormat](https://github.com/c42f/tinyformat)
 * [JSON](https://github.com/nlohmann/json)

##### About SQEE RPG
SQEE RPG is a not a library, but a complete game engine. It's formost aim is to allow the creation of a high quality 3D Action Role Playing Game. It is however being designed such that it should be usuable in other 3D games as well - most of the logic is contained within chaiscript scripts.

##### About SQEE Sokoban
SQEE Sokoban is, along with SQEE RPG, an example application for LibSQEE. It is also a fun, nice looking game that can provide hours of puzzle fun :)

### Building SQEE

To build SQEE, you will need SFML [>= 2.5.1](https://www.sfml-dev.org/download.php). Everything else is included in the Repo. On linux, you grab SFML from your distro's repos, if available and up to date. Otherwise, you can build and/or install it manually from the links above.

A C++ 17 capable compiler is required. SQEE is primarily tested with GCC and Clang, but should build with MSVC as well (see appveyor(https://ci.appveyor.com/project/jagoly/sqee)).



##### SQEE CMake options:
 * `SQEE_STATIC_LIB` - Build LibSQEE and Lua as a static library. Otherwise, build as shared. Defaults to `False`.

### Licence Information

##### SQEE Licences
The SQEE Project is Copyright (c) 2020 James Gangur (jagoly). All code created by me in the repository is released under the GNU GPL Version 3, which can be read [here](http://www.gnu.org/licenses/gpl.html). Any assets created by me in this repository are released under the Creative Commons CC-BY-SA licence, which can be read [here](https://creativecommons.org/licenses/by-sa/2.0).

##### Bundled Source Licences
The bundled STB-Image source, by Sean T. Barrett, is released Public Domain. The bundled TinyFormat source, by Chris Foster, is released under Boost Software Licence 1.0. The bundled Lean Mean C++ Option Parser source, by Matthias S. Benkmann, is released under the MIT Licence. The bundled Nlohmann-JSON source, by Niels Lohmann, is released under the MIT Licence. todo: update other licences
