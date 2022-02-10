#pragma once
#include <stdlib.h>
#include <log.h>
#include <ini.h>

typedef struct {
  int width;
  int height;
} conf_t;

extern conf_t conf;

void conf_init(const char* path);
int conf_get_int(const char* section, const char* key);
