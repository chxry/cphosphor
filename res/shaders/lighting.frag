#version 460 core
out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedospec;
uniform sampler2D shadowmap;
uniform float light_ambient;
uniform float light_diffuse;
uniform mat4 light_view;
uniform mat4 light_projection;
uniform vec3 sun_dir;
uniform vec3 sun_color;
uniform vec3 cam_dir;
uniform int shadow_softness;
struct light_t {
  vec3 pos;
  vec3 color;
  float radius;
  float strength;
};
uniform light_t lights[100];

vec3 lighting_calc(float specular, float distance, vec3 light_dir, vec3 light_color) {
  vec3 normal = texture(gNormal, texcoord).rgb;
  float diffuse = max(dot(normal, normalize(light_dir)), 0.0) * light_diffuse;
  float spec = pow(max(dot(normal, normalize(light_dir + cam_dir)), 0.0), 8.0) * specular;

  if (distance > 0.0) {
    float attenuation = 1.0 / distance;
    diffuse *= attenuation;
    spec *= attenuation;
  }

  return vec3(diffuse + spec) * light_color;
}

void main() {
  vec3 albedo = texture(gAlbedospec, texcoord).rgb;
  float specular = texture(gAlbedospec, texcoord).a;
  FragColor = vec4(albedo, 1.0);
  if (specular > 0) {
    vec3 frag_pos = texture(gPosition, texcoord).rgb;
    vec3 light = lighting_calc(specular, 0.0, sun_dir, sun_color);

    vec4 lightspace = light_projection * light_view * vec4(frag_pos, 1.0);
    lightspace = lightspace * 0.5 + 0.5;
    float current = lightspace.z - 0.0015;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    for (int x = -shadow_softness; x <= shadow_softness; ++x) {
      for (int y = -shadow_softness; y <= shadow_softness; ++y) {
        float closest = texture(shadowmap, lightspace.xy + vec2(x, y) * texelSize).r;
        shadow += current > closest ? 1.0 : 0.0;
      }
    }
    shadow /= pow(shadow_softness * 2 + 1, 2);
    light *= (1 - vec3(shadow));
    light += vec3(light_ambient);

    for (int i = 0; i < 100; i++) {
      if (lights[i].radius == 0) {
        break;
      }
      light += lighting_calc(specular, length(lights[i].pos - frag_pos) / lights[i].radius, normalize(lights[i].pos - frag_pos), lights[i].color * lights[i].strength);
    }

    FragColor.rgb *= light;
  }
}
