#include "conf.h"
#include "gfx.h"

int main() {
  conf_init("conf.ini");
  window_init("flop", conf_get_int("gfx", "width"), conf_get_int("gfx", "height"));
  window_loop();

  window_destroy();
  return 0;
}
