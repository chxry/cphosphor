#include "game.h"

conf_t conf;
lua_State* lua;
mtar_t tar;

state_t state = {
    .world_gravity = 0.003,
    .player_speed = 0.07,
    .player_jumpheight = 0.1,
    .debug_drawcolliders = false};

const char* keybind_names[KEYBINDS] = {"forward", "back", "left", "right", "jump", "menu", "console", "debug"};
const char* keybind_names_h[KEYBINDS] = {"Move Forward", "Move Backward", "Move Left", "Move Right", "Jump", "Open Menu", "Open console", "Show debug info"};

void conf_init(const char* path) {
  JSON_Object* root = json_object(json_parse_file("conf.json"));
  JSON_Array* res = json_object_dotget_array(root, "gfx.res");
  conf.width = json_array_get_number(res, 0);
  conf.height = json_array_get_number(res, 1);
  conf.fullscreen = json_object_dotget_boolean(root, "gfx.fullscreen");
  conf.fps = json_object_dotget_number(root, "gfx.fps");
  conf.msaa = json_object_dotget_number(root, "gfx.msaa");
  conf.fov = json_object_dotget_number(root, "gfx.fov");
  conf.sens = json_object_dotget_number(root, "input.sens");
  for (int i = 0; i < KEYBINDS; i++) {
    char buf[64] = "input.keys.";
    conf.binds[i] = SDL_GetScancodeFromName(json_object_dotget_string(root, strcat(buf, keybind_names[i])));
  }

  log_info("Loaded config \"%s\".", path);
}

void conf_write(const char* path) {
  JSON_Value* rootv = json_value_init_object();
  JSON_Object* root = json_object(rootv);
  JSON_Value* resv = json_value_init_array();
  JSON_Array* res = json_array(resv);
  json_array_append_value(res, json_value_init_number(conf.width));
  json_array_append_value(res, json_value_init_number(conf.height));
  json_object_dotset_value(root, "gfx.res", resv);
  json_object_dotset_boolean(root, "gfx.fullscreen", conf.fullscreen);
  json_object_dotset_number(root, "gfx.fps", conf.fps);
  json_object_dotset_number(root, "gfx.msaa", conf.msaa);
  json_object_dotset_number(root, "gfx.fov", conf.fov);
  json_object_dotset_number(root, "input.sens", conf.sens);
  for (int i = 0; i < KEYBINDS; i++) {
    char buf[64] = "input.keys.";
    json_object_dotset_string(root, strcat(buf, keybind_names[i]), SDL_GetScancodeName(conf.binds[i]));
  }

  json_serialize_to_file_pretty(rootv, "conf.json");
  log_info("Wrote config \"%s\".", path);
}

void assets_init(const char* path) {
  if (mtar_open(&tar, path, "r")) {
    log_error("Could not load \"%s\".", path);
    exit(-1);
  }
  log_info("Loaded \"%s\".", path);
}

asset_t asset_load(const char* path) {
  asset_t asset;
  mtar_header_t h;
  char base[64] = "res/";
  mtar_find(&tar, strcat(base, path), &h);
  asset.len = h.size;
  asset.data = calloc(1, h.size + 1);
  mtar_read_data(&tar, asset.data, h.size);
  return asset;
}

int lua_set(lua_State* l) {
  if (!lua_isuserdata(l, 1)) {
    luaL_error(l, "value doesnt exist.");
    return 0;
  }
  if (lua_isnumber(l, 2)) {
    *(float*)lua_touserdata(l, 1) = lua_tonumber(l, 2);
  } else {
    *(bool*)lua_touserdata(l, 1) = lua_toboolean(l, 2);
  }
  return 0;
}

void lua_bind(lua_State* l, void* val, const char* name) {
  lua_pushlightuserdata(l, val);
  lua_setglobal(l, name);
}

void lua_init() {
  lua = luaL_newstate();
  luaopen_base(lua);
  log_info("Loaded %s.", LUA_VERSION);
  lua_pushcfunction(lua, lua_set);
  lua_setglobal(lua, "set");

  lua_bind(lua, &state.world_gravity, "world_gravity");
  lua_bind(lua, &state.player_speed, "player_speed");
  lua_bind(lua, &state.player_jumpheight, "player_jumpheight");
  lua_bind(lua, &state.debug_drawcolliders, "debug_drawcolliders");
}

void lua_exec(const char* buf, bool log) {
  if (log) {
    log_lua("> %s", buf);
  }
  luaL_loadstring(lua, buf);
  if (lua_pcall(lua, 0, -1, 0)) {
    const char* err = lua_tostring(lua, -1);
    log_error("Lua error: %s", err);
  }
}
