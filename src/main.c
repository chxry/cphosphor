#include "game/game.h"
#include "game/world.h"
#include "gfx/gfx.h"

int main() {
  assets_init("res.tar");
  conf_init("conf.ini");
  window_init("flop");
  world_init();

  window_loop();
  window_destroy();
  return 0;
}
