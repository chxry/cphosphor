#include "model.h"

model_t* model_load(JSON_Object* obj) {
  model_t* model = malloc(sizeof(model_t));
  model->entity = json_object_get_number(obj, "e");
  model->mesh = json_object_get_string(obj, "mesh");
  strcpy(model->mesh_buf, model->mesh);
  model->tex = json_object_get_string(obj, "tex");
  strcpy(model->tex_buf, model->tex);
  return model;
}

JSON_Value* model_save(model_t* model) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", model->entity);
  json_object_set_string(obj, "mesh", model->mesh);
  json_object_set_string(obj, "tex", model->tex);
  return objv;
}

void model_inspector(model_t* model) {
  if (igInputText("Mesh", model->mesh_buf, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
    model->mesh = realloc(model->mesh, strlen(model->mesh_buf));
    strcpy(model->mesh, model->mesh_buf);
  }
  if (igInputText("Texture", model->tex_buf, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
    model->tex = realloc(model->tex, strlen(model->tex_buf));
    strcpy(model->tex, model->tex_buf);
  }
}

model_t* model_create(int entity) {
  model_t* model = malloc(sizeof(model_t));
  model->entity = entity;
  model->mesh = malloc(14);
  strcpy(model->mesh, "mesh/cube.obj");
  strcpy(model->mesh_buf, "mesh/cube.obj");
  model->tex = malloc(13);
  strcpy(model->tex, "tex/flop.jpg");
  strcpy(model->tex_buf, "tex/flop.jpg");
  return model;
}

component_t model = {
    .name = "model",
    .load = model_load,
    .save = model_save,
    .inspector = model_inspector,
    .create = model_create,
    .inspector_label = ICON_FA_CUBE " Model"};
