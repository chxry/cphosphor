#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cglm/cglm.h>

#include "engine/core.h"
#include "engine/log.h"
#include "panels/scene.h"
#include "panels/inspector.h"
#include "panels/outline.h"

int main() {
  log_init();
  assets_init("res.tar");
  window_init(1280, 720, false, "editor");
  world_load("test.json");

  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  io->ConfigWindowsMoveFromTitleBarOnly = true;
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 4.0;
  style->FrameRounding = 4.0;
  style->PopupRounding = 4.0;
  asset_t font = asset_load("roboto.ttf");
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, font.data, font.len, 16, NULL, NULL);
  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");

  scene_init();

  bool quit = false;
  while (!quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      }
      scene_processevent(&e);
      ImGui_ImplSDL2_ProcessEvent(&e);
    }
    scene_update();

    glViewport(0, 0, 1280, 720);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    igNewFrame();
    ImGuiViewport* viewport = igGetMainViewport();
    igSetNextWindowPos(viewport->WorkPos, ImGuiCond_Always, (ImVec2){0, 0});
    igSetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
    igSetNextWindowViewport(viewport->ID);
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
    igBegin("##", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
    igPopStyleVar(1);
    if (igBeginMenuBar()) {
      if (igBeginMenu("File", false)) {
        igEndMenu();
      }
      if (igBeginMenu("Windows", true)) {
        igMenuItem_BoolPtr("Inspector", NULL, &inspector, true);
        igMenuItem_BoolPtr("Outline", NULL, &outline, true);
        igMenuItem_BoolPtr("Scene", NULL, &scene, true);
        igEndMenu();
      }
      igEndMenuBar();
    }

    igDockSpace(igGetID_Str("##"), (ImVec2){0, 0}, ImGuiDockNodeFlags_None, NULL);
    igEnd();

    scene_render();
    inspector_render();
    outline_render();

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }

  return 0;
}
