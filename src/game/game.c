#include "game.h"

ini_t* ini;
conf_t conf;
mtar_t tar;
mtar_header_t h;

void conf_init(const char* path) {
  ini = ini_load(path);
  if (!ini) {
    log_fatal("Could not load config \"%s\".", path);
    exit(-1);
  }
  ini_sget(ini, "gfx", "width", "%i", &conf.width);
  ini_sget(ini, "gfx", "height", "%i", &conf.height);
  ini_sget(ini, "gfx", "fullscreen", "%i", &conf.fullscreen);
  ini_sget(ini, "gfx", "msaa", "%i", &conf.msaa);
  ini_sget(ini, "input", "sens", "%f", &conf.sens);

  log_info("Loaded config \"%s\".", path);
}

void assets_init(const char* path) {
  if (mtar_open(&tar, path, "r")) {
    log_fatal("Could not load \"%s\".", path);
    exit(-1);
  }
  log_info("Loaded \"%s\".", path);
}

asset_t asset_load(const char* path) {
  asset_t asset;
  char base[64] = "res/";
  mtar_find(&tar, strcat(base, path), &h);
  asset.len = h.size;
  asset.data = calloc(1, h.size + 1);
  mtar_read_data(&tar, asset.data, h.size);
  return asset;
}