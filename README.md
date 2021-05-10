# SQEE Readme

[![Travis Status](https://travis-ci.org/jagoly/sqee.svg?branch=master)](https://travis-ci.org/jagoly/sqee)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/7ha0l295301mfgvr?branch=master&svg=true)](https://ci.appveyor.com/project/jagoly/sqee)

##### About SQEE

SQEE is library containing all sorts of useful stuff for 3D applications, particularly games. Some features include:
 * A Vulkan Window class that manages the swapchain and synchronisation
 * An easy to use Vulkan memory allocator
 * Vector, Matrix, Quaternion, and more maths utilities
 * Vulkan Mesh, Texture, Pipeline, Material, and Armature classes
 * Resource Cache and Handle templates for managing assets
 * Small utilities such as stack only string and vector types
 * ImGui integration and some custom wrappers and widgets
 * Much much more

SQEE intergrates a few third party libraries. In no particular order, they are:
 * [GLFW](https://www.glfw.org/)
 * [Wren](https://wren.io/)
 * [MiniAudio](https://github.com/mackron/miniaudio/)
 * [DearImGui](https://github.com/ocornut/imgui/)
 * [STB_Image](https://github.com/nothings/stb/)
 * [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross/)
 * [OptionParser](http://optionparser.sourceforge.net/)
 * [FmtLib](https://github.com/fmtlib/fmt/)
 * [JSON](https://github.com/nlohmann/json/)


### Building SQEE

To build SQEE, you will need [GLFW >= 3.3](https://github.com/glfw/glfw/releases). Everything else is included in the repo. On linux, you can grab GFLW from your distro's repos, if available and up to date. Otherwise, you can build and/or install it manually from the links above.

A C++ 17 capable compiler is required. SQEE is primarily tested with GCC and Clang, but should build with MSVC as well (see [appveyor](https://ci.appveyor.com/project/jagoly/sqee)).

##### SQEE CMake options:
 * `SQEE_STATIC_LIB` - Build SQEE and its dependencies as a static library. Otherwise, build as shared. Defaults to `False`.


### Licence Information

##### SQEE Licences
The SQEE Project is Copyright (c) 2021 James Gangur (jagoly). All code created by me in the repository is released under the GNU GPL Version 3, which can be read [here](http://www.gnu.org/licenses/gpl.html). Any assets created by me in this repository are released under the Creative Commons CC-BY-SA licence, which can be read [here](https://creativecommons.org/licenses/by-sa/2.0).

##### Bundled Source Licences
For licence information for the bundled libraries, see the links above.
