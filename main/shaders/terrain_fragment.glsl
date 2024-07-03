#version 420 core
in vec3 Normal;
in vec3 fragPos;
in vec3 worldPos;

out vec4 fragColor;

uniform vec2 u_resolution;
uniform vec3 vtx;
uniform float clineh;
uniform vec3 viewPos;
uniform float fog_f;

uniform vec2 seed2 = vec2(12.9898,78.233);
uniform vec3 seed3 = vec3(12.9898,78.233,56.3212);
uniform int terrainGen = 0;

float ambient_strength = 0.5;
float spec_strength = .5;
vec3 lightColor = vec3(0.8, 0.8, 0.6);
vec3 terrainColor = vec3(0.6, 0.4, 0.2);

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
  float gs = exp(-length(st - spaceMap(vec2(40.0)))/2);
  return gs + fbm(st);
}

float terrain(vec2 ps){
  if (terrainGen == 0){
    return 20*fbm(spaceMap(ps));

  }
  else if (terrainGen == 1) {
    return 20*fbmOverGauss(spaceMap(ps));
  }
}

vec3 v3dterrain(vec2 ps){
  return vec3(ps.x, terrain(ps), ps.y);
}

vec3 green = vec3(0.059, 0.651, 0.082);
vec3 brown = vec3(0.569, 0.467, 0.059);


vec3 colorizeterrain(vec3 ps){
  float f = ps.y/30.0;
  vec3 interpcolor = mix(mix(green, brown, (1-f)), mix(brown, vec3(1.0), f), f);
  interpcolor = interpcolor*fbm(ps.xz);
  return interpcolor;
}

// volumetric fog
vec3 fog(vec3 color, float distance, vec3 cameraDir, vec3 lightDir){
  float fogAmount = 1.0 - exp(-distance*0.05*fog_f);
  float lightAmount = max(dot(cameraDir, lightDir), 0.0);
  vec3 fogColor = mix(
          vec3(0.569, 0.761, 0.706),
          vec3(0.737, 1, 0.925),
          pow(lightAmount, 8.0));
  return mix(color, fogColor, fogAmount);
}

void main() {

  vec3 norm = normalize(Normal);
  //lighting calculations
  vec3 lightDir = normalize(vtx - fragPos);

  //ambient lighting
  vec3 ambient = ambient_strength*lightColor;

  //diffuse lighting
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff*lightColor;

  //specular lighting
  vec3 vdir = normalize(viewPos - fragPos);
  vec3 ref = reflect(-lightDir, norm);
  float spec = pow(max(dot(vdir, ref), 0.0), 16);
  vec3 specular = spec_strength*spec*lightColor;



  if (abs(fragPos.y - clineh) < 0.02 ){
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else {
    // Calculate light with raymarching
    vec3 res = (ambient + diffuse + specular)*colorizeterrain(fragPos);
    res = fog(res, distance(viewPos, fragPos), normalize(viewPos - fragPos), lightDir);
    fragColor = vec4(res, 1.0);
  }

}
