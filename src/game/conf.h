#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <log.h>
#include <ini.h>

typedef struct {
  int width;
  int height;
  int fullscreen;
  int msaa;
  float sens;
} conf_t;

extern conf_t conf;

void conf_init(const char* path);
