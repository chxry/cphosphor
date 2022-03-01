#include "lighting.h"

unsigned int shadow_shader;
unsigned int lighting_shader;
mesh_t quad;
unsigned int gbuffer, gposition, gnormal, galbedospec, depthbuffer;
unsigned int shadowbuffer, shadowmap;

void gbuffer_init(int width, int height) {
  glGenFramebuffers(1, &gbuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);

  glGenTextures(1, &gposition);
  glBindTexture(GL_TEXTURE_2D, gposition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gposition, 0);

  glGenTextures(1, &gnormal);
  glBindTexture(GL_TEXTURE_2D, gnormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gnormal, 0);

  glGenTextures(1, &galbedospec);
  glBindTexture(GL_TEXTURE_2D, galbedospec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, galbedospec, 0);

  glDrawBuffers(3, (GLenum[3]){GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
  glGenRenderbuffers(1, &depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

  glGenFramebuffers(1, &shadowbuffer);
  glGenTextures(1, &shadowmap);
  glBindTexture(GL_TEXTURE_2D, shadowmap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               SHADOW_RES, SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap, 0);

  shadow_shader = shader_init("shaders/shadow.vert", "shaders/shadow.frag");
  lighting_shader = shader_init("shaders/pass.vert", "shaders/lighting.frag");
  shader_use(lighting_shader);
  shader_set_int(lighting_shader, "gPosition", 0);
  shader_set_int(lighting_shader, "gNormal", 1);
  shader_set_int(lighting_shader, "gAlbedospec", 2);
  shader_set_int(lighting_shader, "shadowmap", 3);

  float verts[] = {
      1, 1, 0, 1, 1,
      1, -1, 0, 1, 0,
      -1, 1, 0, 0, 1,
      1, -1, 0, 1, 0,
      -1, -1, 0, 0, 0,
      -1, 1, 0, 0, 1};
  quad = mesh_init(verts, 6, pos_tex);
}

void gbuffer_resize(int width, int height) {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
  glBindTexture(GL_TEXTURE_2D, gposition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, gnormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, galbedospec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void gbuffer_render_shadows(mat4 light_view, mat4 light_projection) {
  glm_lookat(state.world_lightdir, (vec3){0, 0, 0}, GLM_YUP, light_view);
  glm_ortho(-50, 50, -50, 50, 0.01, 50, light_projection);
  glViewport(0, 0, SHADOW_RES, SHADOW_RES);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  world_render_shadows(light_view, light_projection);
}

void gbuffer_render(mat4 light_view, mat4 light_projection) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader_use(lighting_shader);
  shader_set_vec3(lighting_shader, "light_dir", state.world_lightdir);
  shader_set_vec3(lighting_shader, "light_color", (vec3){1.0, 1.0, 1.0});
  shader_set_mat4(lighting_shader, "light_view", light_view);
  shader_set_mat4(lighting_shader, "light_projection", light_projection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gposition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gnormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, galbedospec);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, shadowmap);
  mesh_render(quad);
}
