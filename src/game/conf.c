#include "conf.h"

ini_t* ini;
conf_t conf;

void conf_init(const char* path) {
  ini = ini_load(path);
  ini_sget(ini, "gfx", "width", "%i", &conf.width);
  ini_sget(ini, "gfx", "height", "%i", &conf.height);
  ini_sget(ini, "gfx", "fullscreen", "%i", &conf.fullscreen);
  ini_sget(ini, "gfx", "msaa", "%i", &conf.msaa);
  ini_sget(ini, "input", "sens", "%f", &conf.sens);

  log_info("Loaded config \"%s\".", path);
}
