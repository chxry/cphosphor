#include "conf.h"
#include "gfx.h"

int main() {
  conf_init("conf.ini");
  window_init("flop");
  window_loop();

  window_destroy();
  return 0;
}
