#include "model.h"

model_t* model_load(JSON_Object* obj) {
  model_t* model = malloc(sizeof(model_t));
  model->entity = json_object_get_number(obj, "e");
  model->mesh = json_object_get_string(obj, "mesh");
  model->tex = json_object_get_string(obj, "tex");
  model->spec = json_object_get_number(obj, "spec");
  return model;
}

JSON_Value* model_save(model_t* model) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", model->entity);
  json_object_set_string(obj, "mesh", model->mesh);
  json_object_set_string(obj, "tex", model->tex);
  json_object_set_number(obj, "spec", model->spec);
  return objv;
}

void model_inspector(model_t* model, int i) {
  char buf[32];
  sprintf(buf, "Mesh##model%i", i);
  asset_selector(buf, model->mesh, "mesh");
  sprintf(buf, "Texture##model%i", i);
  asset_selector(buf, model->tex, "tex");
  igDragFloat("Specular", &model->spec, 0, 0.01, 1, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

model_t* model_create(int entity) {
  model_t* model = malloc(sizeof(model_t));
  model->entity = entity;
  model->mesh = malloc(14);
  strcpy(model->mesh, "mesh/cube.obj");
  model->tex = malloc(14);
  strcpy(model->tex, "tex/white.jpg");
  model->spec = 0.5;
  return model;
}

component_t model = {
    .load = model_load,
    .save = model_save,
    .inspector = model_inspector,
    .create = model_create,
    .inspector_label = ICON_FA_CUBE " Model"};
