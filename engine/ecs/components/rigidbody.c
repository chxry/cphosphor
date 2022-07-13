#include "rigidbody.h"

rigidbody_t* rigidbody_load(JSON_Object* obj) {
  rigidbody_t* rb = malloc(sizeof(rigidbody_t));
  rb->entity = json_object_get_number(obj, "e");
  rb->mass = json_object_get_number(obj, "mass");
  return rb;
}

JSON_Value* rigidbody_save(rigidbody_t* rb) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", rb->entity);
  json_object_set_number(obj, "mass", rb->mass);
  return objv;
}

void rigidbody_inspector(rigidbody_t* rb, int i) {
  igDragFloat("Mass", &rb->mass, 0.1, 0, 0, "%.5g", ImGuiInputTextFlags_None);
}

rigidbody_t* rigidbody_create(int entity) {
  rigidbody_t* rb = malloc(sizeof(rigidbody_t));
  rb->entity = entity;
  rb->mass = 1;
  return rb;
}

component_t rigidbody = {
    .load = rigidbody_load,
    .save = rigidbody_save,
    .inspector = rigidbody_inspector,
    .create = rigidbody_create,
    .inspector_label = ICON_FA_CUBES " Rigidbody"};
