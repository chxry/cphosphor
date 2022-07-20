#version 460 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedospec;

in vec2 texcoord;

uniform sampler2D tex;

void main() {
  vec4 col = texture(tex, texcoord);
  if (col.a < 0.1) {
    discard;
  }
  gAlbedospec.rgb = col.rgb;
  gAlbedospec.a = -1.0;
}
