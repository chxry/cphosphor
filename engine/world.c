#include "world.h"

world_t world;
unsigned int skybox_tex;

void world_load(const char* path) {
  vec_init(&world.entities);

  JSON_Object* root = json_object(json_parse_string(asset_load(path).data));
  world.light_ambient = json_object_dotget_number(root, "light.ambient");
  glm_vec3_copy((vec3)VEC3_FROM_JSON(json_object_dotget_array(root, "light.dir")), world.light_dir);
  glm_vec3_copy((vec3)VEC3_FROM_JSON(json_object_dotget_array(root, "light.color")), world.light_color);
  world.sky_mode = json_object_dotget_number(root, "sky.mode");
  JSON_Array* entities = json_object_get_array(root, "entities");
  for (int i = 0; i < json_array_get_count(entities); i++) {
    JSON_Object* obj = json_array_get_object(entities, i);
    const char* name = json_object_get_string(obj, "name");
    JSON_Array* pos = json_object_get_array(obj, "pos");
    JSON_Array* rot = json_object_get_array(obj, "rot");
    JSON_Array* scale = json_object_get_array(obj, "scale");
    model_t* model = NULL;
    if (json_object_get_object(obj, "model")) {
      const char* mesh = json_object_dotget_string(obj, "model.mesh");
      const char* tex = json_object_dotget_string(obj, "model.tex");
      model = malloc(sizeof(model_t));
      model->mesh = mesh;
      model->tex = tex;
    }
    collider_t* collider = NULL;
    if (json_object_get_object(obj, "collider")) {
      JSON_Array* min = json_object_dotget_array(obj, "collider.min");
      JSON_Array* max = json_object_dotget_array(obj, "collider.max");
      collider = malloc(sizeof(collider_t));
      glm_vec3_copy((vec3)VEC3_FROM_JSON(min), collider->min);
      glm_vec3_copy((vec3)VEC3_FROM_JSON(max), collider->max);
    }
    entity_t entity = {.name = realloc((void*)name, 256), .pos = VEC3_FROM_JSON(pos), .rot = VEC3_FROM_JSON(rot), .scale = VEC3_FROM_JSON(scale), .model = model, .collider = collider};
    vec_push(&world.entities, entity);
  }
  log_info("Loaded world \"%s\".", path);
  // grab from file
  skybox_tex = tex_load_cubemap((char* [6]){"tex/sky/right.jpg", "tex/sky/left.jpg", "tex/sky/top.jpg", "tex/sky/bottom.jpg", "tex/sky/front.jpg", "tex/sky/back.jpg"}, GL_RGB);
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
  JSON_Value* rootv = json_value_init_object();
  JSON_Object* root = json_object(rootv);
  JSON_Value* entitiesv = json_value_init_array();
  JSON_Array* entities = json_array(entitiesv);
  json_object_dotset_number(root, "light.ambient", world.light_ambient);
  json_object_dotset_value(root, "light.dir", json_vec3(world.light_dir));
  json_object_dotset_value(root, "light.color", json_vec3(world.light_color));
  json_object_dotset_number(root, "sky.mode", world.sky_mode);

  int i;
  entity_t entity;
  vec_foreach(&world.entities, entity, i) {
    JSON_Value* objv = json_value_init_object();
    JSON_Object* obj = json_object(objv);
    json_object_set_string(obj, "name", entity.name);
    json_object_set_value(obj, "pos", json_vec3(entity.pos));
    json_object_set_value(obj, "rot", json_vec3(entity.rot));
    json_object_set_value(obj, "scale", json_vec3(entity.scale));
    if (entity.model) {
      json_object_dotset_string(obj, "model.mesh", entity.model->mesh);
      json_object_dotset_string(obj, "model.tex", entity.model->tex);
    }
    if (entity.collider) {
      json_object_dotset_value(obj, "collider.min", json_vec3(entity.collider->min));
      json_object_dotset_value(obj, "collider.max", json_vec3(entity.collider->max));
    }
    json_array_append_value(entities, objv);
  }
  json_object_set_value(root, "entities", entitiesv);
  json_serialize_to_file(rootv, path);
  log_info("Wrote world \"%s\".", path);
}

