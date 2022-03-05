#include "outline.h"

void outline_render() {
  if (outline) {
    if (igBegin(OUTLINE_TITLE, &outline, ImGuiWindowFlags_None)) {
      int i;
      entity_t entity;
      vec_foreach(&world.entities, entity, i) {
        if (igSelectable_Bool(entity.name, selected_entity == i, ImGuiSelectableFlags_None, (ImVec2){0, 0})) {
          selected_entity = i;
        }
      }
    }
    igEnd();
  }
}