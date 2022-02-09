#include "gfx.h"

int main() {
  window_init("flop", 1280, 720);
  window_loop();

  window_destroy();
  return 0;
}