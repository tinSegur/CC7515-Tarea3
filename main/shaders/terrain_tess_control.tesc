#version 400
layout (vertices = 4) out;

uniform mat4 model;
uniform mat4 view;

// parámetros de teselación
const float MIN_D = 100;
const float MAX_D = 1000;

const int min_t = 2;
const int max_t = 24;

float get_tess_fac(vec4 spacepos){
    // calcular posición en viewspace
    mat4 mvm = view*model;
    vec4 v = mvm*spacepos;

    // distancia en viewspace
    float f = length(v.xyz);

    // mapear a [0,1]
    f = clamp((f - MIN_D)/(MAX_D - MIN_D), 0.0, 1.0);

    // interpolar nivel de teselación
    f = mix(max_t, min_t, f);
    return f;
}

void main() {

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = get_tess_fac(gl_in[0].gl_Position);
    gl_TessLevelOuter[1] = get_tess_fac(gl_in[1].gl_Position);
    gl_TessLevelOuter[2] = get_tess_fac(gl_in[2].gl_Position);
    gl_TessLevelOuter[3] = get_tess_fac(gl_in[3].gl_Position);

    gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
    gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);

}