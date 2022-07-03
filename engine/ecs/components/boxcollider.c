#include "boxcollider.h"

boxcollider_t* boxcollider_load(JSON_Object* obj) {
  JSON_Array* min = json_object_get_array(obj, "min");
  JSON_Array* max = json_object_get_array(obj, "max");
  boxcollider_t* collider = malloc(sizeof(boxcollider_t));
  collider->entity = json_object_get_number(obj, "e");
  glm_vec3_copy((vec3)VEC3_FROM_JSON(min), collider->b.min);
  glm_vec3_copy((vec3)VEC3_FROM_JSON(max), collider->b.max);
  return collider;
}

JSON_Value* boxcollider_save(boxcollider_t* collider) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", collider->entity);
  json_object_set_value(obj, "min", json_vec3(collider->b.min));
  json_object_set_value(obj, "max", json_vec3(collider->b.max));
  return objv;
}

void boxcollider_inspector(boxcollider_t* collider, int i) {
  char buf[32];
  sprintf(buf, "Min##boxcollider%i", i);
  igDragFloat3(buf, collider->b.min, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
  sprintf(buf, "Max##boxcollider%i", i);
  igDragFloat3(buf, collider->b.max, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
}

boxcollider_t* boxcollider_create(int entity) {
  boxcollider_t* collider = malloc(sizeof(boxcollider_t));
  collider->entity = entity;
  glm_vec3_copy(GLM_VEC3_ZERO, collider->b.min);
  glm_vec3_copy(GLM_VEC3_ONE, collider->b.max);
  return collider;
}

component_t boxcollider = {
    .load = boxcollider_load,
    .save = boxcollider_save,
    .inspector = boxcollider_inspector,
    .create = boxcollider_create,
    .inspector_label = ICON_FA_CUBES " Box Collider"};
