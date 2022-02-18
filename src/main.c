#include "game/game.h"
#include "game/world.h"
#include "gfx/gfx.h"
#include "log.h"

int main() {
  log_init();
  conf_init("conf.json");
  assets_init("res.tar");
  lua_init();
  window_init("flop");
  world_init();

  window_loop();
  window_destroy();
  return 0;
}
