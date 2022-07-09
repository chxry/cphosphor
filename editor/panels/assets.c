#include "assets.h"

void assets_render() {
  if (assets_open) {
    if (igBegin(ASSETS_TITLE, &assets_open, ImGuiWindowFlags_None)) {
    }
    igEnd();
  }
}