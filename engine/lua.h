#pragma once
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <vec.h>
#include <capi/capi.h>
#include <SDL2/SDL.h>

#include "ecs/world.h"
#include "log.h"

void lua_init();
void lua_exec(char* buf);
void lua_call_scripts(char* func);
