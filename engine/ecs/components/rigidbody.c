#include "rigidbody.h"

rigidbody_t* rigidbody_load(JSON_Object* obj) {
  rigidbody_t* rb = malloc(sizeof(rigidbody_t));
  rb->entity = json_object_get_number(obj, "e");
  rb->collider = json_object_get_number(obj, "collider");
  rb->mass = json_object_get_number(obj, "mass");
  rb->friction = json_object_get_number(obj, "friction");
  rb->bounciness = json_object_get_number(obj, "bounciness");
  return rb;
}

JSON_Value* rigidbody_save(rigidbody_t* rb) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", rb->entity);
  json_object_set_number(obj, "collider", rb->collider);
  json_object_set_number(obj, "mass", rb->mass);
  json_object_set_number(obj, "friction", rb->friction);
  json_object_set_number(obj, "bounciness", rb->bounciness);
  return objv;
}

void rigidbody_inspector(rigidbody_t* rb, int i) {
  igCombo_Str("Collider", &rb->collider, "Cube\0Sphere\0Cylinder\0\0", 0);
  igDragFloat("Mass", &rb->mass, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
  igDragFloat("Friction", &rb->friction, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
  ui_helpmarker("Best results using friction > 0.");
  igDragFloat("Bounciness", &rb->bounciness, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
  ui_helpmarker("Best results using 0 bounciness.");
}

rigidbody_t* rigidbody_create(int entity) {
  rigidbody_t* rb = malloc(sizeof(rigidbody_t));
  rb->entity = entity;
  rb->collider = collider_cube;
  rb->mass = 1;
  rb->friction = 0.5;
  rb->bounciness = 0;
  return rb;
}

component_t rigidbody = {
    .load = rigidbody_load,
    .save = rigidbody_save,
    .inspector = rigidbody_inspector,
    .create = rigidbody_create,
    .inspector_label = ICON_FA_CUBES " Rigidbody"};
