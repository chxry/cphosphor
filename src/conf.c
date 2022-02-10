#include "conf.h"

ini_t* conf;

void conf_init(const char* path) {
  conf = ini_load(path);
}

int conf_get_int(const char* section, const char* key) {
  const char* val = ini_get(conf, section, key);
  if (!val) {
    log_fatal("Could not get %s.%s from config.", section, key);
    exit(-1);
  }
  return atoi(val);
}
