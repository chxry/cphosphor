#include "about.h"

void about_render() {
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){20, 20});
  if (about) {
    if (igBegin(ABOUT_TITLE, &about, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
      igPushFont(display_font);
      igText("Floppa Engine");
      igPopFont();
      igText("Version " VERSION);
      igSeparator();
      igText("github.com/chxry/tester");
    }
    igEnd();
  }
  igPopStyleVar(1);
}