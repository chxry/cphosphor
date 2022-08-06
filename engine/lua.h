#pragma once
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <vec.h>

#include "ecs/world.h"
#include "log.h"

void lua_init();
void lua_exec(char* buf);
void lua_update();
