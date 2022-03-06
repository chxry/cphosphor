#include "inspector.h"

void inspector_render() {
  if (inspector) {
    if (igBegin(INSPECTOR_TITLE, &inspector, ImGuiWindowFlags_None)) {
      ImVec2 size;
      igGetContentRegionAvail(&size);
      if (selected_entity >= 0) {
        entity_t* entity = &world.entities.data[selected_entity];
        igSetNextItemWidth(size.x);
        igInputText("##", entity->name, 256, ImGuiInputTextFlags_None, NULL, NULL);
        if (igCollapsingHeader_BoolPtr(TRANSFORM_COMPONENT, NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
          igDragFloat3("Position", entity->pos, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Rotation", entity->rot, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Scale", entity->scale, 0.01, 0, 0, "%.5g", ImGuiSliderFlags_None);
        }
        bool has_model = entity->model;
        if (igCollapsingHeader_BoolPtr(MODEL_COMPONENT, &has_model, ImGuiTreeNodeFlags_DefaultOpen)) {
          if (igInputText("Mesh", mesh_mesh_buf, sizeof(mesh_mesh_buf), ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
            char* new = malloc(strlen(mesh_mesh_buf));
            free((void*)entity->model->mesh);
            entity->model->mesh = strcpy(new, mesh_mesh_buf);
          }
          if (igInputText("Texture", mesh_tex_buf, sizeof(mesh_tex_buf), ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
            char* new = malloc(strlen(mesh_tex_buf));
            free((void*)entity->model->tex);
            entity->model->tex = strcpy(new, mesh_tex_buf);
          }
        }
        if (!has_model && entity->model) {
          free(entity->model);
          entity->model = NULL;
        }
        bool has_collider = entity->collider;
        if (igCollapsingHeader_BoolPtr(COLLIDER_COMPONENT, &has_collider, ImGuiTreeNodeFlags_DefaultOpen)) {
          igDragFloat3("Min", entity->collider->min, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
          igDragFloat3("Max", entity->collider->max, 0.1, 0, 0, "%.5g", ImGuiSliderFlags_None);
        }
        if (!has_collider && entity->collider) {
          free(entity->collider);
          entity->collider = NULL;
        }
        igSeparator();
        if (igButton(ICON_FA_PLUS " Add component", (ImVec2){size.x, 0})) {
          igOpenPopup_Str("addcomponent", ImGuiPopupFlags_None);
        }
        if (igBeginPopup("addcomponent", ImGuiWindowFlags_None)) {
          igBeginDisabled(has_model);
          if (igSelectable_Bool(MODEL_COMPONENT, false, ImGuiSelectableFlags_None, (ImVec2){size.x, 0})) {
            entity->model = malloc(sizeof(model_t));
            entity->model->mesh = malloc(14);
            strcpy((char*)entity->model->mesh, "mesh/cube.obj");
            entity->model->tex = malloc(13);
            strcpy((char*)entity->model->tex, "tex/flop.jpg");
            set_selected_entity(selected_entity);
          }
          igEndDisabled();
          igBeginDisabled(has_collider);
          if (igSelectable_Bool(COLLIDER_COMPONENT, false, ImGuiSelectableFlags_None, (ImVec2){size.x, 0})) {
            entity->collider = malloc(sizeof(collider_t));
            glm_vec3_copy(GLM_VEC3_ZERO, entity->collider->min);
            glm_vec3_copy(GLM_VEC3_ONE, entity->collider->max);
          }
          igEndDisabled();
          igEndPopup();
        }
      } else {
        igText(ICON_FA_EXCLAMATION_TRIANGLE " No entity selected.");
      }
    }
    igEnd();
  }
}