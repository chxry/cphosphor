#include "conf.h"

ini_t* ini;
conf_t conf;

void conf_init(const char* path) {
  ini = ini_load(path);
  conf.width = conf_get_int("gfx", "width");
  conf.height = conf_get_int("gfx", "height");
  log_info("Loaded config \"%s\".", path);
}

int conf_get_int(const char* section, const char* key) {
  const char* val = ini_get(ini, section, key);
  if (!val) {
    log_fatal("Could not get %s.%s from config.", section, key);
    exit(-1);
  }
  return atoi(val);
}
