#include "world.h"

vec_entity_t entities;

void world_load(const char* path) {
  vec_init(&entities);

  JSON_Array* objects = json_array(json_parse_string(asset_load(path).data));
  for (int i = 0; i < json_array_get_count(objects); i++) {
    JSON_Object* obj = json_array_get_object(objects, i);
    const char* name = json_object_get_string(obj, "name");
    JSON_Array* pos = json_object_get_array(obj, "pos");
    JSON_Array* rot = json_object_get_array(obj, "rot");
    JSON_Array* scale = json_object_get_array(obj, "scale");
    const char* mesh = json_object_get_string(obj, "mesh");
    const char* tex = json_object_get_string(obj, "tex");
    JSON_Array* acolliders = json_object_get_array(obj, "colliders");
    vec_collider_t colliders;
    vec_init(&colliders);
    for (int i = 0; i < json_array_get_count(acolliders); i++) {
      JSON_Object* colobj = json_array_get_object(acolliders, i);
      JSON_Array* min = json_object_get_array(colobj, "min");
      JSON_Array* max = json_object_get_array(colobj, "max");
      collider_t collider = {.min = VEC3_FROM_JSON(min), .max = VEC3_FROM_JSON(max)};
      vec_push(&colliders, collider);
    }
    entity_t entity = {.name = name, .mesh = mesh, .tex = tex, .pos = VEC3_FROM_JSON(pos), .rot = VEC3_FROM_JSON(rot), .scale = VEC3_FROM_JSON(scale), .colliders = colliders};
    vec_push(&entities, entity);
  }
  log_info("Loaded world \"%s\".", path);
}

// move
JSON_Value* json_vec3(vec3 vec) {
  JSON_Value* arrv = json_value_init_array();
  JSON_Array* arr = json_array(arrv);
  json_array_append_number(arr, vec[0]);
  json_array_append_number(arr, vec[1]);
  json_array_append_number(arr, vec[2]);
  return arrv;
}

void world_write(const char* path) {
  JSON_Value* rootv = json_value_init_array();
  JSON_Array* objects = json_array(rootv);

  int i, j;
  entity_t entity;
  collider_t collider;
  vec_foreach(&entities, entity, i) {
    JSON_Value* objv = json_value_init_object();
    JSON_Object* obj = json_object(objv);
    json_object_set_string(obj, "name", entity.name);
    json_object_set_string(obj, "mesh", entity.mesh);
    json_object_set_string(obj, "tex", entity.tex);
    json_object_set_value(obj, "pos", json_vec3(entity.pos));
    json_object_set_value(obj, "rot", json_vec3(entity.rot));
    json_object_set_value(obj, "scale", json_vec3(entity.scale));
    JSON_Value* colsv = json_value_init_array();
    JSON_Array* cols = json_array(colsv);
    vec_foreach(&entity.colliders, collider, j) {
      JSON_Value* colobjv = json_value_init_object();
      JSON_Object* colobj = json_object(colobjv);
      json_object_set_value(colobj, "min", json_vec3(collider.min));
      json_object_set_value(colobj, "max", json_vec3(collider.max));
      json_array_append_value(cols, colobjv);
    }
    json_object_set_value(obj, "colliders", colsv);
    json_array_append_value(objects, objv);
  }

  json_serialize_to_file(rootv, path);
  log_info("Wrote world \"%s\".", path);
}

void world_render(mat4 view, mat4 projection) {
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);

  int i;
  entity_t entity;
  vec_foreach(&entities, entity, i) {
    tex_use(get_tex(entity.tex));
    mat4 model;
    glm_translate_make(model, entity.pos);
    glm_rotate_x(model, glm_rad(entity.rot[0]), model);
    glm_rotate_y(model, glm_rad(entity.rot[1]), model);
    glm_rotate_z(model, glm_rad(entity.rot[2]), model);
    glm_scale(model, entity.scale);
    shader_set_mat4(basic_shader, "model", model);
    mesh_render(get_mesh(entity.mesh, pos_tex_norm));
  }
}

void world_render_colliders(mat4 view, mat4 projection) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  mat4 model;
  vec3 center;
  vec3 size;
  int i, j;
  entity_t entity;
  collider_t collider;
  vec_foreach(&entities, entity, i) {
    vec_foreach(&entity.colliders, collider, j) {
      glm_vec3_center(collider.min, collider.max, center);
      glm_vec3_sub(collider.max, collider.min, size);
      glm_vec3_divs(size, 2, size);
      glm_translate_make(model, center);
      glm_scale(model, size);
      shader_set_mat4(debug_shader, "model", model);
      mesh_render(get_mesh("mesh/sky.obj", pos));
    }
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void world_render_shadows(mat4 view, mat4 projection) {
  shader_use(shadow_shader);
  shader_set_mat4(shadow_shader, "view", view);
  shader_set_mat4(shadow_shader, "projection", projection);

  int i;
  entity_t entity;
  vec_foreach(&entities, entity, i) {
    mat4 model;
    glm_translate_make(model, entity.pos);
    glm_rotate_x(model, glm_rad(entity.rot[0]), model);
    glm_rotate_y(model, glm_rad(entity.rot[1]), model);
    glm_rotate_z(model, glm_rad(entity.rot[2]), model);
    glm_scale(model, entity.scale);
    shader_set_mat4(shadow_shader, "model", model);
    mesh_render(get_mesh(entity.mesh, pos_tex_norm));
  }
}

bool world_test_collision(collider_t box) {
  bool collides = false;
  int i, j;
  entity_t entity;
  collider_t collider;
  vec_foreach(&entities, entity, i) {
    vec_foreach(&entity.colliders, collider, j) {
      if ((collides = glm_aabb_aabb((vec3*)&box, (vec3*)&collider))) {
        goto br;
      }
    }
  }
br:
  return collides;
}

// move
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
  int i, j;
  entity_t entity;
  collider_t collider;
  vec_foreach(&entities, entity, i) {
    vec_foreach(&entity.colliders, collider, j) {
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
  }

  return closest;
}
