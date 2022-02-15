#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <microtar.h>
#include <parson/parson.h>
#include <log.h>

typedef struct {
  int width;
  int height;
  bool fullscreen;
  int msaa;
  float fov;
  float sens;
} conf_t;

typedef struct {
  void* data;
  int len;
} asset_t;

extern conf_t conf;

void conf_init(const char* path);
void conf_write(const char* path);
void assets_init(const char* path);
asset_t asset_load(const char* path);
