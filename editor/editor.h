#pragma once
#include <stdbool.h>
#include <string.h>

#include "engine/ecs/world.h"

extern bool inspector_open;
extern bool outline_open;
extern bool scene_open;
extern bool assets_open;
extern bool environment_open;
extern bool info_open;

extern int selected_entity;
extern char name_buf[256];

void set_selected_entity(int entity);
