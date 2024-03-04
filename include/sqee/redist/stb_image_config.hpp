#pragma once

#define STBI_NO_STDIO

#define STBI_NO_JPEG
//#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
//#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#define STBI_FAILURE_USERMSG

// sqee also sets stbi__vertically_flip_on_load_global to 1 in stb_image.cpp,
// don't forget to change that again when updating stb_image
