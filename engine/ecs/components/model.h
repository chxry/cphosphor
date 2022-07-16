#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "ecs/world.h"
#include "util/ui.h"

typedef struct {
  int entity;
  const char* mesh;
  const char* tex;
  float spec;
} model_t;

extern component_t model;