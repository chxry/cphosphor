#include "inspector.h"

void inspector_render() {
  if (inspector) {
    if (igBegin(INSPECTOR_TITLE, &inspector, ImGuiWindowFlags_None)) {
      ImVec2 size;
      igGetContentRegionAvail(&size);
      if (selected_entity >= 0) {
        entity_t* entity = &world.entities.data[selected_entity];
        igSetNextItemWidth(size.x - 24);
        igInputText("##", entity->name, 256, ImGuiInputTextFlags_None, NULL, NULL);
        igSameLine(0, 4);
        if (igButton(ICON_FA_TRASH, (ImVec2){20, 20})) {
          vec_splice(&world.entities, selected_entity, 1);
          selected_entity = -1;
          // free
          // with current system each entity before would have to have its components index shifted - switch to random entity ids.
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
          if (model->c.entity == selected_entity) {
            bool o = true;
            sprintf(buf, MODEL_COMPONENT "##%i", i);
            if (igCollapsingHeader_BoolPtr(buf, &o, ImGuiTreeNodeFlags_DefaultOpen)) {
              if (igInputText("Mesh", model->mesh_buf, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
                model->mesh = realloc(model->mesh, strlen(model->mesh_buf));
                strcpy(model->mesh, model->mesh_buf);
              }
              if (igInputText("Texture", model->tex_buf, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
                model->tex = realloc(model->tex, strlen(model->tex_buf));
                strcpy(model->tex, model->tex_buf);
              }
            }
            if (!o) {
              vec_splice(&world.models, i, 1);
              // free
            }
          }
        }
        collider_t* collider;
        vec_foreach_ptr(&world.colliders, collider, i) {
          if (collider->c.entity == selected_entity) {
            bool o = true;
            sprintf(buf, COLLIDER_COMPONENT "##%i", i);
            if (igCollapsingHeader_BoolPtr(buf, &o, ImGuiTreeNodeFlags_DefaultOpen)) {
              sprintf(buf, "Min##collider%i", i);
              igDragFloat3(buf, collider->b.min, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
              sprintf(buf, "Max##collider%i", i);
              igDragFloat3(buf, collider->b.max, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
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
            model_t model;
            model.c.entity = selected_entity;
            model.mesh = malloc(14);
            strcpy(model.mesh, "mesh/cube.obj");
            strcpy(model.mesh_buf, "mesh/cube.obj");
            model.tex = malloc(13);
            strcpy(model.tex, "tex/flop.jpg");
            strcpy(model.tex_buf, "tex/flop.jpg");
            vec_push(&world.models, model);
          }
          if (igSelectable_Bool(COLLIDER_COMPONENT, false, ImGuiSelectableFlags_None, (ImVec2){size.x, 0})) {
            collider_t collider;
            collider.c.entity = selected_entity;
            glm_vec3_copy(GLM_VEC3_ZERO, collider.b.min);
            glm_vec3_copy(GLM_VEC3_ONE, collider.b.max);
            vec_push(&world.colliders, collider);
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