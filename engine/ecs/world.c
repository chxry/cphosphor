#include "world.h"

world_t world;
unsigned int skybox_tex;

void world_load(const char* path) {
  vec_init(&world.entities);
  vec_init(&world.models);
  vec_init(&world.colliders);

  JSON_Object* root = json_object(json_parse_string(asset_load(path).data));
  world.light_ambient = json_object_dotget_number(root, "light.ambient");
  glm_vec3_copy((vec3)VEC3_FROM_JSON(json_object_dotget_array(root, "light.dir")), world.light_dir);
  glm_vec3_copy((vec3)VEC3_FROM_JSON(json_object_dotget_array(root, "light.color")), world.light_color);
  world.sky_mode = json_object_dotget_number(root, "sky.mode");

  JSON_Array* entities = json_object_get_array(root, "entities");
  for (int i = 0; i < json_array_get_count(entities); i++) {
    JSON_Object* obj = json_array_get_object(entities, i);
    const char* name = json_object_get_string(obj, "name");
    int id = json_object_get_number(obj, "id");
    JSON_Array* pos = json_object_get_array(obj, "pos");
    JSON_Array* rot = json_object_get_array(obj, "rot");
    JSON_Array* scale = json_object_get_array(obj, "scale");
    // todo dont realloc name
    entity_t entity = {.name = realloc((void*)name, 256), .id = id, .pos = VEC3_FROM_JSON(pos), .rot = VEC3_FROM_JSON(rot), .scale = VEC3_FROM_JSON(scale)};
    vec_push(&world.entities, entity);
  }

  JSON_Array* models = json_object_dotget_array(root, "components.model");
  for (int i = 0; i < json_array_get_count(models); i++) {
    vec_push(&world.models, model_load(json_array_get_object(models, i)));
  }

  JSON_Array* colliders = json_object_dotget_array(root, "components.collider");
  for (int i = 0; i < json_array_get_count(colliders); i++) {
    vec_push(&world.colliders, collider_load(json_array_get_object(colliders, i)));
  }
  log_info("Loaded world \"%s\".", path);
  // grab from file
  skybox_tex = tex_load_cubemap((char* [6]){"tex/sky/right.jpg", "tex/sky/left.jpg", "tex/sky/top.jpg", "tex/sky/bottom.jpg", "tex/sky/front.jpg", "tex/sky/back.jpg"}, GL_RGB);
}

entity_t* get_entity(int id) {
  int i;
  entity_t* entity;
  vec_foreach_ptr(&world.entities, entity, i) {
    if (entity->id == id) {
      return entity;
    }
  }
  return 0;
}

void entity_delete(int id) {
  int i;
  entity_t* entity;
  vec_foreach_ptr(&world.entities, entity, i) {
    if (entity->id == id) {
      vec_splice(&world.entities, i, 1);
      // free
      return;
    }
  }
}

void world_write(const char* path) {
  JSON_Value* rootv = json_value_init_object();
  JSON_Object* root = json_object(rootv);
  json_object_dotset_number(root, "light.ambient", world.light_ambient);
  json_object_dotset_value(root, "light.dir", json_vec3(world.light_dir));
  json_object_dotset_value(root, "light.color", json_vec3(world.light_color));
  json_object_dotset_number(root, "sky.mode", world.sky_mode);

  int i;
  entity_t entity;
  JSON_Value* entitiesv = json_value_init_array();
  JSON_Array* entities = json_array(entitiesv);
  vec_foreach(&world.entities, entity, i) {
    JSON_Value* objv = json_value_init_object();
    JSON_Object* obj = json_object(objv);
    json_object_set_string(obj, "name", entity.name);
    json_object_set_number(obj, "id", entity.id);
    json_object_set_value(obj, "pos", json_vec3(entity.pos));
    json_object_set_value(obj, "rot", json_vec3(entity.rot));
    json_object_set_value(obj, "scale", json_vec3(entity.scale));
    json_array_append_value(entities, objv);
  }
  json_object_set_value(root, "entities", entitiesv);

  model_t model;
  JSON_Value* modelsv = json_value_init_array();
  JSON_Array* models = json_array(modelsv);
  vec_foreach(&world.models, model, i) {
    json_array_append_value(models, model_save(model));
  }
  json_object_dotset_value(root, "components.model", modelsv);

  collider_t collider;
  JSON_Value* collidersv = json_value_init_array();
  JSON_Array* colliders = json_array(collidersv);
  vec_foreach(&world.colliders, collider, i) {
    JSON_Value* objv = json_value_init_object();
    JSON_Object* obj = json_object(objv);
    json_object_set_number(obj, "e", collider.entity);
    json_object_set_value(obj, "min", json_vec3(collider.b.min));
    json_object_set_value(obj, "max", json_vec3(collider.b.max));
    json_array_append_value(colliders, objv);
  }
  json_object_dotset_value(root, "components.collider", collidersv);

  json_serialize_to_file(rootv, path);
  log_info("Wrote world \"%s\".", path);
}

