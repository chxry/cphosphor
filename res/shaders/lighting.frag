#version 460 core
#define ambient 0.3
out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedospec;
uniform sampler2D shadowmap;
uniform mat4 light_view;
uniform mat4 light_projection;
uniform vec3 viewpos;
uniform vec3 light_dir;
uniform vec3 light_color;

void main() {
  vec3 fragpos = texture(gPosition, texcoord).rgb;
  vec3 normal = texture(gNormal, texcoord).rgb;
  vec3 albedo = texture(gAlbedospec, texcoord).rgb;
  float specular = texture(gAlbedospec, texcoord).a;

  float diffuse = specular > 0 ? max(dot(normal, normalize(light_dir)), 0.0) : 1.0-ambient;

  vec4 lightspace = light_projection * light_view * vec4(fragpos,1.0);
  lightspace = lightspace * 0.5 + 0.5;
  float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
  float current = lightspace.z-bias;
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float closest = texture(shadowmap, lightspace.xy + vec2(x, y) * texelSize).r;
      shadow += current - bias > closest ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  vec3 light = vec3(ambient + (1-shadow) * diffuse);
  FragColor = vec4(light * albedo,1.0);
}
