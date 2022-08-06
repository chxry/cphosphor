#version 460 core
out float FragColor;

in vec2 texcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform vec3 kernel[128];
uniform mat4 projection;

float radius = 0.5;
float bias = 0.025;

void main() {
  vec3 fragpos = texture(gPosition, texcoord).rgb;
  vec3 normal = texture(gNormal, texcoord).rgb;

  float occlusion = 0.0;
  for (int i = 0; i < 128; i++) {
    vec3 samplepos = fragpos + kernel[i];
    vec4 offset = vec4(samplepos, 1.0);
    offset = projection * offset;
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;

    float sampleDepth = texture(gPosition, offset.xy).z;
    occlusion += (sampleDepth >= samplepos.z + bias ? 1.0 : 0.0);
  }

  occlusion = 1.0 - occlusion / 128.0;
  FragColor = occlusion;
}
