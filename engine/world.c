#include "world.h"

vec_t(gameobj_t) gameobjs;
vec_t(collider_t) colliders;

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
}

void world_render_colliders(mat4 view, mat4 projection) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  int i;
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

bool world_test_collision(collider_t box) {
  bool collides = false;
  int i;
  collider_t collider;
  vec_foreach(&colliders, collider, i) {
    if ((collides = glm_aabb_aabb((vec3*)&box, (vec3*)&collider))) {
      break;
    }
  }
  return collides;
}

//move
float aabb_raycast(vec3 origin, vec3 dir, collider_t box) {
  float t[10];
  t[1] = (box.min[0] - origin[0]) / dir[0];
  t[2] = (box.max[0] - origin[0]) / dir[0];
  t[3] = (box.min[1] - origin[1]) / dir[1];
  t[4] = (box.max[1] - origin[1]) / dir[1];
  t[5] = (box.min[2] - origin[2]) / dir[2];
  t[6] = (box.max[2] - origin[2]) / dir[2];
  t[7] = MAX(MAX(MIN(t[1], t[2]), MIN(t[3], t[4])), MIN(t[5], t[6]));
  t[8] = MIN(MIN(MAX(t[1], t[2]), MAX(t[3], t[4])), MAX(t[5], t[6]));
  t[9] = (t[8] < 0 || t[7] > t[8]) ? 0 : t[7];
  return t[9];
}

float world_raycast(vec3 origin, vec3 dir) {
  float closest = 0;
  int i;
  collider_t collider;
  vec_foreach(&colliders, collider, i) {
    float distance = aabb_raycast(origin, dir, collider);
    if (distance == 0) {
      continue;
    }
    if (closest == 0) {
      closest = distance;
      continue;
    }
    closest = distance < closest ? distance : closest;
  }
  return closest;
}
