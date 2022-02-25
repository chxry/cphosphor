#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 fragpos;
out vec2 texcoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vec4 worldpos = model * vec4(aPos, 1.0);
  fragpos = worldpos.xyz; 
  texcoord = aTexCoord;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  normal = normalMatrix * aNormal;

  gl_Position = projection * view * worldpos;
}