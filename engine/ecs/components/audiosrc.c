#include "audiosrc.h"

audiosrc_t* audiosrc_load(JSON_Object* obj) {
  audiosrc_t* audiosrc = malloc(sizeof(audiosrc_t));
  audiosrc->entity = json_object_get_number(obj, "e");
  audiosrc->sound = json_object_get_string(obj, "sound");
  audiosrc->autoplay = json_object_get_boolean(obj, "autoplay");
  return audiosrc;
}

JSON_Value* audiosrc_save(audiosrc_t* audiosrc) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", audiosrc->entity);
  json_object_set_string(obj, "sound", audiosrc->sound);
  json_object_set_boolean(obj, "autoplay", audiosrc->autoplay);
  return objv;
}

void audiosrc_inspector(audiosrc_t* audiosrc, int i) {
  char buf[32];
  sprintf(buf, "Sound##audiosrc%i", i);
  asset_selector(buf, audiosrc->sound, "sound");
  sprintf(buf, "Play on start##audiosrc%i", i);
  igCheckbox(buf, &audiosrc->autoplay);
}

audiosrc_t* audiosrc_create(int entity) {
  audiosrc_t* audiosrc = malloc(sizeof(audiosrc_t));
  audiosrc->entity = entity;
  audiosrc->sound = malloc(19);
  strcpy(audiosrc->sound, "audio/turiipip.mp3");
  audiosrc->autoplay = false;
  return audiosrc;
}

component_t audiosrc = {
    .load = audiosrc_load,
    .save = audiosrc_save,
    .inspector = audiosrc_inspector,
    .create = audiosrc_create,
    .inspector_label = ICON_FA_VOLUME_UP " Audio Source"};
