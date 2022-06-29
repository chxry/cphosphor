#pragma once
#include <string.h>
#include <stdbool.h>
#include <cglm/cglm.h>

#include "engine/ecs/world.h"

extern bool inspector;
extern bool outline;
extern bool scene;
extern bool assets;
extern bool environment;
extern bool about;

extern int selected_entity;
extern vec3 light_dir;
