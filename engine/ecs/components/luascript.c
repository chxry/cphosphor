#include "luascript.h"

luascript_t* luascript_load(JSON_Object* obj) {
  luascript_t* luascript = malloc(sizeof(luascript_t));
  luascript->entity = json_object_get_number(obj, "e");
  luascript->path = json_object_get_string(obj, "path");
  return luascript;
}

JSON_Value* luascript_save(luascript_t* luascript) {
  JSON_Value* objv = json_value_init_object();
  JSON_Object* obj = json_object(objv);
  json_object_set_number(obj, "e", luascript->entity);
  json_object_set_string(obj, "path", luascript->path);
  return objv;
}

void luascript_inspector(luascript_t* luascript, int i) {
  char buf[32];
  sprintf(buf, "Path##luascript%i", i);
  asset_selector(buf, luascript->path, "luafile");
}

luascript_t* luascript_create(int entity) {
  luascript_t* luascript = malloc(sizeof(luascript_t));
  luascript->entity = entity;
  luascript->path = malloc(17);
  strcpy(luascript->path, "scripts/spin.lua");
  return luascript;
}

component_t luascript = {
    .load = luascript_load,
    .save = luascript_save,
    .inspector = luascript_inspector,
    .create = luascript_create,
    .inspector_label = ICON_FA_CODE " Lua Script"};
