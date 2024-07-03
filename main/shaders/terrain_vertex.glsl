#version 400 core
#define MAX_TERRAIN_ARGS

layout (location = 0) in vec3 aPos;

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
                         seed2))*43758.5453123);
}

float random (vec3 st) {
    return fract(sin(dot(st.xyz,
                         seed3))*43758.5453123);
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

float terrain(vec2 ps){
    if (terrainGen == 0){
        return 10*fbm(spaceMap(ps));

    }
    else if (terrainGen == 1) {
        return 10*fbmOverGauss(spaceMap(ps));
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
    vec3 st = aPos;
    float t = u_time;

    st.y = terrain(st.xz);
    vec3 aN = -normalize(getBetterNorm(st.xz, st.y));


    gl_Position = projection * view * model * vec4(st, 1.0);
    Normal = aN;
    fragPos = vec3(model*vec4(st, 1.0));
    worldPos = st;
}
