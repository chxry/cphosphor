#include "game/game.h"
#include "gfx/gfx.h"

int main() {
  assets_init("res.tar");
  conf_init("conf.ini");
  window_init("flop");
  window_loop();

  window_destroy();
  return 0;
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>