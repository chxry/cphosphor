#include "renderer.h"

mesh_t quad;
unsigned int gbuffer, gposition, gnormal, galbedospec, depthbuffer;
unsigned int shadowbuffer, shadowmap;
unsigned int skybox_tex;

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
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void renderer_render(unsigned int fbo, mat4 view, mat4 projection, int x, int y, int colliders) {
  mat4 light_view, light_projection;
  glm_lookat(world.light_dir, (vec3){0, 0, 0}, GLM_YUP, light_view);
  glm_ortho(-50, 50, -50, 50, 0.01, 50, light_projection);
  glViewport(0, 0, SHADOW_RES, SHADOW_RES);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  shader_use(shadow_shader);
  shader_set_mat4(shadow_shader, "view", light_view);
  shader_set_mat4(shadow_shader, "projection", light_projection);

  int i;
  model_t* model;
  vec_foreach(&get_component("model")->components, model, i) {
    entity_t* entity = get_entity(model->entity);
    mat4 modelm;
    glm_translate_make(modelm, entity->pos);
    glm_rotate_x(modelm, glm_rad(entity->rot[0]), modelm);
    glm_rotate_y(modelm, glm_rad(entity->rot[1]), modelm);
    glm_rotate_z(modelm, glm_rad(entity->rot[2]), modelm);
    glm_scale(modelm, entity->scale);
    shader_set_mat4(shadow_shader, "model", modelm);
    mesh_render(*get_mesh(model->mesh));
  }

  glViewport(0, 0, x, y);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);

  vec_foreach(&get_component("model")->components, model, i) {
    entity_t* entity = get_entity(model->entity);
    tex_use(get_tex(model->tex)->tex);
    mat4 modelm;
    glm_translate_make(modelm, entity->pos);
    glm_rotate_x(modelm, glm_rad(entity->rot[0]), modelm);
    glm_rotate_y(modelm, glm_rad(entity->rot[1]), modelm);
    glm_rotate_z(modelm, glm_rad(entity->rot[2]), modelm);
    glm_scale(modelm, entity->scale);
    shader_set_mat4(basic_shader, "model", modelm);
    mesh_render(*get_mesh(model->mesh));
  }

  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  rigidbody_t* rb;
  vec_foreach(&get_component("rigidbody")->components, rb, i) {
    entity_t* entity = get_entity(rb->entity);
    if (colliders < 0 || entity->id == colliders) {
      mat4 modelm;
      mesh_t mesh;
      glm_translate_make(modelm, entity->pos);
      glm_rotate_x(modelm, glm_rad(entity->rot[0]), modelm);
      glm_rotate_y(modelm, glm_rad(entity->rot[1]), modelm);
      glm_rotate_z(modelm, glm_rad(entity->rot[2]), modelm);
      switch (rb->collider) {
      case collider_cube:
        mesh = *get_mesh("mesh/cube.obj");
        glm_scale(modelm, entity->scale);
        break;
      case collider_sphere:
        mesh = *get_mesh("mesh/sphere.obj");
        glm_scale_uni(modelm, (entity->scale[0] + entity->scale[1] + entity->scale[2]) / 3);
        break;
      case collider_cylinder:
        mesh = *get_mesh("mesh/cylinder.obj");
        float r = (entity->scale[0] + entity->scale[2]) / 2;
        glm_scale(modelm, (vec3){r, entity->scale[1], r});
        break;
      }
      shader_set_mat4(debug_shader, "model", modelm);
      mesh_render(mesh);
    }
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDepthFunc(GL_LEQUAL);
  mat4 skybox_view = GLM_MAT4_ZERO_INIT;
  mat3 view3;
  glm_mat4_pick3(view, view3);
  glm_mat4_ins3(view3, skybox_view);
  switch (world.sky_mode) {
  case skybox:
    shader_use(skybox_shader);
    tex_use_cubemap(skybox_tex);
    shader_set_mat4(skybox_shader, "view", skybox_view);
    shader_set_mat4(skybox_shader, "projection", projection);
    shader_set_vec3(skybox_shader, "light_dir", world.light_dir);
    break;
  case atmosphere:
    shader_use(atmosphere_shader);
    shader_set_mat4(atmosphere_shader, "view", skybox_view);
    shader_set_mat4(atmosphere_shader, "projection", projection);
    shader_set_vec3(atmosphere_shader, "light_dir", world.light_dir);
    break;
  }
  mesh_render(*get_mesh("mesh/cube.obj"));
  glDepthFunc(GL_LESS);

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
