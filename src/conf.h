#pragma once
#include <stdlib.h>
#include <log.h>
#include <ini.h>

void conf_init(const char* path);
int conf_get_int(const char* section, const char* key);
