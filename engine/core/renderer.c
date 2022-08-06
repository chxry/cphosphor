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

void render_shadows(mat4 light_view, mat4 light_projection) {
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
}

void render_models(mat4 view, mat4 projection, vec3 cam_pos) {
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);
  shader_set_vec3(basic_shader, "cam_pos", cam_pos);
  tex_use_cubemap(get_cubemap(768313171)->tex, GL_TEXTURE0);

  int i;
  model_t* model;
  vec_foreach(&get_component("model")->components, model, i) {
    entity_t* entity = get_entity(model->entity);
    tex_use(get_tex(model->tex)->tex, GL_TEXTURE1);
    mat4 modelm;
    glm_translate_make(modelm, entity->pos);
    glm_rotate_x(modelm, glm_rad(entity->rot[0]), modelm);
    glm_rotate_y(modelm, glm_rad(entity->rot[1]), modelm);
    glm_rotate_z(modelm, glm_rad(entity->rot[2]), modelm);
    glm_scale(modelm, entity->scale);
    shader_set_mat4(basic_shader, "model", modelm);
    shader_set_float(basic_shader, "spec", model->spec);
    shader_set_float(basic_shader, "reflectivity", model->reflectivity);
    mesh_render(*get_mesh(model->mesh));
  }
}

void render_colliders(mat4 view, mat4 projection, int colliders) {
  glDepthFunc(GL_LEQUAL);
  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  int i;
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
  glDepthFunc(GL_LESS);
}

void render_billboard_component(vec_void_t components, unsigned int tex, vec3 cam_pos) {
  tex_use(tex, GL_TEXTURE0);
  int i;
  void* component;
  vec_foreach(&components, component, i) {
    entity_t* entity = get_entity(*(int*)component);
    mat4 model;
    glm_translate_make(model, entity->pos);
    float dis = glm_vec3_distance(entity->pos, cam_pos) / 40.0;
    shader_set_float(billboard_shader, "distance", dis + 0.2);
    shader_set_mat4(billboard_shader, "model", model);
    mesh_render(quad);
  }
}

void render_billboards(mat4 view, mat4 projection, vec3 cam_pos) {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_ALWAYS);
  shader_use(billboard_shader);
  shader_set_mat4(billboard_shader, "view", view);
  shader_set_mat4(billboard_shader, "projection", projection);
  render_billboard_component(get_component("light")->components, get_tex("tex/light.png")->tex, cam_pos);
  render_billboard_component(get_component("audiosrc")->components, get_tex("tex/audiosrc.png")->tex, cam_pos);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDepthFunc(GL_LESS);
}

void render_skybox(mat4 view, mat4 projection) {
  glDepthFunc(GL_LEQUAL);
  mat4 skybox_view = GLM_MAT4_ZERO_INIT;
  mat3 view3;
  glm_mat4_pick3(view, view3);
  glm_mat4_ins3(view3, skybox_view);
  switch (world.sky_mode) {
  case skybox:
    shader_use(skybox_shader);
    tex_use_cubemap(get_cubemap(768313171)->tex, GL_TEXTURE0);
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
}

void renderer_render(unsigned int fbo, vec3 cam_pos, vec3 cam_dir, float fov, int x, int y, int colliders, bool billboards) {
  mat4 view, projection;
  glm_look(cam_pos, cam_dir, GLM_YUP, view);
  glm_perspective(glm_rad(fov), (float)x / (float)y, 0.1, 100.0, projection);

  mat4 light_view, light_projection;
  glm_lookat(world.light_dir, (vec3){0, 0, 0}, GLM_YUP, light_view);
  vec4 ndc[8] = {
      {-1.0, -1.0, 0.0, 1.0},
      {1.0, -1.0, 0.0, 1.0},
      {-1.0, 1.0, 0.0, 1.0},
      {1.0, 1.0, 0.0, 1.0},
      {-1.0, -1.0, 1.0, 1.0},
      {1.0, -1.0, 1.0, 1.0},
      {-1.0, 1.0, 1.0, 1.0},
      {1.0, 1.0, 1.0, 1.0},
  };
  mat4 inv;
  glm_mat4_mul(projection, view, inv);
  glm_mat4_inv_fast(inv, inv);
  vec3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
  vec3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
  for (int i = 0; i < 8; i++) {
    glm_mat4_mulv(inv, ndc[i], ndc[i]);
    glm_mat4_mulv(light_view, ndc[i], ndc[i]);
    glm_vec4_divs(ndc[i], ndc[i][3], ndc[i]);

    min[0] = MIN(min[0], ndc[i][0]);
    max[0] = MAX(max[0], ndc[i][0]);
    min[1] = MIN(min[1], ndc[i][1]);
    max[1] = MAX(max[1], ndc[i][1]);
    min[2] = MIN(min[2], ndc[i][2]);
    max[2] = MAX(max[2], ndc[i][2]);
  }
  glm_ortho(min[0], max[0], min[1], max[1], -max[2], -min[2], light_projection);

  // Shadow pass
  glViewport(0, 0, SHADOW_RES, SHADOW_RES);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  render_shadows(light_view, light_projection);

  // Geometry pass
  glViewport(0, 0, x, y);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  render_models(view, projection, cam_pos);
  render_colliders(view, projection, colliders);
  render_skybox(view, projection);

  // Lighting pass
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader_use(lighting_shader);
  shader_set_float(lighting_shader, "light_ambient", world.light_ambient);
  shader_set_float(lighting_shader, "light_diffuse", world.light_diffuse);
  shader_set_vec3(lighting_shader, "sun_dir", world.light_dir);
  shader_set_vec3(lighting_shader, "sun_color", world.light_color);
  shader_set_int(lighting_shader, "shadow_softness", world.shadow_softness);
  shader_set_mat4(lighting_shader, "light_view", light_view);
  shader_set_mat4(lighting_shader, "light_projection", light_projection);
  shader_set_vec3(lighting_shader, "cam_dir", cam_dir);
  int i = 0;
  light_t* light;
  char buf[32];
  vec_foreach(&get_component("light")->components, light, i) {
    entity_t* entity = get_entity(light->entity);
    snprintf(buf, sizeof(buf), "lights[%i].pos", i);
    shader_set_vec3(lighting_shader, buf, entity->pos);
    snprintf(buf, sizeof(buf), "lights[%i].color", i);
    shader_set_vec3(lighting_shader, buf, light->color);
    snprintf(buf, sizeof(buf), "lights[%i].strength", i);
    shader_set_float(lighting_shader, buf, light->strength);
  }
  snprintf(buf, sizeof(buf), "lights[%i].strength", i);
  shader_set_float(lighting_shader, buf, -1);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gposition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gnormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, galbedospec);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, shadowmap);
  mesh_render(quad);
  if (billboards) {
    render_billboards(view, projection, cam_pos);
  }
}
