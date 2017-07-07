# SQEE Readme

##### About LibSQEE
LibSQEE is library containing all sorts of useful stuff for 3D applications, particularly games. Features include C++ wrappers for various OpenGL components, a simple text rendering system, some nice classes for Windows and Scenes, a scripting console, loaders for custom model, material and animation formats, a generic resource manager, sound system, simple event handling system, physics bodies and more. LibSQEE intergrates a few third party libraries, namely:

 * [SFML](http://www.sfml-dev.org/)
 * [ChaiScript](http://chaiscript.com)
 * [ReactPhysics3D](http://reactphysics3d.com)
 * [STB_Image](https://github.com/nothings/stb)
 * [GLLoadGen](https://bitbucket.org/alfonse/glloadgen)
 * [Option Parser](http://optionparser.sourceforge.net/)
 * [TinyFormat](https://github.com/c42f/tinyformat)
 * [JSON](https://github.com/nlohmann/json)

##### About SQEE RPG
SQEE RPG is a not a library, but a complete game engine. It's formost aim is to allow the creation of a high quality 3D Action Role Playing Game. It is however being designed such that it should be usuable in other 3D games as well - most of the logic is contained withing chaiscript scripts.

##### About SQEE Sokoban
SQEE Sokoban is, along with SQEE RPG, an example application for LibSQEE. It is also a fun, nice looking game that can provide hours of puzzle fun :)

### Building SQEE

To build SQEE, you will need SFML ([>= 2.4.2](http://www.sfml-dev.org/download.php)), ChaiScript ([>= 6.0.0](https://github.com/ChaiScript/ChaiScript/releases)) and ReactPhysics3D ([my fork](https://github.com/jagoly/reactphysics3d/)). On linux, you may be able to grab one or more of them from your distro's repos, if available and up to date. Otherwise, you can build and/or install them manually from the links above. 

##### SQEE CMake options:
 * `SQEE_[OS]` - Specify the target to build for. Rather than trying to automatically detect it, you must specify one option manually: `LINUX`, `WINDOWS`, `ANDROID` or `MACOSX`. All default to `False`.
 * `SQEE_STATIC_LINK` - Link with static versions of dependencies, if available. Otherwise, use shared versions. Defaults to `False`.
 * `SQEE_STATIC_LIB` - Build LibSQEE as a static library. Otherwise, build as shared. Defaults to `False`.

### Licence Information

##### SQEE Licences
The SQEE Project is Copyright (c) 2017 James Gangur (jagoly). All code created by me in the repository is released under the GNU GPL Version 3, which can be read [here](http://www.gnu.org/licenses/gpl.html). Any assets created by me in this repository are released under the Creative Commons CC-BY-SA licence, which can be read [here](https://creativecommons.org/licenses/by-sa/2.0).

##### Bundled Source Licences
The bundled STB-Image source, by Sean T. Barrett, is released Public Domain. The bundled TinyFormat source, by Chris Foster, is released under Boost Software Licence 1.0. The bundled Lean Mean C++ Option Parser source, by Matthias S. Benkmann, is released under the MIT Licence. The bundled Nlohmann-JSON source, by Niels Lohmann, is released under the MIT Licence.
