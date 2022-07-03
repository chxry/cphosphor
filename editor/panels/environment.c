#include "environment.h"

void environment_render() {
  if (environment) {
    if (igBegin(ENVIRONMENT_TITLE, &environment, ImGuiWindowFlags_None)) {
      igText(ICON_FA_LIGHTBULB_O " Lighting:");
      igDragFloat("Ambient", &world.light_ambient, 0.01, 0, 10, "%.2f", ImGuiSliderFlags_None);
      igDragFloat("Diffuse", &world.light_diffuse, 0.01, 0, 10, "%.2f", ImGuiSliderFlags_None);
      igDragFloat3("Direction", world.light_dir, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
      igColorEdit3("Color", world.light_color, ImGuiColorEditFlags_NoInputs);
      igText(ICON_FA_SUN_O " Sky:");
      igCombo_Str("Sky Mode", &world.sky_mode, "Skybox\0Atmosphere\0", 0);
    }
    igEnd();
  }
}