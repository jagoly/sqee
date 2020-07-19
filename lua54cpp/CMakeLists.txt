cmake_minimum_required(VERSION 3.16)
project(Lua54CPP CXX)

# platform notes:
# linux assumes clang/gcc
# windows assumes msvc
# BSD/OSX aren't supported yet

option(LUA54CPP_STATIC_LIB "Build Lua54CPP as a static library" False)

set(LUA_SOURCES

    "src/lua.h" # do not use lua.hpp

    "src/ljumptab.h"
    "src/llimits.h"
    "src/lopnames.h"
    "src/lprefix.h"
    "src/luaconf.h"
    "src/lualib.h"

    "src/lbaselib.cpp"
    "src/lcorolib.cpp"
    "src/ldblib.cpp"
    "src/ldump.cpp"
    "src/linit.cpp"
    "src/liolib.cpp"
    "src/lmathlib.cpp"
    "src/loadlib.cpp"
    "src/loslib.cpp"
    "src/lstrlib.cpp"
    "src/ltablib.cpp"
    "src/lutf8lib.cpp"

    "src/lapi.h"      "src/lapi.cpp"
    "src/lauxlib.h"   "src/lauxlib.cpp"
    "src/lcode.h"     "src/lcode.cpp"
    "src/lctype.h"    "src/lctype.cpp"
    "src/ldebug.h"    "src/ldebug.cpp"
    "src/ldo.h"       "src/ldo.cpp"
    "src/lfunc.h"     "src/lfunc.cpp"
    "src/lgc.h"       "src/lgc.cpp"
    "src/llex.h"      "src/llex.cpp"
    "src/lmem.h"      "src/lmem.cpp"
    "src/lobject.h"   "src/lobject.cpp"
    "src/lopcodes.h"  "src/lopcodes.cpp"
    "src/lparser.h"   "src/lparser.cpp"
    "src/lstate.h"    "src/lstate.cpp"
    "src/lstring.h"   "src/lstring.cpp"
    "src/ltable.h"    "src/ltable.cpp"
    "src/ltm.h"       "src/ltm.cpp"
    "src/lundump.h"   "src/lundump.cpp"
    "src/lvm.h"       "src/lvm.cpp"
    "src/lzio.h"      "src/lzio.cpp"

    #"src/lua.cpp"
    #"src/luac.cpp"
)

if (LUA54CPP_STATIC_LIB)
    add_library(lua54cpp STATIC ${LUA_SOURCES})
else ()
    add_library(lua54cpp SHARED ${LUA_SOURCES})
endif ()

target_include_directories(lua54cpp PUBLIC "${PROJECT_SOURCE_DIR}/src")

set_property(TARGET lua54cpp PROPERTY CXX_STANDARD 17)
set_property(TARGET lua54cpp PROPERTY CXX_STANDARD_REQUIRED True)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(lua54cpp PUBLIC -Wall -Wextra)
    target_compile_definitions(lua54cpp PUBLIC LUA_USE_POSIX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # todo
endif ()
