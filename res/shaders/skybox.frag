#version 460 core
layout (location = 2) out vec4 gAlbedospec;
in vec3 texcoord;

uniform samplerCube skybox;

void main() {
  gAlbedospec.rgb = texture(skybox, texcoord).rgb;
  gAlbedospec.a = -1;
}