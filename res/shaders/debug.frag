#version 460 core
layout (location = 2) out vec4 gAlbedospec;
out vec4 FragColor;

void main() {
  gAlbedospec.rgb = vec3(1.0,0.0,0.0);
  gAlbedospec.a = -1.0;
} 