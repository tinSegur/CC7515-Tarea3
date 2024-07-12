#version 410

layout (quads, fractional_even_spacing, ccw) in;

uniform float seed = 43758.5453123;
uniform vec2 seed2 = vec2(12.9898,78.233);
uniform vec3 seed3 = vec3(12.9898,78.233,56.3212);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int terrainGen = 0;

uniform float u_time;
uniform float world_scale = 0.1;


const float PI = 3.14159265;

float amp = 1.;
float freq = .1;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         seed2))*seed);
}

float random (vec3 st) {
    return fract(sin(dot(st.xyz,
                         seed3))*seed);
}

float noise(vec2 st){
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
    (c - a)* u.y * (1.0 - u.x) +
    (d - b) * u.x * u.y;
}

vec2 spaceMap(vec2 ps){
    return ps*world_scale;
}

// default fbm terrain generation
#define OCTAVES 12
float fbm (vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;

    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

// introduce gauss distortion over fbm to simulate a mountain
float fbmOverGauss (vec2 st) {
    float gs = exp(-length(st)/2);
    return gs + fbm(st);
}

// introduce second derivative of gauss distortion over fbm to simulate canyon
float fbmOverCanyon(vec2 st) {
    float sq = st.y*st.y;
    float cn = (sq - 1)*exp(-sq/2);
    return cn + fbm(st);
}

float terrain(vec2 ps){
    if (terrainGen == 0){
        return 20*fbm(spaceMap(ps));

    }
    else if (terrainGen == 1) {
        return 20*fbmOverGauss(spaceMap(ps));
    }
    else if (terrainGen == 2) {
        return 20*fbmOverCanyon(spaceMap(ps));
    }
}

vec3 v3dterrain(vec2 ps){
    return vec3(ps.x, terrain(ps), ps.y);
}

vec3 getNorm(vec2 pos){
    vec2 uop = pos + vec2(-1.0, -1.0);
    vec2 oop = pos + vec2(1.0, 1.0);

    float uoy = fbm(uop);
    float ooy = fbm(oop);

    return cross(vec3(uop.x, uoy, uop.y), vec3(oop.x, ooy, oop.y));
}

vec3 getBetterNorm(vec2 pos, float cy){
    vec3 cspace = vec3(pos.x, cy, pos.y);
    vec3 norm = vec3(0.0);

    //generate offset vectors
    vec2 vert = vec2(0.0, -1.0); // Offset up
    vec2 horz = vec2(1.0, 0.0); // Offset right

    vec2 offsetpos[6] = vec2[6](
        pos + vert,
        pos + vert + horz,
        pos + horz,
        pos - vert,
        pos - vert - horz,
        pos - horz
    );

    for (int i = 0; i<6; i++){
        vec3 p1 = v3dterrain(offsetpos[i]);
        vec3 p2 = v3dterrain(offsetpos[(i+1)%6]);

        norm += cross(p1 - cspace, p2 - cspace);
    }

    return norm/6;
}

out vec3 Normal;
out vec3 fragPos;
out vec3 worldPos;

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

    p.y += floor(terrain(p.xz)*100)/100; // redondear hacia abajo para reducir grietas debido a la teselación

    Normal = getBetterNorm(p.xz, p.y);
    fragPos = vec3(model*p);
    worldPos = p.xyz;

    gl_Position = projection*view*model*p;
}