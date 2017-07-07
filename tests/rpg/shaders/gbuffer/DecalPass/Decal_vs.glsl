// GLSL Vertex Shader

//============================================================================//

#include headers/blocks/Camera

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

//============================================================================//

layout(location=0) in vec3 v_Position;

layout(location=0) uniform mat4 u_ModelView;

out noperspective vec2 screenPos;
out vec3 viewNorm, viewTan;

//============================================================================//

void main()
{
    gl_Position = CB.projMat * u_ModelView * vec4(v_Position, 1.f);
    screenPos = gl_Position.xy / gl_Position.w * 0.5f + 0.5f;

    viewNorm = mat3(u_ModelView) * vec3(0.f, 0.f, +1.f);
    viewTan = mat3(u_ModelView) * vec3(0.f, -1.f, 0.f);
}
