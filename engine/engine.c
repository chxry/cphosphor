#include "engine.h"

lua_State* lua;

void engine_init() {
  srand(time(NULL));
  log_info("Floppa Engine v" VERSION ".");
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

void lua_init() {
  lua = luaL_newstate();
  luaopen_base(lua);
  log_info("Loaded %s.", LUA_VERSION);
  lua_pushcfunction(lua, lua_set);
  lua_setglobal(lua, "set");
}

void lua_exec(const char* buf) {
  log_lua("> %s", buf);
  luaL_loadstring(lua, buf);
  if (lua_pcall(lua, 0, -1, 0)) {
    const char* err = lua_tostring(lua, -1);
    log_error("Lua error: %s", err);
  }
}

void lua_bind(void* val, const char* name) {
  lua_pushlightuserdata(lua, val);
  lua_setglobal(lua, name);
}
