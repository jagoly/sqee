extern const char ui_VS_box_simple[] = R"glsl(

#version 420 core

uniform vec4 posSize;

const vec2 V_data[4] = {
    vec2(0.f, 0.f), vec2(0.f, 1.f),
    vec2(1.f, 1.f), vec2(1.f, 0.f)
};

out vec2 localPos;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    localPos = V_data[gl_VertexID];
    gl_Position.xy = (posSize.xy + localPos * posSize.zw) * 2.f - 1.f;
    gl_Position.z = 0.f; gl_Position.w = 1.f;
}

)glsl";


////////////////////////////////////////

extern const char ui_FS_colour_simple[] = R"glsl(

#version 420 core

uniform vec3 colour;

out vec4 fragColour;

void main() {
    fragColour = vec4(colour, 1.f);
}

)glsl";
