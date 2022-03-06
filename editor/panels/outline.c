#include "outline.h"

void outline_render() {
  if (outline) {
    if (igBegin(OUTLINE_TITLE, &outline, ImGuiWindowFlags_None)) {
      ImVec2 size;
      igGetWindowSize(&size);
      int i;
      entity_t entity;
      vec_foreach(&world.entities, entity, i) {
        char buf[32];
        sprintf(buf, "%s##%i", entity.name, i);
        if (igSelectable_Bool(buf, selected_entity == i, ImGuiSelectableFlags_None, (ImVec2){0, 0})) {
          set_selected_entity(i);
        }
      }
      igSeparator();
      if (igButton(ICON_FA_PLUS " Add Entity", (ImVec2){size.x, 0})) {
        entity_t entity = {.name = "new", .pos = GLM_VEC3_ZERO_INIT, .rot = GLM_VEC3_ZERO_INIT, .scale = GLM_VEC3_ONE_INIT, .model = NULL, .collider = NULL};
        vec_push(&world.entities, entity);
      }
    }
    igEnd();
  }
}