#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "ecs/world.h"

typedef struct {
  int entity;
  const char* text;
  char text_buf[256];
} text_t;

extern component_t text;