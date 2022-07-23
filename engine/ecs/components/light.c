#include "light.h"

light_t* light_load(JSON_Object* obj) {
  light_t* light = malloc(sizeof(light_t));
  light->entity = json_object_get_number(obj, "e");
  JSON_Array* color = json_object_get_array(obj, "color");
  glm_vec3_copy((vec3)VEC3_FROM_JSON(color), light->color);
  light->radius = json_object_get_number(obj, "radius");
  light->strength = json_object_get_number(obj, "strength");
  return light;
}

JSON_Value* light_save(light_t* light) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", light->entity);
  json_object_set_value(obj, "color", json_vec3(light->color));
  json_object_set_number(obj, "radius", light->radius);
  json_object_set_number(obj, "strength", light->strength);
  return objv;
}

void light_inspector(light_t* light, int i) {
  igColorEdit3("Color", light->color, ImGuiColorEditFlags_NoInputs);
  igDragFloat("Radius", &light->radius, 0.01, 0.01, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
  igDragFloat("Strength", &light->strength, 0.01, 0.01, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

light_t* light_create(int entity) {
  light_t* light = malloc(sizeof(light_t));
  light->entity = entity;
  glm_vec3_copy(GLM_VEC3_ONE, light->color);
  light->radius = 1.0;
  light->strength = 1.0;
  return light;
}

component_t light = {
    .load = light_load,
    .save = light_save,
    .inspector = light_inspector,
    .create = light_create,
    .inspector_label = ICON_FA_LIGHTBULB_O " Light"};
