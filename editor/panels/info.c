#include "info.h"

void info_render() {
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){20, 20});
  if (info_open) {
    if (igBegin(INFO_TITLE, &info_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
      ui_info(8);
    }
    igEnd();
  }
  igPopStyleVar(1);
}