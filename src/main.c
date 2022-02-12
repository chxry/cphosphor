#include "conf.h"
#include "gfx.h"

int main() {
  conf_init("conf.ini");
  window_init("flop", conf.width, conf.height, conf.fullscreen);
  window_loop();

  window_destroy();
  return 0;
}
