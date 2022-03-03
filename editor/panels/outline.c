#include "outline.h"

bool outline = true;

void outline_render() {
  if (outline) {
    igSetNextWindowSize((ImVec2){320, 540}, ImGuiCond_Once);
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