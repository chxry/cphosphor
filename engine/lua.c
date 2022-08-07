#include "lua.h"

lua_State* lua;

int l_setpos(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  entity->pos[0] = lua_tonumber(L, 2);
  entity->pos[1] = lua_tonumber(L, 3);
  entity->pos[2] = lua_tonumber(L, 4);
  // needs to be updated in bullet (same for other values)
  return 0;
}

int l_getpos(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  lua_pushnumber(L, entity->pos[0]);
  lua_pushnumber(L, entity->pos[1]);
  lua_pushnumber(L, entity->pos[2]);
  return 3;
}

int l_setrot(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  entity->rot[0] = lua_tonumber(L, 2);
  entity->rot[1] = lua_tonumber(L, 3);
  entity->rot[2] = lua_tonumber(L, 4);
  return 0;
}

int l_getrot(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  lua_pushnumber(L, entity->rot[0]);
  lua_pushnumber(L, entity->rot[1]);
  lua_pushnumber(L, entity->rot[2]);
  return 3;
}

int l_setscale(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  entity->scale[0] = lua_tonumber(L, 2);
  entity->scale[1] = lua_tonumber(L, 3);
  entity->scale[2] = lua_tonumber(L, 4);
  return 0;
}

int l_getscale(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  lua_pushnumber(L, entity->scale[0]);
  lua_pushnumber(L, entity->scale[1]);
  lua_pushnumber(L, entity->scale[2]);
  return 3;
}

int l_getname(lua_State* L) {
  entity_t* entity = get_entity(lua_tonumber(L, 1));
  lua_pushstring(L, entity->name);
  return 1;
}

luaL_Reg entity_funcs[] = {
    {"setpos", l_setpos},
    {"getpos", l_getpos},
    {"setrot", l_setrot},
    {"getrot", l_getrot},
    {"setscale", l_setscale},
    {"getscale", l_getscale},
    {"getname", l_getname},
    {NULL, NULL}};

int l_getcolor(lua_State* L) {
  int entity = lua_tonumber(L, 1);
  int i;
  light_t* light;
  vec_foreach(&get_component("light")->components, light, i) {
    if (light->entity == entity) {
      lua_pushnumber(L, light->color[0]);
      lua_pushnumber(L, light->color[1]);
      lua_pushnumber(L, light->color[2]);
      break;
    }
  }
  return 3;
}

int l_setcolor(lua_State* L) {
  int entity = lua_tonumber(L, 1);
  int i;
  light_t* light;
  vec_foreach(&get_component("light")->components, light, i) {
    if (light->entity == entity) {
      light->color[0] = lua_tonumber(L, 2);
      light->color[1] = lua_tonumber(L, 3);
      light->color[2] = lua_tonumber(L, 4);
      break;
    }
  }
  return 0;
}

int l_getstrength(lua_State* L) {
  int entity = lua_tonumber(L, 1);
  int i;
  light_t* light;
  vec_foreach(&get_component("light")->components, light, i) {
    if (light->entity == entity) {
      lua_pushnumber(L, light->strength);
      break;
    }
  }
  return 1;
}

int l_setstrength(lua_State* L) {
  int entity = lua_tonumber(L, 1);
  int i;
  light_t* light;
  vec_foreach(&get_component("light")->components, light, i) {
    if (light->entity == entity) {
      light->strength = lua_tonumber(L, 2);
      break;
    }
  }
  return 0;
}

luaL_Reg light_funcs[] = {
    {"getcolor", l_getcolor},
    {"setcolor", l_setcolor},
    {"getstrength", l_getstrength},
    {"setstrength", l_setstrength},
    {NULL, NULL}};

int l_applyimpulse(lua_State* L) {
  int entity = lua_tonumber(L, 1);
  int i;
  rigidbody_t* rb;
  vec_foreach(&get_component("rigidbody")->components, rb, i) {
    if (rb->entity == entity) {
      bodyApplyImpulse(rb->body, (vec3){lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)}, GLM_VEC3_ZERO);
      break;
    }
  }
  return 0;
}

luaL_Reg rb_funcs[] = {
    {"applyimpulse", l_applyimpulse},
    {NULL, NULL}};

int l_getkeydown(lua_State* L) {
  const unsigned char* keys = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keys[SDL_GetScancodeFromName(lua_tostring(L, 1))]);
  return 1;
}

luaL_Reg input_funcs[] = {
    {"getkeydown", l_getkeydown},
    {NULL, NULL}};

int l_eq(lua_State* L) {
  lua_pushboolean(L, fabs(lua_tonumber(L, 1) - lua_tonumber(L, 2)) < 0.01);
  return 1;
}

int l_print(lua_State* L) {
  int n = lua_gettop(L);
  char buf[512] = "";
  for (int i = 1; i <= n; i++) {
    strcat(buf, lua_tostring(L, i));
    strcat(buf, " ");
  }
  log_lua(buf);
  return 0;
}

void lua_init() {
  lua = luaL_newstate();
  luaopen_base(lua);
  luaL_setfuncs(lua, entity_funcs, 0);
  lua_pushvalue(lua, -1);
  lua_setglobal(lua, "entity");
  luaL_setfuncs(lua, light_funcs, 0);
  lua_pushvalue(lua, -1);
  lua_setglobal(lua, "light");
  luaL_setfuncs(lua, rb_funcs, 0);
  lua_pushvalue(lua, -1);
  lua_setglobal(lua, "rb");
  luaL_setfuncs(lua, input_funcs, 0);
  lua_pushvalue(lua, -1);
  lua_setglobal(lua, "input");
  lua_pushcfunction(lua, l_eq);
  lua_setglobal(lua, "eq");
  lua_pushcfunction(lua, l_print);
  lua_setglobal(lua, "print");
  log_info("Loaded " LUA_VERSION);
}

void lua_exec(char* buf) {
  log_lua("> %s", buf);
  luaL_loadstring(lua, buf);
  if (lua_pcall(lua, 0, -1, 0)) {
    char* err = lua_tostring(lua, -1);
    log_error("Lua error: %s", err);
  }
}

void lua_call_scripts(char* func) {
  int i;
  luascript_t* luascript;
  vec_foreach(&get_component("luascript")->components, luascript, i) {
    lua_pushnumber(lua, luascript->entity);
    lua_setglobal(lua, "self");
    lua_pushnil(lua);
    lua_setglobal(lua, func);
    if (luaL_dostring(lua, get_luafile(luascript->path)->contents)) {
      char* err = lua_tostring(lua, -1);
      log_error("Lua error in \"%s\" (called by %i/%i): %s", luascript->path, luascript->entity, i, err);
      continue;
    }
    lua_getglobal(lua, func);
    if (lua_pcall(lua, 0, 0, 0)) {
      lua_pop(lua, 1);
    }
  }
}
