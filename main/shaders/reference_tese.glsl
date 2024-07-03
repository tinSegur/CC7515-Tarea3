#version 400
layout (quads, fractional_odd_spacing, ccw) in;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 gVP= projection*view*model;
    // get the results from the tessellator
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // get the position for each vertex
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // same interpolation as the previous one
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // transform from world to clip space
    gl_Position = gVP * p;
}