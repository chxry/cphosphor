#include "game.h"

conf_t conf;
state_t state = {
    .world_gravity = 0.0025,
    .player_speed = 0.0075,
    .player_jumpheight = 0.1,
    .debug_drawcolliders = false};

const char* keybind_names[KEYBINDS] = {"forward", "back", "left", "right", "jump", "crouch", "sprint", "menu", "console", "debug"};
const char* keybind_names_h[KEYBINDS] = {"Move Forward", "Move Backward", "Move Left", "Move Right", "Jump", "Crouch", "Sprint", "Open Menu", "Open console", "Show debug info"};

void conf_load(const char* path) {
  JSON_Object* root = json_object(json_parse_file("conf.json"));
  JSON_Array* res = json_object_dotget_array(root, "gfx.res");
  conf.width = json_array_get_number(res, 0);
  conf.height = json_array_get_number(res, 1);
  conf.fullscreen = json_object_dotget_boolean(root, "gfx.fullscreen");
  conf.fps = json_object_dotget_number(root, "gfx.fps");
  conf.fov = json_object_dotget_number(root, "gfx.fov");
  conf.sens = json_object_dotget_number(root, "input.sens");
  conf.volume = json_object_dotget_number(root, "audio.volume");
  for (int i = 0; i < KEYBINDS; i++) {
    char buf[64] = "input.keys.";
    conf.binds[i] = SDL_GetScancodeFromName(json_object_dotget_string(root, strcat(buf, keybind_names[i])));
  }
  conf.crosshair_size = json_object_dotget_number(root, "crosshair.size");
  conf.crosshair_thickness = json_object_dotget_number(root, "crosshair.thickness");
  conf.crosshair_color = json_object_dotget_number(root, "crosshair.color");

  log_info("Loaded config \"%s\".", path);
}

void conf_write(const char* path) {
  JSON_Value* rootv = json_value_init_object();
  JSON_Object* root = json_object(rootv);
  JSON_Value* resv = json_value_init_array();
  JSON_Array* res = json_array(resv);
  json_array_append_number(res, conf.width);
  json_array_append_number(res, conf.height);
  json_object_dotset_value(root, "gfx.res", resv);
  json_object_dotset_boolean(root, "gfx.fullscreen", conf.fullscreen);
  json_object_dotset_number(root, "gfx.fps", conf.fps);
  json_object_dotset_number(root, "gfx.fov", conf.fov);
  json_object_dotset_number(root, "input.sens", conf.sens);
  json_object_dotset_number(root, "audio.volume", conf.volume);
  for (int i = 0; i < KEYBINDS; i++) {
    char buf[64] = "input.keys.";
    json_object_dotset_string(root, strcat(buf, keybind_names[i]), SDL_GetScancodeName(conf.binds[i]));
  }
  json_object_dotset_number(root, "crosshair.size", conf.crosshair_size);
  json_object_dotset_number(root, "crosshair.thickness", conf.crosshair_thickness);
  json_object_dotset_number(root, "crosshair.color", conf.crosshair_color);

  json_serialize_to_file_pretty(rootv, path);
  log_info("Wrote config \"%s\".", path);
}
