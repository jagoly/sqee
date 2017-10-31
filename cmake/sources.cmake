cmake_minimum_required(VERSION 3.4)

set(SOURCES

  #==========================================================#
  # 3rd Party Source                                         #
  #==========================================================#

  "include/sqee/redist/gl_loader.hpp"
  "source/sqee/redist/gl_loader.cpp"

  "include/sqee/redist/stb_image.hpp"
  "source/sqee/redist/stb_image.cpp"

  "include/sqee/redist/lmccop.hpp"
  "include/sqee/redist/nl_json.hpp"
  "include/sqee/redist/tinyformat.hpp"

  "include/sqee/redist/imgui/imconfig.hpp"
  "include/sqee/redist/imgui/imgui.hpp"
  "include/sqee/redist/imgui/imgui_internal.hpp"

  "include/sqee/redist/imgui/stb_rect_pack.hpp"
  "include/sqee/redist/imgui/stb_textedit.hpp"
  "include/sqee/redist/imgui/stb_truetype.hpp"

  "source/sqee/redist/imgui/imgui.cpp"
  "source/sqee/redist/imgui/imgui_demo.cpp"
  "source/sqee/redist/imgui/imgui_draw.cpp"

  "include/sqee/redist/variant.hpp"

  #==========================================================#
  # SQEE Source                                              #
  #==========================================================#

  "include/sqee/assert.hpp"
  "include/sqee/builtins.hpp"
  "include/sqee/export.hpp"
  "include/sqee/helpers.hpp"
  "include/sqee/macros.hpp"
  "include/sqee/setup.hpp"

  #----------------------------------------------------------#

  "include/sqee/app/Application.hpp"   "source/sqee/app/Application.cpp"
  "include/sqee/app/ChaiConsole.hpp"   "source/sqee/app/ChaiConsole.cpp"
  "include/sqee/app/ChaiEngine.hpp"    "source/sqee/app/ChaiEngine.cpp"
  "include/sqee/app/DebugOverlay.hpp"  "source/sqee/app/DebugOverlay.cpp"
  "include/sqee/app/GuiSystem.hpp"     "source/sqee/app/GuiSystem.cpp"
  "include/sqee/app/GuiWidgets.hpp"    "source/sqee/app/GuiWidgets.cpp"
  "include/sqee/app/InputDevices.hpp"  "source/sqee/app/InputDevices.cpp"
  "include/sqee/app/MessageBus.hpp"    "source/sqee/app/MessageBus.cpp"
  "include/sqee/app/PreProcessor.hpp"  "source/sqee/app/PreProcessor.cpp"
  "include/sqee/app/Scene.hpp"         "source/sqee/app/Scene.cpp"
  "include/sqee/app/Window.hpp"        "source/sqee/app/Window.cpp"

  "include/sqee/app/Event.hpp"

  #----------------------------------------------------------#

  "include/sqee/debug/Logging.hpp"  "source/sqee/debug/Logging.cpp"
  "include/sqee/debug/Misc.hpp"     "source/sqee/debug/Misc.cpp"
  "include/sqee/debug/OpenGL.hpp"   "source/sqee/debug/OpenGL.cpp"
  "include/sqee/debug/Text.hpp"     "source/sqee/debug/Text.cpp"

  #----------------------------------------------------------#

  "include/sqee/dop/Classes.hpp"    "source/sqee/dop/Classes.cpp"
  "include/sqee/dop/Functions.hpp"  "source/sqee/dop/Functions.cpp"

  #----------------------------------------------------------#

  "include/sqee/gl/Context.hpp"        "source/sqee/gl/Context.cpp"
  "include/sqee/gl/Drawing.hpp"        "source/sqee/gl/Drawing.cpp"
  "include/sqee/gl/FixedBuffer.hpp"    "source/sqee/gl/FixedBuffer.cpp"
  "include/sqee/gl/FrameBuffer.hpp"    "source/sqee/gl/FrameBuffer.cpp"
  "include/sqee/gl/Program.hpp"        "source/sqee/gl/Program.cpp"
  "include/sqee/gl/TextureBase.hpp"    "source/sqee/gl/TextureBase.cpp"
  "include/sqee/gl/Textures.hpp"       "source/sqee/gl/Textures.cpp"
  "include/sqee/gl/UniformBuffer.hpp"  "source/sqee/gl/UniformBuffer.cpp"
  "include/sqee/gl/VertexArray.hpp"    "source/sqee/gl/VertexArray.cpp"

  #----------------------------------------------------------#

  "include/sqee/maths/Culling.hpp"
  "include/sqee/maths/Functions.hpp"
  "include/sqee/maths/Matrices.hpp"
  "include/sqee/maths/Quaternion.hpp"
  "include/sqee/maths/Scalar.hpp"
  "include/sqee/maths/Vectors.hpp"
  "include/sqee/maths/Volumes.hpp"

  #----------------------------------------------------------#

  "include/sqee/misc/Files.hpp"       "source/sqee/misc/Files.cpp"
  "include/sqee/misc/Json.hpp"        "source/sqee/misc/Json.cpp"
  "include/sqee/misc/StringCast.hpp"  "source/sqee/misc/StringCast.cpp"
  "include/sqee/misc/Parsing.hpp"     "source/sqee/misc/Parsing.cpp"

  "include/sqee/misc/Algorithms.hpp"

  "include/sqee/misc/PoolTools.hpp"

  "include/sqee/misc/ResourceCache.hpp"
  "include/sqee/misc/ResourceHandle.hpp"

  "include/sqee/misc/FlagSet.hpp"
  "include/sqee/misc/UniqueAny.hpp"

  #----------------------------------------------------------#

  "include/sqee/physics/Bodies.hpp"  "source/sqee/physics/Bodies.cpp"

  #----------------------------------------------------------#

  "include/sqee/render/Armature.hpp"  "source/sqee/render/Armature.cpp"
  "include/sqee/render/Material.hpp"  "source/sqee/render/Material.cpp"
  "include/sqee/render/Mesh.hpp"      "source/sqee/render/Mesh.cpp"
  "include/sqee/render/Volume.hpp"    "source/sqee/render/Volume.cpp"

  #----------------------------------------------------------#

  "include/sqee/scripts/BasicSetup.hpp"  "source/sqee/scripts/BasicSetup.cpp"

  "include/sqee/scripts/Helpers.hpp"

  #----------------------------------------------------------#

  "include/sqee/sound/Sound.hpp"  "source/sqee/sound/Sound.cpp"


  #==========================================================#
  # Compiled-In Data                                         #
  #==========================================================#

  "source/sqee/data/ShaderHeaders.cpp"

  "source/sqee/data/UbuntuMinimal.cpp"

  "source/sqee/data/TextFont.c"
  "source/sqee/data/TextGlow.c"

  "source/sqee/data/Volumes.c"
)
