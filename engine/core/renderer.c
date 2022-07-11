#include "renderer.h"

mesh_t quad;
unsigned int gbuffer, gposition, gnormal, galbedospec, depthbuffer;
unsigned int shadowbuffer, shadowmap;

void renderer_init(int width, int height) {
  glGenFramebuffers(1, &gbuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);

  glGenTextures(1, &gposition);
  glBindTexture(GL_TEXTURE_2D, gposition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gposition, 0);

  glGenTextures(1, &gnormal);
  glBindTexture(GL_TEXTURE_2D, gnormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gnormal, 0);

  glGenTextures(1, &galbedospec);
  glBindTexture(GL_TEXTURE_2D, galbedospec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap, 0);

  float verts[] = {
      1, 1, 0, 1, 1,
      1, -1, 0, 1, 0,
      -1, 1, 0, 0, 1,
      1, -1, 0, 1, 0,
      -1, -1, 0, 0, 0,
      -1, 1, 0, 0, 1};
  quad = mesh_init(verts, 6, pos_tex);
}

void renderer_resize(int width, int height) {
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

void renderer_render(unsigned int fbo, mat4 view, mat4 projection, int x, int y) {
  mat4 light_view, light_projection;
  glm_lookat(world.light_dir, (vec3){0, 0, 0}, GLM_YUP, light_view);
  glm_ortho(-50, 50, -50, 50, 0.01, 50, light_projection);
  glViewport(0, 0, SHADOW_RES, SHADOW_RES);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  world_render_shadows(light_view, light_projection);
  glViewport(0, 0, x, y);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  world_render(view, projection);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader_use(lighting_shader);
  shader_set_float(lighting_shader, "light_ambient", world.light_ambient);
  shader_set_float(lighting_shader, "light_diffuse", world.light_diffuse);
  shader_set_vec3(lighting_shader, "light_dir", world.light_dir);
  shader_set_vec3(lighting_shader, "light_color", world.light_color);
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
