#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "engine/ecs/world.h"
#include "engine/util/ui.h"
#include "engine/engine.h"
#include "engine/assets.h"
#include "engine/log.h"
#include "panels/scene.h"
#include "panels/inspector.h"
#include "panels/outline.h"
#include "panels/assets.h"
#include "panels/environment.h"
#include "panels/info.h"

int main() {
  log_init();
  engine_init();
  window_init(1280, 720, false, ENGINE_NAME " Editor " ENGINE_VER);
  imgui_init(true);
  audio_init(1.0);
  splash_render("Loading", 1280, 720);
  assets_init("res.zip");
  shaders_init();
  world_load("res/scene.json");

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
    igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0);
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0);
    igBegin("##", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
    igPopStyleVar(3);
    if (igBeginMenuBar()) {
      if (igBeginMenu("File", true)) {
        if (igMenuItem_BoolPtr(ICON_FA_FILE " Save", NULL, NULL, true)) {
          world_write("scene.json");
        }
        igBeginDisabled(true);
        igMenuItem_BoolPtr(ICON_FA_UNDO " Undo", NULL, NULL, true);
        igMenuItem_BoolPtr(ICON_FA_REPEAT " Redo", NULL, NULL, true);
        igEndDisabled();
        if (igMenuItem_BoolPtr(ICON_FA_TIMES " Exit", NULL, NULL, true)) {
          return 0;
        }
        igEndMenu();
      }
      if (igBeginMenu("Windows", true)) {
        igMenuItem_BoolPtr(INSPECTOR_TITLE, NULL, &inspector_open, true);
        igMenuItem_BoolPtr(OUTLINE_TITLE, NULL, &outline_open, true);
        igMenuItem_BoolPtr(SCENE_TITLE, NULL, &scene_open, true);
        igMenuItem_BoolPtr(ASSETS_TITLE, NULL, &assets_open, true);
        igMenuItem_BoolPtr(ENVIRONMENT_TITLE, NULL, &environment_open, true);
        igEndMenu();
      }
      igMenuItem_BoolPtr("Info", NULL, &info_open, true);
      igEndMenuBar();
    }
    igDockSpace(igGetID_Str("##"), (ImVec2){0, 0}, ImGuiDockNodeFlags_None, NULL);
    igEnd();

    scene_render();
    environment_render();
    inspector_render();
    outline_render();
    assets_render();
    info_render();

    // igShowDemoWindow(0);
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }

  return 0;
}
