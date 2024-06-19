#version 420 core
in vec3 Normal;
in vec3 fragPos;

out vec4 fragColor;

uniform vec2 u_resolution;
uniform vec3 vtx;

float ambient_strength = 0.3;
vec3 lightColor = vec3(0.2, 0.2, 0.6);

void main() {

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(vtx - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff*lightColor;


  vec3 res = (ambient_strength*lightColor + diffuse)*vtx;
  fragColor = vec4(res, 1.0);
}
