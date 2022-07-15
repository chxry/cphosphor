#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cglm/cglm.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "core/gfx.h"
#include "log.h"

extern char* cpu_name;

void engine_init();
void lua_init();
void lua_exec(const char* buf);
void lua_bind(void* val, const char* name);
