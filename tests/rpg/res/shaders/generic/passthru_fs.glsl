#version 330
#extension GL_ARB_shading_language_420pack : enable

// define LAYER

in vec2 texcrd;

#ifdef LAYER
uniform int layer;
layout(binding=0) uniform sampler2DArray tex;
#else
layout(binding=0) uniform sampler2D tex;
#endif

out vec4 fragColour;


void main() {
  #ifdef LAYER
    fragColour = texture(tex, vec3(texcrd, layer));
  #else
    fragColour = texture(tex, texcrd);
  #endif
}
