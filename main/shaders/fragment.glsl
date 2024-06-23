#version 420 core
in vec3 Normal;
in vec3 fragPos;
in vec3 worldPos;

out vec4 fragColor;

uniform vec2 u_resolution;
uniform vec3 vtx;
uniform float clineh;
uniform vec3 viewPos;

float ambient_strength = 0.2;
float spec_strength = .5;
vec3 lightColor = vec3(0.8, 0.8, 0.6);
vec3 terrainColor = vec3(0.6, 0.4, 0.2);

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
  float spec = pow(max(dot(vdir, ref), 0.0), 32);
  vec3 specular = spec_strength*spec*lightColor;


  if (abs(worldPos.y - clineh) < 0.05 ){
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else {
    vec3 res = (ambient + diffuse + specular)*terrainColor;
    fragColor = vec4(res, 1.0);
  }

}
