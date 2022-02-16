#version 460 core
in vec3 fragpos;
in vec2 texcoord;
in vec3 normal;
out vec4 FragColor;

uniform sampler2D tex;
uniform struct {
  vec3 pos;
  vec3 color;
  float ambient;
} light;

void main() {
  vec3 lightDir = normalize(light.pos - fragpos); 
  vec3 color = texture(tex,texcoord).rgb;

  vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.color;
  vec3 ambient = light.ambient * light.color;

  FragColor = vec4((diffuse+ambient)*color, 1.0);
}