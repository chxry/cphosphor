#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "ecs/world.h"

typedef struct {
  int entity;
  const char* mesh;
  const char* tex;
  // todo only in editor mode
  char mesh_buf[256];
  char tex_buf[256];
} model_t;

extern component_t model;