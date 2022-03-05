#include "inspector.h"

void inspector_render() {
  if (inspector) {
    if (igBegin(INSPECTOR_TITLE, &inspector, ImGuiWindowFlags_None)) {
      if (selected_entity >= 0) {
        if (igCollapsingHeader_TreeNodeFlags(ICON_FA_ARROWS " Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
          igDragFloat3("Position", world.entities.data[selected_entity].pos, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
          igDragFloat3("Rotation", world.entities.data[selected_entity].rot, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
          igDragFloat3("Scale", world.entities.data[selected_entity].scale, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
        }
        if (igCollapsingHeader_TreeNodeFlags(ICON_FA_CUBE " Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
        }
        if (igCollapsingHeader_TreeNodeFlags(ICON_FA_CUBES " Colliders", ImGuiTreeNodeFlags_DefaultOpen)) {
        }
      } else {
        igText(ICON_FA_EXCLAMATION_TRIANGLE " No entity selected.");
      }
    }
    igEnd();
  }
}