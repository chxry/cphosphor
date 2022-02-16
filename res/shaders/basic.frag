#version 460 core
in vec3 vert;
in vec2 texcoord;
in vec3 normal;
out vec4 FragColor;

uniform sampler2D tex;
uniform mat4 model;
uniform struct {
  vec3 position;
  vec3 color;
  float attenuation;
  float ambientCoefficient;
} light;

void main() {
  vec3 wnormal = normalize(transpose(inverse(mat3(model))) * normal);
  vec3 surfacePos = vec3(model * vec4(vert, 1));
  vec4 surfaceColor = texture(tex, texcoord);
  vec3 surfaceToLight = normalize(light.position - surfacePos);

  float diffuseCoefficient = max(0.0, dot(wnormal, surfaceToLight));
  vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.color;

  vec3 ambient = light.ambientCoefficient * surfaceColor.rgb * light.color;

  float distanceToLight = length(light.position - surfacePos);
  float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

  vec3 linearColor = ambient + attenuation*diffuse;
  vec3 gamma = vec3(1.0/2.2);
  FragColor = vec4(pow(linearColor, gamma), surfaceColor.a);
}