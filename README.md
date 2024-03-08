# <img src="logo.svg" align="left" width=72px alt="SQEE Logo"> SQEE Readme <div> [![Linux Build](https://github.com/jagoly/sqee/actions/workflows/linux.yml/badge.svg)](https://github.com/jagoly/sqee/actions/workflows/linux.yml) [![Windows Build](https://github.com/jagoly/sqee/actions/workflows/windows.yml/badge.svg)](https://github.com/jagoly/sqee/actions/workflows/windows.yml) </div>

### About SQEE

SQEE is library containing all sorts of useful stuff for 3D applications, particularly games. Some features include:
 * A Vulkan Window class that manages the swapchain and synchronisation
 * An easy to use Vulkan memory allocator
 * Vector, Matrix, Quaternion, and more maths utilities
 * Vulkan Mesh, Texture, Pipeline, DrawItem, Armature, and more
 * Resource Cache and Handle templates for managing assets
 * JSON format describing how to render scenes or objects
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
 * [FmtLib](https://github.com/fmtlib/fmt/)
 * [YYJSON](https://github.com/ibireme/yyjson)


### Building SQEE

To build SQEE, you will need [GLFW >= 3.4](https://github.com/glfw/glfw/releases). Everything else is included in the repo. On linux, you can grab GFLW from your distro's repos, if available and up to date. Otherwise, you can build it manually from the link above.

A C++20 capable compiler is required. Building SQEE is fairly simple, have a look at [linux.yml](https://github.com/jagoly/sqee/blob/master/.github/workflows/linux.yml) and [windows.yml](https://github.com/jagoly/sqee/blob/master/.github/workflows/windows.yml) for how.

##### SQEE CMake options:
 * `SQEE_STATIC_LIB` - Build SQEE and its dependencies as a static library. Otherwise, build as shared. Defaults to `False`.


### Licence Information

##### SQEE Licences
The SQEE Project is Copyright (c) 2024 James Gangur (jagoly). All code created by me in the repository is released under the GNU GPL Version 3, which can be read [here](http://www.gnu.org/licenses/gpl.html). Any assets created by me in this repository are released under the Creative Commons CC-BY-SA licence, which can be read [here](https://creativecommons.org/licenses/by-sa/2.0).

##### Bundled Source Licences
For licence information for the bundled libraries, see the links above.
