#version 400

layout (quads, fractional_even_spacing, ccw) in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // get the position for each vertex
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p11 = gl_in[2].gl_Position;
    vec4 p10 = gl_in[3].gl_Position;

    // interpolar puntos entre teselación
    vec4 p0 = mix(p01, p00, u);
    vec4 p1 = mix(p11, p10, u);
    vec4 p = mix(p1, p0, v);

    p.xyz = 2*normalize(p.xyz); // normalizar para ubicar sobre esfera unitaria

    gl_Position = projection*view*model*p;

}