#version 460 core
out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D tex;

void main() {
  vec4 col = texture(tex, texcoord);
  if (col.a < 0.1) {
    discard;
  }
  FragColor.rgb = col.rgb;
  FragColor.a = 0.5;
}
