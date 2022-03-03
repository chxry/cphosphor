#include "inspector.h"

bool inspector = true;

void inspector_render() {
  if (inspector) {
    igSetNextWindowSize((ImVec2){320, 540}, ImGuiCond_Once);
    if (igBegin("Inspector", &inspector, ImGuiWindowFlags_None)) {
      if (selected_entity >= 0) {
        igText("Transform:");
        igDragFloat3("Position", &gameobjs.data[selected_entity].pos, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
        igDragFloat3("Rotation", &gameobjs.data[selected_entity].rot, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
        igDragFloat3("Scale", &gameobjs.data[selected_entity].scale, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
        igSeparator();
      } else {
        igText("no entity selected");
      }
    }
    igEnd();
  }
}