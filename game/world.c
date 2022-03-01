#include "world.h"

vec_gameobj_t gameobjs;
vec_collider_t colliders;

void world_init() {
  vec_init(&gameobjs);
  vec_init(&colliders);

  JSON_Object* root = json_object(json_parse_string(asset_load("test.json").data));
  JSON_Array* objects = json_object_get_array(root, "gameobjs");
  for (int i = 0; i < json_array_get_count(objects); i++) {
    JSON_Object* obj = json_array_get_object(objects, i);
    const char* mesh = json_object_get_string(obj, "mesh");
    const char* tex = json_object_get_string(obj, "tex");
    JSON_Array* pos = json_object_get_array(obj, "pos");
    JSON_Array* rot = json_object_get_array(obj, "rot");
    JSON_Array* scale = json_object_get_array(obj, "scale");
    gameobj_t gameobj = {.mesh = mesh, .tex = tex, .pos = VEC3_FROM_JSON(pos), .rot = VEC3_FROM_JSON(rot), .scale = VEC3_FROM_JSON(scale)};
    vec_push(&gameobjs, gameobj);
  }
  JSON_Array* acolliders = json_object_get_array(root, "colliders");
  for (int i = 0; i < json_array_get_count(acolliders); i++) {
    JSON_Object* obj = json_array_get_object(acolliders, i);
    JSON_Array* min = json_object_get_array(obj, "min");
    JSON_Array* max = json_object_get_array(obj, "max");
    collider_t collider = {.min = VEC3_FROM_JSON(min), .max = VEC3_FROM_JSON(max)};
    vec_push(&colliders, collider);
  }
}

void world_render(mat4 view, mat4 projection) {
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);

  int i;
  gameobj_t obj;
  vec_foreach(&gameobjs, obj, i) {
    tex_use(get_tex(obj.tex));
    mat4 model;
    glm_translate_make(model, obj.pos);
    glm_rotate_x(model, glm_rad(obj.rot[0]), model);
    glm_rotate_y(model, glm_rad(obj.rot[1]), model);
    glm_rotate_z(model, glm_rad(obj.rot[2]), model);
    glm_scale(model, obj.scale);
    shader_set_mat4(basic_shader, "model", model);
    mesh_render(get_mesh(obj.mesh, pos_tex_norm));
  }

  if (state.debug_drawcolliders) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shader_use(debug_shader);
    shader_set_mat4(debug_shader, "view", view);
    shader_set_mat4(debug_shader, "projection", projection);
    collider_t collider;
    vec_foreach(&colliders, collider, i) {
      mat4 model;
      vec3 center;
      vec3 size;
      glm_vec3_center(collider.min, collider.max, center);
      glm_vec3_sub(collider.max, collider.min, size);
      glm_vec3_divs(size, 2, size);
      glm_translate_make(model, center);
      glm_scale(model, size);
      shader_set_mat4(debug_shader, "model", model);
      mesh_render(get_mesh("mesh/sky.obj", pos));
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void world_render_shadows(mat4 view, mat4 projection) {
  shader_use(shadow_shader);
  shader_set_mat4(shadow_shader, "view", view);
  shader_set_mat4(shadow_shader, "projection", projection);

  int i;
  gameobj_t obj;
  vec_foreach(&gameobjs, obj, i) {
    mat4 model;
    glm_translate_make(model, obj.pos);
    glm_rotate_x(model, glm_rad(obj.rot[0]), model);
    glm_rotate_y(model, glm_rad(obj.rot[1]), model);
    glm_rotate_z(model, glm_rad(obj.rot[2]), model);
    glm_scale(model, obj.scale);
    shader_set_mat4(shadow_shader, "model", model);
    mesh_render(get_mesh(obj.mesh, pos_tex_norm));
  }
}