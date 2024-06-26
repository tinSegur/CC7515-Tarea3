#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_time;

const float PI = 3.14159265;

float amp = 1.;
float freq = .1;

float random (vec2 st) {
  return fract(sin(dot(st.xy,
                       vec2(12.9898,78.233)))*43758.5453123);
}

float random (vec3 st) {
    return fract(sin(dot(st.xyz,
                         vec3(12.9898,78.233,56.3212)))*43758.5453123);
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

#define OCTAVES 6
float fbm (vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

vec3 getNorm(vec2 pos){
    vec2 uop = pos + vec2(-1.0, -1.0);
    vec2 oop = pos + vec2(1.0, 1.0);

    float uoy = fbm(uop);
    float ooy = fbm(oop);

    return cross(vec3(uop.x, uoy, uop.y), vec3(oop.x, ooy, oop.y));
}


out vec3 Normal;
out vec3 fragPos;

void main() {
    vec3 st = aPos;
    float t = u_time;

    st.y = 10*fbm(st.xz);
    vec3 aN = normalize(getNorm(st.xz));

    gl_Position = projection * view * model * vec4(st, 1.0);
    Normal = aN;
    fragPos = vec3(model*vec4(st, 1.0));
}
