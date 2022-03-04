#include "outline.h"

void outline_render() {
  if (outline) {
    if (igBegin("Outline", &outline, ImGuiWindowFlags_None)) {
      int i;
      gameobj_t gameobj;
      vec_foreach(&gameobjs, gameobj, i) {
        if (igSelectable_Bool(gameobj.name, selected_entity == i, ImGuiSelectableFlags_None, (ImVec2){0, 0})) {
          selected_entity = i;
        }
      }
    }
    igEnd();
  }
}