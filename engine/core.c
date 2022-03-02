#include "core.h"

lua_State* lua;
mtar_t tar;

map_t(mesh_t) meshes;
map_t(unsigned int) textures;

void assets_init(const char* path) {
  map_init(&meshes);
  map_init(&textures);
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
  if (mtar_read_data(&tar, asset.data, h.size)) {
    log_error("Failed to load asset \"%s\".", path);
  }
  return asset;
}

mesh_t get_mesh(const char* path, mesh_attr attr) {
  mesh_t* mesh;
  if ((mesh = map_get(&meshes, path))) {
    return *mesh;
  } else {
    mesh_t mesh = mesh_load_obj(path, attr);
    map_set(&meshes, path, mesh);
    return mesh;
  }
}

unsigned int get_tex(const char* path) {
  unsigned int* tex;
  if ((tex = map_get(&textures, path))) {
    return *tex;
  } else {
    unsigned int tex = tex_load(path, GL_RGB);
    if (!tex) {
      tex = get_tex("tex/missing.jpg");
    }
    map_set(&textures, path, tex);
    return tex;
  }
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
