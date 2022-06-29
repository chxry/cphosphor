#include "inspector.h"

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
          vec_splice(&world.entities, selected_entity, 1); // WONT WORK WITH IDS
          selected_entity = -1;
          // free
        }
        if (igCollapsingHeader_BoolPtr(TRANSFORM_COMPONENT, NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
          igDragFloat3("Position", entity->pos, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Rotation", entity->rot, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Scale", entity->scale, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
        }
        char buf[32];
        int i;
        model_t* model;
        vec_foreach_ptr(&world.models, model, i) {
          if (model->entity == selected_entity) {
            bool o = true;
            sprintf(buf, MODEL_COMPONENT "##%i", i);
            if (igCollapsingHeader_BoolPtr(buf, &o, ImGuiTreeNodeFlags_DefaultOpen)) {
              model_inspector(model);
            }
            if (!o) {
              vec_splice(&world.models, i, 1);
              // free
            }
          }
        }
        collider_t* collider;
        vec_foreach_ptr(&world.colliders, collider, i) {
          if (collider->entity == selected_entity) {
            bool o = true;
            sprintf(buf, COLLIDER_COMPONENT "##%i", i);
            if (igCollapsingHeader_BoolPtr(buf, &o, ImGuiTreeNodeFlags_DefaultOpen)) {
              collider_inspector(collider, i);
            }
            if (!o) {
              vec_splice(&world.colliders, i, 1);
              // free
            }
          }
        }
        igSeparator();
        if (igButton(ICON_FA_PLUS " Add component", (ImVec2){size.x, 0})) {
          igOpenPopup_Str("addcomponent", ImGuiPopupFlags_None);
        }
        if (igBeginPopup("addcomponent", ImGuiWindowFlags_None)) {
          if (igSelectable_Bool(MODEL_COMPONENT, false, ImGuiSelectableFlags_None, (ImVec2){size.x, 0})) {
            vec_push(&world.models, model_create(selected_entity));
          }
          if (igSelectable_Bool(COLLIDER_COMPONENT, false, ImGuiSelectableFlags_None, (ImVec2){size.x, 0})) {
            vec_push(&world.colliders, collider_create(selected_entity));
          }
          igEndPopup();
        }
      } else {
        igText(ICON_FA_EXCLAMATION_TRIANGLE " No entity selected.");
      }
    }
    igEnd();
  }
}