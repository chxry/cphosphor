#include "inspector.h"

void inspector_panel(component_t component) {
  char buf[32];
  int i;
  void* c;
  vec_foreach(get_components(component.name), c, i) {
    if (*((int*)c) == selected_entity) {
      bool o = true;
      sprintf(buf, "%s##%i", component.inspector_label, i);
      if (igCollapsingHeader_BoolPtr(buf, &o, ImGuiTreeNodeFlags_DefaultOpen)) {
        component.inspector(c);
      }
      if (!o) {
        // delete
      }
    }
  }
}

void inspector_render() {
  if (inspector) {
    if (igBegin(INSPECTOR_TITLE, &inspector, ImGuiWindowFlags_None)) {
      ImVec2 size;
      igGetContentRegionAvail(&size);
      if (selected_entity >= 0) {
        entity_t* entity = get_entity(selected_entity);
        igSetNextItemWidth(size.x - 24);
        igInputText("##", entity->name, 256, ImGuiInputTextFlags_None, NULL, NULL);
        igSameLine(0, 4);
        if (igButton(ICON_FA_TRASH, (ImVec2){20, 20})) {
          entity_delete(selected_entity);
          selected_entity = -1;
        }
        if (igCollapsingHeader_BoolPtr(TRANSFORM_COMPONENT, NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
          igDragFloat3("Position", entity->pos, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Rotation", entity->rot, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Scale", entity->scale, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
        }
        component_iter(inspector_panel);
        igSeparator();
        if (igButton(ICON_FA_PLUS " Add component", (ImVec2){size.x, 0})) {
          igOpenPopup_Str("addcomponent", ImGuiPopupFlags_None);
        }
        if (igBeginPopup("addcomponent", ImGuiWindowFlags_None)) {
          igEndPopup();
        }
      } else {
        igText(ICON_FA_EXCLAMATION_TRIANGLE " No entity selected.");
      }
    }
    igEnd();
  }
}