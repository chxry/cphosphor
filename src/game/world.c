#include "world.h"

vec_t(gameobj_t) gameobjs;
map_t(mesh_t) meshes;
map_t(unsigned int) textures;

void world_init() {
  vec_init(&gameobjs);
  map_init(&meshes);
  map_init(&textures);

  JSON_Value* root = json_parse_string(asset_load("test.json").data);
  JSON_Array* objects = json_value_get_array(root);
  for (int i = 0; i < json_array_get_count(objects); i++) {
    JSON_Object* obj = json_array_get_object(objects, i);
    const char* mesh = json_object_get_string(obj, "mesh");
    const char* tex = json_object_get_string(obj, "tex");
    if (!map_get(&meshes, mesh)) {
      map_set(&meshes, mesh, mesh_load_obj(mesh, pos_tex_norm));
    }
    if (!map_get(&textures, tex)) {
      map_set(&textures, tex, tex_load(tex, GL_RGB));
    }
    JSON_Array* pos = json_object_get_array(obj, "pos");
    JSON_Array* rot = json_object_get_array(obj, "rot");
    JSON_Array* scale = json_object_get_array(obj, "scale");
    gameobj_t gameobj = {.mesh = mesh, .tex = tex, .pos = VEC3_FROM_JSON(pos), .rot = VEC3_FROM_JSON(rot), .scale = VEC3_FROM_JSON(scale)};
    vec_push(&gameobjs, gameobj);
  }
}

void world_render(mat4 view, mat4 projection) {
  shader_use(basic_shader);
  shader_set_mat4(basic_shader, "view", view);
  shader_set_mat4(basic_shader, "projection", projection);
  shader_set_vec3(basic_shader, "light.position", GLM_YUP);
  shader_set_vec3(basic_shader, "light.color", GLM_VEC3_ONE);
  shader_set_float(basic_shader, "light.attenuation", 0.2);
  shader_set_float(basic_shader, "light.ambientCoefficient", 0.005);

  int i;
  gameobj_t obj;
  vec_foreach(&gameobjs, obj, i) {
    tex_use(*map_get(&textures, obj.tex));
    mat4 model;
    glm_translate_make(model, obj.pos);
    glm_rotate_x(model, glm_rad(obj.rot[0]), model);
    glm_rotate_y(model, glm_rad(obj.rot[1]), model);
    glm_rotate_z(model, glm_rad(obj.rot[2]), model);
    glm_scale(model, obj.scale);
    shader_set_mat4(basic_shader, "model", model);
    mesh_render(*map_get(&meshes, obj.mesh));
  }
}