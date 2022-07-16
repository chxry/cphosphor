#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedospec;

in vec3 fragpos;
in vec2 texcoord;
in vec3 normal;

uniform sampler2D tex;
uniform float spec;

void main() {
  gPosition = fragpos;
  gNormal = normalize(normal);
  gAlbedospec.rgb = texture(tex, texcoord).rgb;
  gAlbedospec.a = spec;
}