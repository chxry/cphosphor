#pragma once
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cglm/cglm.h>
#include <microtar.h>
#include <vec.h>
#include <map.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "core/gfx.h"
#include "log.h"

typedef struct {
  void* data;
  int len;
} asset_t;

void engine_init();
void assets_init(const char* path);
asset_t asset_load(const char* path);
mesh_t get_mesh(const char* path, mesh_attr attr);
unsigned int get_tex(const char* path);
void lua_init();
void lua_exec(const char* buf);
void lua_bind(void* val, const char* name);
