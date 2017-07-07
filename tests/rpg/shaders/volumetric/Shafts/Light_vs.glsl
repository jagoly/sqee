// GLSL Vertex Shader

//============================================================================//

#include builtin/misc/screen

#include headers/blocks/Camera

//============================================================================//

out vec2 texcrd;
out vec3 viewRay;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

//============================================================================//

void main()
{
    texcrd = v_Position * 0.5f + 0.5f;
    vec4 viewRayW = CB.invProjMat * vec4(v_Position, 0.f, 1.f);
    viewRay = viewRayW.xyz / viewRayW.w;

    gl_Position = vec4(v_Position, 0.f, 1.f);
}
