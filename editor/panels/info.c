#include "info.h"

void info_render() {
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){20, 20});
  if (info) {
    if (igBegin(INFO_TITLE, &info, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
      ImVec2 size;
      igGetContentRegionAvail(&size);
      igPushFont(display_font);
      igText("Floppa Engine");
      igPopFont();
      igText("github.com/chxry/tester");
      igSameLine(size.x - 8, 0);
      igText("v" VERSION);
      igSeparator();
      if (igTreeNode_Str("Debug Info")) {
        ImGuiIO io = *igGetIO();
        igText("Entities: %i", world.entities.length);
        if (selected_entity >= 0) {
          igText("Selected entity: %i", selected_entity);
        } else {
          igText("Selected entity: None");
        }
        igText("%.1ffps %.1fms", io.Framerate, io.DeltaTime * 1000);
        igTreePop();
      }
    }
    igEnd();
  }
  igPopStyleVar(1);
}