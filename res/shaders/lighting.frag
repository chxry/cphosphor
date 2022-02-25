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
  float closest = texture(shadowmap, lightspace.xy).r; 
  float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
  float current = lightspace.z-bias;
  float shadow = current < closest ? 1.0 : 0.0;

  vec3 light = vec3(ambient + shadow * diffuse);
  FragColor = vec4(light * albedo,1.0);
}