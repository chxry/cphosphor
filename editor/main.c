#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cglm/cglm.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "engine/core.h"
#include "engine/world.h"
#include "engine/log.h"
#include "panels/scene.h"
#include "panels/inspector.h"
#include "panels/outline.h"
#include "panels/assets.h"
#include "panels/lighting.h"

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
  ImVec4* colors = style->Colors;
  colors[ImGuiCol_Text] = (ImVec4){1.00, 1.00, 1.00, 1.00};
  colors[ImGuiCol_TextDisabled] = (ImVec4){0.50, 0.50, 0.50, 1.00};
  colors[ImGuiCol_WindowBg] = (ImVec4){0.10, 0.10, 0.10, 1.00};
  colors[ImGuiCol_ChildBg] = (ImVec4){0.00, 0.00, 0.00, 0.00};
  colors[ImGuiCol_PopupBg] = (ImVec4){0.19, 0.19, 0.19, 0.92};
  colors[ImGuiCol_Border] = (ImVec4){0.19, 0.19, 0.19, 0.29};
  colors[ImGuiCol_BorderShadow] = (ImVec4){0.00, 0.00, 0.00, 0.24};
  colors[ImGuiCol_FrameBg] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_FrameBgHovered] = (ImVec4){0.19, 0.19, 0.19, 0.54};
  colors[ImGuiCol_FrameBgActive] = (ImVec4){0.20, 0.22, 0.23, 1.00};
  colors[ImGuiCol_TitleBg] = (ImVec4){0.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_TitleBgActive] = (ImVec4){0.06, 0.06, 0.06, 1.00};
  colors[ImGuiCol_TitleBgCollapsed] = (ImVec4){0.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_MenuBarBg] = (ImVec4){0.14, 0.14, 0.14, 1.00};
  colors[ImGuiCol_ScrollbarBg] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_ScrollbarGrab] = (ImVec4){0.34, 0.34, 0.34, 0.54};
  colors[ImGuiCol_ScrollbarGrabHovered] = (ImVec4){0.40, 0.40, 0.40, 0.54};
  colors[ImGuiCol_ScrollbarGrabActive] = (ImVec4){0.56, 0.56, 0.56, 0.54};
  colors[ImGuiCol_CheckMark] = (ImVec4){0.33, 0.67, 0.86, 1.00};
  colors[ImGuiCol_SliderGrab] = (ImVec4){0.34, 0.34, 0.34, 0.54};
  colors[ImGuiCol_SliderGrabActive] = (ImVec4){0.56, 0.56, 0.56, 0.54};
  colors[ImGuiCol_Button] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_ButtonHovered] = (ImVec4){0.19, 0.19, 0.19, 0.54};
  colors[ImGuiCol_ButtonActive] = (ImVec4){0.20, 0.22, 0.23, 1.00};
  colors[ImGuiCol_Header] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_HeaderHovered] = (ImVec4){0.00, 0.00, 0.00, 0.36};
  colors[ImGuiCol_HeaderActive] = (ImVec4){0.20, 0.22, 0.23, 0.33};
  colors[ImGuiCol_Separator] = (ImVec4){0.28, 0.28, 0.28, 0.29};
  colors[ImGuiCol_SeparatorHovered] = (ImVec4){0.44, 0.44, 0.44, 0.29};
  colors[ImGuiCol_SeparatorActive] = (ImVec4){0.40, 0.44, 0.47, 1.00};
  colors[ImGuiCol_ResizeGrip] = (ImVec4){0.28, 0.28, 0.28, 0.29};
  colors[ImGuiCol_ResizeGripHovered] = (ImVec4){0.44, 0.44, 0.44, 0.29};
  colors[ImGuiCol_ResizeGripActive] = (ImVec4){0.40, 0.44, 0.47, 1.00};
  colors[ImGuiCol_Tab] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TabHovered] = (ImVec4){0.14, 0.14, 0.14, 1.00};
  colors[ImGuiCol_TabActive] = (ImVec4){0.20, 0.20, 0.20, 0.36};
  colors[ImGuiCol_TabUnfocused] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TabUnfocusedActive] = (ImVec4){0.14, 0.14, 0.14, 1.00};
  colors[ImGuiCol_DockingPreview] = (ImVec4){0.33, 0.67, 0.86, 1.00};
  colors[ImGuiCol_DockingEmptyBg] = (ImVec4){0.10, 0.10, 0.10, 1.00};
  colors[ImGuiCol_PlotLines] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_PlotLinesHovered] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_PlotHistogram] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_PlotHistogramHovered] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_TableHeaderBg] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TableBorderStrong] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TableBorderLight] = (ImVec4){0.28, 0.28, 0.28, 0.29};
  colors[ImGuiCol_TableRowBg] = (ImVec4){0.00, 0.00, 0.00, 0.00};
  colors[ImGuiCol_TableRowBgAlt] = (ImVec4){1.00, 1.00, 1.00, 0.06};
  colors[ImGuiCol_TextSelectedBg] = (ImVec4){0.20, 0.22, 0.23, 1.00};
  colors[ImGuiCol_DragDropTarget] = (ImVec4){0.33, 0.67, 0.86, 1.00};
  colors[ImGuiCol_NavHighlight] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_NavWindowingHighlight] = (ImVec4){0.19, 0.19, 0.19, 0.54};
  colors[ImGuiCol_NavWindowingDimBg] = (ImVec4){1.00, 0.00, 0.00, 0.20};
  colors[ImGuiCol_ModalWindowDimBg] = (ImVec4){1.00, 0.00, 0.00, 0.35};
  style->WindowRounding = 4.0;
  style->PopupRounding = 4.0;
  style->FrameRounding = 2.0;

  asset_t roboto = asset_load("roboto.ttf");
  asset_t fa = asset_load("fontawesome.ttf");
  ImFontConfig config = *ImFontConfig_ImFontConfig();
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, roboto.data, roboto.len, 16, &config, NULL);
  config.MergeMode = true;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, fa.data, fa.len, 12, &config, icon_ranges);
  ImFontAtlas_Build(io->Fonts);
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
    igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0);
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0);
    igBegin("##", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
    igPopStyleVar(3);
    if (igBeginMenuBar()) {
      if (igBeginMenu("File", true)) {
        if (igMenuItem_BoolPtr(ICON_FA_FILE " Save", NULL, NULL, true)) {
          world_write("test.json");
        }
        igEndMenu();
      }
      if (igBeginMenu("Windows", true)) {
        igMenuItem_BoolPtr(INSPECTOR_TITLE, NULL, &inspector, true);
        igMenuItem_BoolPtr(OUTLINE_TITLE, NULL, &outline, true);
        igMenuItem_BoolPtr(SCENE_TITLE, NULL, &scene, true);
        igMenuItem_BoolPtr(ASSETS_TITLE, NULL, &assets, true);
        igMenuItem_BoolPtr(LIGHTING_TITLE, NULL, &lighting, true);
        igEndMenu();
      }
      igEndMenuBar();
    }
    igDockSpace(igGetID_Str("##"), (ImVec2){0, 0}, ImGuiDockNodeFlags_None, NULL);
    igEnd();

    scene_render();
    lighting_render();
    inspector_render();
    outline_render();
    assets_render();

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }

  return 0;
}
