#pragma once
#include <stdlib.h>
#include <string.h>
#include <ini.h>
#include <microtar.h>
#include <log.h>

typedef struct {
  int width;
  int height;
  int fullscreen;
  int msaa;
  float sens;
} conf_t;

typedef struct {
  void* data;
  int len;
} asset_t;

extern conf_t conf;

void conf_init(const char* path);
void assets_init(const char* path);
asset_t asset_load(const char* path);
