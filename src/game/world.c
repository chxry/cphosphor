#include "world.h"

float floor_verts[] = {
    -15.0, -1.0, -15.0, 0.0, 1.0,
    15.0, -1.0, -15.0, 1.0, 1.0,
    15.0, -1.0, 15.0, 1.0, 0.0,
    15.0, -1.0, 15.0, 1.0, 0.0,
    -15.0, -1.0, 15.0, 0.0, 0.0,
    -15.0, -1.0, -15.0, 0.0, 1.0};

mesh_t floor_mesh;
unsigned int floor_tex;
mesh_t teapot_mesh;

void world_init() {
  floor_mesh = mesh_init_pos_tex(floor_verts, 6);
  floor_tex = tex_load("cat.png", GL_RGBA);
  teapot_mesh = mesh_load_obj("teapot.obj");
}

void world_render(mat4 view, mat4 projection) {
  shader_use(basic_shader);
  tex_use(floor_tex);
  shader_set_mat4(basic_shader, "model", GLM_MAT4_IDENTITY);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);
  mesh_render(floor_mesh);

  shader_use(blank_shader);
  mat4 teapot_model = GLM_MAT4_IDENTITY;
  glm_rotate_y(teapot_model, SDL_GetTicks() / 450.0, teapot_model);
  shader_set_mat4(blank_shader, "model", teapot_model);
  shader_set_mat4(blank_shader, "view", view);
  shader_set_mat4(blank_shader, "projection", projection);
  mesh_render(teapot_mesh);
}