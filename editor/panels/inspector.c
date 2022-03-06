#include "inspector.h"

void inspector_render() {
  if (inspector) {
    if (igBegin(INSPECTOR_TITLE, &inspector, ImGuiWindowFlags_None)) {
      ImVec2 size;
      igGetWindowSize(&size);
      if (selected_entity >= 0) {
        entity_t* entity = &world.entities.data[selected_entity];
        if (igCollapsingHeader_BoolPtr(TRANSFORM_COMPONENT, NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
          igDragFloat3("Position", entity->pos, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
          igDragFloat3("Rotation", entity->rot, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
          igDragFloat3("Scale", entity->scale, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
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
          igDragFloat3("Min", entity->collider->min, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
          igDragFloat3("Max", entity->collider->max, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
        }
        if (!has_collider && entity->collider) {
          free(entity->collider);
          entity->collider = NULL;
        }
        igSeparator();
        if (igButton(ICON_FA_PLUS " Add component", (ImVec2){size.x, 0})) {
          igOpenPopup_Str("addcomponent", ImGuiPopupFlags_None);
        }
        igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
        if (igBeginPopup("addcomponent", ImGuiWindowFlags_None)) {
          if (!has_model) {
            if (igButton(MODEL_COMPONENT, (ImVec2){size.x, 0})) {
              entity->model = malloc(sizeof(model_t));
              entity->model->mesh = "mesh/cube.obj";
              entity->model->tex = "tex/flop.jpg";
              set_selected_entity(selected_entity);
            }
          }
          if (!has_collider) {
            if (igButton(COLLIDER_COMPONENT, (ImVec2){size.x, 0})) {
              entity->collider = malloc(sizeof(collider_t));
              glm_vec3_copy(GLM_VEC3_ZERO, entity->collider->min);
              glm_vec3_copy(GLM_VEC3_ONE, entity->collider->max);
            }
          }
          igEndPopup();
        }
        igPopStyleVar(1);
      } else {
        igText(ICON_FA_EXCLAMATION_TRIANGLE " No entity selected.");
      }
    }
    igEnd();
  }
}