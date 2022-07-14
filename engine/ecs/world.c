#include "world.h"

world_t world;

void world_load(const char* path) {
  vec_init(&world.entities);
  map_init(&world.components);
  component_register("model", model);
  component_register("boxcollider", boxcollider);
  component_register("text", text);
  component_register("rigidbody", rigidbody);

  JSON_Object* root = json_object(json_parse_file(path)); // use asset system
  world.light_ambient = json_object_dotget_number(root, "light.ambient");
  world.light_diffuse = json_object_dotget_number(root, "light.diffuse");
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
    entity_t entity = {.name = name, .id = id, .pos = VEC3_FROM_JSON(pos), .rot = VEC3_FROM_JSON(rot), .scale = VEC3_FROM_JSON(scale)};
    vec_push(&world.entities, entity);
  }

  JSON_Object* comps = json_object_get_object(root, "components");
  for (int i = 0; i < json_object_get_count(comps); i++) {
    JSON_Array* arr = json_value_get_array(json_object_get_value_at(comps, i));
    component_t* component = get_component(json_object_get_name(comps, i));
    for (int i = 0; i < json_array_get_count(arr); i++) {
      vec_push(&component->components, component->load(json_array_get_object(arr, i)));
    }
  }

  log_info("Loaded world \"%s\".", path);
  // grab from file
  // skybox_tex = tex_load_cubemap((char* [6]){"tex/sky/right.jpg", "tex/sky/left.jpg", "tex/sky/top.jpg", "tex/sky/bottom.jpg", "tex/sky/front.jpg", "tex/sky/back.jpg"}, GL_RGB);
}

void component_register(char* name, component_t component) {
  vec_init(&component.components);
  map_set(&world.components, name, component);
}

void component_iter(void (*f)(component_t* c, char* name)) {
  char* key;
  map_iter_t iter = map_iter(&world.components);
  while ((key = map_next(&world.components, &iter))) {
    f(get_component(key), key);
  }
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

component_t* get_component(char* name) {
  return map_get(&world.components, name);
}

int delete_id;
void delete_components(component_t* component, char* name) {
  int i;
  void* c;
  vec_foreach(&component->components, c, i) {
    if (*((int*)c) == delete_id) {
      vec_splice(&component->components, i, 1);
    }
  }
}

void entity_delete(int id) {
  int i;
  entity_t* entity;
  vec_foreach_ptr(&world.entities, entity, i) {
    if (entity->id == id) {
      vec_splice(&world.entities, i, 1);
      delete_id = id;
      component_iter(delete_components);
      return;
    }
  }
}

JSON_Object* comps;
void write_component(component_t* component, char* name) {
  int i;
  void* c;
  JSON_Value* csv = json_value_init_array();
  JSON_Array* cs = json_array(csv);
  vec_foreach(&component->components, c, i) {
    json_array_append_value(cs, component->save(c));
  }
  json_object_set_value(comps, name, csv);
}

void world_write(const char* path) {
  JSON_Value* rootv = json_value_init_object();
  JSON_Object* root = json_object(rootv);
  json_object_dotset_number(root, "light.ambient", world.light_ambient);
  json_object_dotset_number(root, "light.diffuse", world.light_diffuse);
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

  JSON_Value* compsv = json_value_init_object();
  comps = json_object(compsv);
  component_iter(write_component);
  json_object_set_value(root, "components", compsv);

  json_serialize_to_file(rootv, path);
  log_info("Wrote world \"%s\".", path);
}

bool world_test_collision(aabb_t box) {
  int i;
  boxcollider_t* collider;
  vec_foreach(&get_component("boxcollider")->components, collider, i) {
    entity_t* entity = get_entity(collider->entity);
    aabb_t global;
    glm_vec3_add(entity->pos, collider->b.min, global.min);
    glm_vec3_add(entity->pos, collider->b.max, global.max);
    if (glm_aabb_aabb((vec3*)&box, (vec3*)&global)) {
      return true;
    }
  }
  return false;
}

collision_t world_raycast(vec3 origin, vec3 dir) {
  collision_t collision = {.distance = 0};
  int i;
  boxcollider_t* collider;
  vec_foreach(&get_component("boxcollider")->components, collider, i) {
    entity_t* entity = get_entity(collider->entity);
    aabb_t global;
    glm_vec3_add(entity->pos, collider->b.min, global.min);
    glm_vec3_add(entity->pos, collider->b.max, global.max);
    float distance = aabb_raycast(origin, dir, global);
    if (distance == 0) {
      continue;
    }
    if (collision.distance == 0 || collision.distance > distance) {
      collision.entity = entity;
      collision.distance = distance;
    }
  }
  glm_vec3_scale(dir, collision.distance, collision.hit);
  glm_vec3_add(origin, collision.hit, collision.hit);

  return collision;
}
