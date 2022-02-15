#include "game/game.h"
#include "game/world.h"
#include "gfx/gfx.h"

int main() {
  conf_init("conf.json");
  assets_init("res.tar");
  window_init("flop");
  world_init();

  window_loop();
  window_destroy();
  return 0;
}
