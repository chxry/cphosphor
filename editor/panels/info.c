#include "info.h"

char* cpu_name;

void info_init() {
  FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
  if (cpuinfo == NULL) {
    log_error("Could not open /proc/cpuinfo.");
    return;
  }
  size_t i = 0;
  while (getline(&cpu_name, &i, cpuinfo) > 0) {
    if (strstr(cpu_name, "model name")) {
      strtok(cpu_name, ":");
      cpu_name = strtok(NULL, ":") + 1;
      break;
    }
  }
  fclose(cpuinfo);
}

void info_render() {
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){20, 20});
  if (info_open) {
    if (igBegin(INFO_TITLE, &info_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
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
        if (igTreeNode_Str("World")) {
          igText("Entities: %i", world.entities.length);
          if (selected_entity >= 0) {
            igText("Selected entity: %i", selected_entity);
          } else {
            igText("Selected entity: None");
          }
          igTreePop();
        }
        if (igTreeNode_Str("Peformance")) {
          ImGuiIO io = *igGetIO();
          igText("%.1ffps %.1fms", io.Framerate, io.DeltaTime * 1000);
          igTreePop();
        }
        if (igTreeNode_Str("System")) {
          igText("%s", cpu_name ? cpu_name : "Unknown CPU.");
          igText("%s", glGetString(GL_RENDERER));
          igTreePop();
        }

        igTreePop();
      }
    }
    igEnd();
  }
  igPopStyleVar(1);
}