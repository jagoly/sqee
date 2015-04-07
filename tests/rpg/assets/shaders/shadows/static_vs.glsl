#version 330
#extension GL_ARB_shading_language_420pack : enable

// define PUNCH

layout(location = 0) in vec3 V_pos;

uniform mat4 modelMat;
uniform mat4 shadMat;

#ifdef PUNCH
layout(location = 3) in vec2 V_texcrd;
out vec2 texcrd;
#endif

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = shadMat * modelMat * vec4(V_pos, 1.f);

  #ifdef PUNCH
    texcrd = V_texcrd;
  #endif
}
