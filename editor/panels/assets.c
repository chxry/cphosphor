#include "assets.h"

void assets_render() {
  if (assets) {
    if (igBegin(ASSETS_TITLE, &assets, ImGuiWindowFlags_None)) {
    }
    igEnd();
  }
}