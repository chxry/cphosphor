#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
out vec3 vert;
out vec2 texcoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vert = aPos;
  texcoord = aTexCoord;
  normal = aNormal;
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}