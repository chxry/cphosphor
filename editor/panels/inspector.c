#include "inspector.h"

void inspector_panel(component_t* component, char* name) {
  char buf[32];
  int i;
  void* c;
  vec_foreach(&component->components, c, i) {
    if (*((int*)c) == selected_entity) {
      bool o = true;
      sprintf(buf, "%s##%i", component->inspector_label, i);
      if (igCollapsingHeader_BoolPtr(buf, &o, ImGuiTreeNodeFlags_DefaultOpen)) {
        component->inspector(c, i);
      }
      if (!o) {
        vec_splice(&component->components, i, 1);
      }
    }
  }
}

ImVec2 size;
void component_create(component_t* component, char* name) {
  if (igSelectable_Bool(component->inspector_label, false, ImGuiSelectableFlags_None, (ImVec2){size.x, 0})) {
    vec_push(&component->components, component->create(selected_entity));
  }
}

void inspector_render() {
  if (inspector_open) {
    if (igBegin(INSPECTOR_TITLE, &inspector_open, ImGuiWindowFlags_None)) {
      igGetContentRegionAvail(&size);
      if (selected_entity >= 0) {
        entity_t* entity = get_entity(selected_entity);
        igSetNextItemWidth(size.x - 24);
        if (igInputText("##", name_buf, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
          entity->name = realloc(entity->name, strlen(name_buf));
          strcpy(entity->name, name_buf);
        }
        igSameLine(0, 4);
        if (igButton(ICON_FA_TRASH, (ImVec2){20, 20})) {
          entity_delete(selected_entity);
          set_selected_entity(-1);
        }
        if (igCollapsingHeader_BoolPtr(ICON_FA_ARROWS " Transform", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
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
          component_iter(component_create);
          igEndPopup();
        }
      } else {
        igText(ICON_FA_EXCLAMATION_TRIANGLE " No entity selected.");
      }
    }
    igEnd();
  }
}