void world_render(mat4 view, mat4 projection) {
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);

  int i;
  entity_t entity;
  vec_foreach(&world.entities, entity, i) {
    if (!entity.model) {
      continue;
    }
    tex_use(get_tex(entity.model->tex));
    mat4 model;
    glm_translate_make(model, entity.pos);
    glm_rotate_x(model, glm_rad(entity.rot[0]), model);
    glm_rotate_y(model, glm_rad(entity.rot[1]), model);
    glm_rotate_z(model, glm_rad(entity.rot[2]), model);
    glm_scale(model, entity.scale);
    shader_set_mat4(basic_shader, "model", model);
    mesh_render(get_mesh(entity.model->mesh, pos_tex_norm));
  }

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
  mesh_render(get_mesh("mesh/sky.obj", pos));
  glDepthFunc(GL_LESS);
}

void world_render_colliders(mat4 view, mat4 projection) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  mat4 model;
  vec3 center;
  vec3 size;
  int i;
  entity_t entity;
  vec_foreach(&world.entities, entity, i) {
    if (!entity.collider) {
      continue;
    }
    glm_vec3_center(entity.collider->min, entity.collider->max, center);
    glm_vec3_add(entity.pos, center, center);
    glm_vec3_sub(entity.collider->max, entity.collider->min, size);
    glm_vec3_divs(size, 2, size);
    glm_translate_make(model, center);
    glm_scale(model, size);
    shader_set_mat4(debug_shader, "model", model);
    mesh_render(get_mesh("mesh/sky.obj", pos));
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void world_render_collider(mat4 view, mat4 projection, int entity) {
  collider_t* collider = world.entities.data[entity].collider;
  if (!collider) {
    return;
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  mat4 model;
  vec3 center;
  vec3 size;
  glm_vec3_center(collider->min, collider->max, center);
  glm_vec3_add(world.entities.data[entity].pos, center, center);
  glm_vec3_sub(collider->max, collider->min, size);
  glm_vec3_divs(size, 2, size);
  glm_translate_make(model, center);
  glm_scale(model, size);
  shader_set_mat4(debug_shader, "model", model);
  mesh_render(get_mesh("mesh/sky.obj", pos));
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void world_render_shadows(mat4 view, mat4 projection) {
  shader_use(shadow_shader);
  shader_set_mat4(shadow_shader, "view", view);
  shader_set_mat4(shadow_shader, "projection", projection);

  int i;
  entity_t entity;
  vec_foreach(&world.entities, entity, i) {
    if (!entity.model) {
      continue;
    }
    mat4 model;
    glm_translate_make(model, entity.pos);
    glm_rotate_x(model, glm_rad(entity.rot[0]), model);
    glm_rotate_y(model, glm_rad(entity.rot[1]), model);
    glm_rotate_z(model, glm_rad(entity.rot[2]), model);
    glm_scale(model, entity.scale);
    shader_set_mat4(shadow_shader, "model", model);
    mesh_render(get_mesh(entity.model->mesh, pos_tex_norm));
  }
}

bool world_test_collision(collider_t box) {
  int i;
  entity_t entity;
  vec_foreach(&world.entities, entity, i) {
    if (!entity.collider) {
      continue;
    }
    collider_t local;
    glm_vec3_add(entity.pos, entity.collider->min, local.min);
    glm_vec3_add(entity.pos, entity.collider->max, local.max);
    if (glm_aabb_aabb((vec3*)&box, (vec3*)&local)) {
      return true;
    }
  }
  return false;
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
  int i;
  entity_t entity;
  vec_foreach(&world.entities, entity, i) {
    if (!entity.collider) {
      continue;
    }
    collider_t local;
    glm_vec3_add(entity.pos, entity.collider->min, local.min);
    glm_vec3_add(entity.pos, entity.collider->max, local.max);
    float distance = aabb_raycast(origin, dir, local);
    if (distance == 0) {
      continue;
    }
    if (closest == 0 || closest > distance) {
      closest = distance;
    }
  }

  return closest;
}
