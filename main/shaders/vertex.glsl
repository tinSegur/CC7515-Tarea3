#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_time;

const float PI = 3.14159265;

float amp = 1.;
float freq = .1;

float random (in vec2 st) {
  return fract(sin(dot(st.xy,
                       vec2(12.9898,78.233)))*43758.5453123);
}


mat4 rotateZ(float angle) {
  mat4 rotationMatrix;
  rotationMatrix[0] = vec4(cos(angle), sin(angle), 0, 0);
  rotationMatrix[1] = vec4(-sin(angle), cos(angle), 0, 0);
  rotationMatrix[2] = vec4(0, 0, 1, 0);
  rotationMatrix[3] = vec4(0, 0, 0, 1);
  return rotationMatrix;
}

float noise(float x, float z){
    return sin(x*freq*2.1 + z)*4.5
    + sin(x*freq*1.72 + z*1.121)*4.0
    + sin(x*freq*2.221 + z*0.437)*5.0
    + sin(x*freq*3.1122+ z*4.269)*2.5;
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
    float amplitude = 1.5;
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

out vec3 Normal;
out vec3 fragPos;

void main() {
    vec3 st = aPos;
    float t = u_time;
    float x  = t*st.x/10;
    float uox = t*(st.x + 1.)/10;
    float oox = t*(st.x - 1.)/10;


    float z  = t*st.z/10;
    float uoz = t*(st.z + 1.)/10;
    float ooz = t*(st.z - 1.)/10;

    vec2 sst = vec2(x, z);
    vec2 uost = vec2(uox, uoz);
    vec2 oost = vec2(oox, ooz);

    st.y = fbm(sst);

    vec3 aN = cross(vec3(uox, fbm(uost), uoz), vec3(oox, fbm(oost), ooz));

    gl_Position = projection * view * model * vec4(st, 1.0);
    Normal = aN;
    fragPos = vec3(model*vec4(st, 1.0));
}
