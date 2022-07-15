#include "text.h"

text_t* text_load(JSON_Object* obj) {
  text_t* text = malloc(sizeof(text_t));
  text->entity = json_object_get_number(obj, "e");
  text->text = json_object_get_string(obj, "text");
  strcpy(text->text_buf, text->text);
  return text;
}

JSON_Value* text_save(text_t* text) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", text->entity);
  json_object_set_string(obj, "text", text->text);
  return objv;
}

void text_inspector(text_t* text, int i) {
  char buf[32];
  sprintf(buf, "Text##text%i", i);
  if (igInputText(buf, text->text_buf, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
    text->text = realloc(text->text, strlen(text->text_buf));
    strcpy(text->text, text->text_buf);
  }
}

text_t* text_create(int entity) {
  text_t* text = malloc(sizeof(text_t));
  text->entity = entity;
  text->text = malloc(5);
  strcpy(text->text, "text");
  strcpy(text->text_buf, "text");
  return text;
}

component_t text = {
    .load = text_load,
    .save = text_save,
    .inspector = text_inspector,
    .create = text_create,
    .inspector_label = ICON_FA_FONT " Text"};