void world_render(mat4 view, mat4 projection) {
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);

  int i;
  model_t model;
  vec_foreach(&world.models, model, i) {
    entity_t* entity = get_entity(model.entity);
    tex_use(get_tex(model.tex));
    mat4 modelm;
    glm_translate_make(modelm, entity->pos);
    glm_rotate_x(modelm, glm_rad(entity->rot[0]), modelm);
    glm_rotate_y(modelm, glm_rad(entity->rot[1]), modelm);
    glm_rotate_z(modelm, glm_rad(entity->rot[2]), modelm);
    glm_scale(modelm, entity->scale);
    shader_set_mat4(basic_shader, "model", modelm);
    mesh_render(get_mesh(model.mesh, pos_tex_norm));
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
  collider_t collider;
  vec_foreach(&world.colliders, collider, i) {
    entity_t* entity = get_entity(collider.entity);
    glm_vec3_center(collider.b.min, collider.b.max, center);
    glm_vec3_add(entity->pos, center, center);
    glm_vec3_sub(collider.b.max, collider.b.min, size);
    glm_vec3_divs(size, 2, size);
    glm_translate_make(model, center);
    glm_scale(model, size);
    shader_set_mat4(debug_shader, "model", model);
    mesh_render(get_mesh("mesh/sky.obj", pos));
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void world_render_collider(mat4 view, mat4 projection, int entity) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(debug_shader);
  shader_set_mat4(debug_shader, "view", view);
  shader_set_mat4(debug_shader, "projection", projection);

  mat4 model;
  vec3 center;
  vec3 size;
  int i;
  collider_t collider;
  vec_foreach(&world.colliders, collider, i) {
    if (collider.entity == entity) {
      entity_t* entity = get_entity(collider.entity);
      glm_vec3_center(collider.b.min, collider.b.max, center);
      glm_vec3_add(entity->pos, center, center);
      glm_vec3_sub(collider.b.max, collider.b.min, size);
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
  model_t model;
  vec_foreach(&world.models, model, i) {
    entity_t* entity = get_entity(model.entity);
    mat4 modelm;
    glm_translate_make(modelm, entity->pos);
    glm_rotate_x(modelm, glm_rad(entity->rot[0]), modelm);
    glm_rotate_y(modelm, glm_rad(entity->rot[1]), modelm);
    glm_rotate_z(modelm, glm_rad(entity->rot[2]), modelm);
    glm_scale(modelm, entity->scale);
    shader_set_mat4(shadow_shader, "model", modelm);
    mesh_render(get_mesh(model.mesh, pos_tex_norm));
  }
}

bool world_test_collision(aabb_t box) {
  int i;
  collider_t collider;
  vec_foreach(&world.colliders, collider, i) {
    entity_t* entity = get_entity(collider.entity);
    aabb_t global;
    glm_vec3_add(entity->pos, collider.b.min, global.min);
    glm_vec3_add(entity->pos, collider.b.max, global.max);
    if (glm_aabb_aabb((vec3*)&box, (vec3*)&global)) {
      return true;
    }
  }
  return false;
}

float world_raycast(vec3 origin, vec3 dir) {
  float closest = 0;
  int i;
  collider_t collider;
  vec_foreach(&world.colliders, collider, i) {
    entity_t* entity = get_entity(collider.entity);
    aabb_t global;
    glm_vec3_add(entity->pos, collider.b.min, global.min);
    glm_vec3_add(entity->pos, collider.b.max, global.max);
    float distance = aabb_raycast(origin, dir, global);
    if (distance == 0) {
      continue;
    }
    if (closest == 0 || closest > distance) {
      closest = distance;
    }
  }

  return closest;
}
