#pragma once
#include <string.h>
#include <stdbool.h>
#include <cglm/cglm.h>

#include "engine/world.h"

extern bool inspector;
extern bool outline;
extern bool scene;
extern bool assets;
extern bool environment;

extern int selected_entity;
extern vec3 light_dir;
extern char mesh_mesh_buf[256];
extern char mesh_tex_buf[256];

void set_selected_entity(int i);