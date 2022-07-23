#version 460 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedospec;

in vec3 fragpos;
in vec2 texcoord;
in vec3 normal;

uniform samplerCube skybox;
uniform sampler2D tex;
uniform vec3 cam_pos;
uniform float spec;
uniform float reflectivity;

void main() {
  gPosition = fragpos;
  gNormal = normalize(normal);
  gAlbedospec.a = spec;

  vec3 albedo = texture(tex, texcoord).rgb;
  vec3 reflection = texture(skybox, reflect(normalize(fragpos - cam_pos), gNormal)).rgb;
  gAlbedospec.rgb = mix(albedo, reflection, reflectivity);
}