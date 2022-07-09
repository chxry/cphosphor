#include "outline.h"

void outline_render() {
  if (outline_open) {
    if (igBegin(OUTLINE_TITLE, &outline_open, ImGuiWindowFlags_None)) {
      ImVec2 size;
      igGetWindowSize(&size);
      int i;
      entity_t entity;
      vec_foreach(&world.entities, entity, i) {
        char buf[32];
        sprintf(buf, "%s##%i", entity.name, i);
        if (igSelectable_Bool(buf, selected_entity == entity.id, ImGuiSelectableFlags_None, (ImVec2){0, 0})) {
          selected_entity = entity.id;
        }
      }
      igSeparator();
      if (igButton(ICON_FA_PLUS " Add Entity", (ImVec2){size.x, 0})) {
        char* new = malloc(256);
        strcpy(new, "new");
        entity_t entity = {.name = new, .id = rand(), .pos = GLM_VEC3_ZERO_INIT, .rot = GLM_VEC3_ZERO_INIT, .scale = GLM_VEC3_ONE_INIT};
        vec_push(&world.entities, entity);
      }
    }
    igEnd();
  }
}