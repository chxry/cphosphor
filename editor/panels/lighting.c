#include "lighting.h"

void lighting_render() {
  if (lighting) {
    if (igBegin(LIGHTING_TITLE, &lighting, ImGuiWindowFlags_None)) {
      igDragFloat("Ambient", &world.light_ambient, 0.01, 0, 10, "%.2f", ImGuiSliderFlags_None);
      igDragFloat3("Direction", world.light_dir, 0.1, 0, 0, "%.1f", ImGuiSliderFlags_None);
      igColorEdit3("Color", world.light_color, ImGuiColorEditFlags_NoInputs);
    }
    igEnd();
  }
}