#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#include "engine/core.h"
#include "engine/gfx.h"
#include "engine/log.h"

int main() {
  log_init();
  assets_init("res.tar");
  window_init(1280, 720, false, "flop");

  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  io->IniFilename = NULL;
  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 4.0;
  style->FrameRounding = 4.0;
  style->PopupRounding = 4.0;
  asset_t font = asset_load("roboto.ttf");
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, font.data, font.len, 16, NULL, NULL);
  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");

  bool quit = false;
  while (!quit) {
    SDL_GL_SwapWindow(window);
  }

  return 0;
}
