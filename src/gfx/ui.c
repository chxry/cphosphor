#include "ui.h"

bool debug_overlay = true;
bool options = true;
bool demo = true;

conf_t options_conf;

void ui_init(SDL_Window* window, SDL_GLContext* ctx) {
  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  io->IniFilename = NULL;
  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 4.0;
  style->FrameRounding = 4.0;
  style->PopupRounding = 4.0;
  ImVec4* colors = style->Colors;
  colors[ImGuiCol_FrameBg] = LILAC;
  colors[ImGuiCol_FrameBgHovered] = LILAC2;
  colors[ImGuiCol_FrameBgActive] = LILAC3;
  colors[ImGuiCol_TitleBgActive] = LILAC;
  colors[ImGuiCol_CheckMark] = (ImVec4){1.0, 1.0, 1.0, 1.0};
  colors[ImGuiCol_SliderGrab] = LILAC;
  colors[ImGuiCol_SliderGrabActive] = LILAC3;
  colors[ImGuiCol_Button] = LILAC;
  colors[ImGuiCol_ButtonHovered] = LILAC2;
  colors[ImGuiCol_ButtonActive] = LILAC3;
  colors[ImGuiCol_Header] = LILAC;
  colors[ImGuiCol_HeaderHovered] = LILAC2;
  colors[ImGuiCol_HeaderActive] = LILAC3;
  colors[ImGuiCol_SeparatorHovered] = LILAC2;
  colors[ImGuiCol_SeparatorActive] = LILAC3;
  colors[ImGuiCol_ResizeGrip] = LILAC;
  colors[ImGuiCol_ResizeGripHovered] = LILAC2;
  colors[ImGuiCol_ResizeGripActive] = LILAC3;
  colors[ImGuiCol_Tab] = LILAC;
  colors[ImGuiCol_TabHovered] = LILAC2;
  colors[ImGuiCol_TabActive] = LILAC3;
  colors[ImGuiCol_NavHighlight] = LILAC;
  colors[ImGuiCol_TextSelectedBg] = LILAC3;
  asset_t font = asset_load("jetbrains mono.ttf");
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, font.data, font.len, 16, NULL, NULL);
  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");
  options_conf = conf;
}

void ui_processevent(SDL_Event* e) {
  ImGui_ImplSDL2_ProcessEvent(e);
  if (e->type == SDL_KEYDOWN) {
    if (e->key.keysym.sym == SDLK_BACKQUOTE) {
      debug_overlay = !debug_overlay;
    }
  }
}

void ui_render(SDL_Window* window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  igNewFrame();
  ImGuiIO io = *igGetIO();

  if (debug_overlay) {
    igSetNextWindowPos((ImVec2){16, 16}, ImGuiCond_Always, VEC2_ZERO);
    igSetNextWindowBgAlpha(0.5);
    if (igBegin("Debug overlay", &debug_overlay, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize)) {
      igText("%.1fms", io.DeltaTime * 1000);
      igText("%.1ffps", io.Framerate);
      igSeparator();
      igText("\'esc\' toggle cursor");
      igText("\'`\' toggle debug");
    }
    igEnd();
  }

  if (options) {
    igSetNextWindowSize((ImVec2){480, 320}, ImGuiCond_Once);
    if (igBegin("Options", &options, ImGuiWindowFlags_None)) {
      bool open = true;
      igBeginChild_Str("optionscontent", (ImVec2){0, -igGetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_None);
      if (igBeginTabBar("options", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (igBeginTabItem("Video", &open, ImGuiTabItemFlags_NoCloseButton)) {
          igInputInt2("Resolution", (int*)&options_conf, ImGuiInputTextFlags_None);
          igCheckbox("Fullscreen", &options_conf.fullscreen);
          igCombo_Str("MSAA", &options_conf.msaa, "None\0x2\0x4\0x8\0x16", 0);
          igSliderFloat("FOV", &options_conf.fov, 30, 90, "%.1f", ImGuiSliderFlags_None);
          igEndTabItem();
        }
        if (igBeginTabItem("Input", &open, ImGuiTabItemFlags_NoCloseButton)) {
          igSliderFloat("Sensitivity", &options_conf.sens, 0.02, 0.4, "%.2f", ImGuiSliderFlags_None);
          igEndTabItem();
        }
        igEndTabBar();
      }
      igEndChild();
      if (igButton("Apply", VEC2_ZERO)) {
        conf = options_conf;
        SDL_SetWindowSize(window, conf.width, conf.height);
        SDL_SetWindowFullscreen(window, conf.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        conf_write("conf.json");
      }
      igSameLine(0, 4);
      if (igButton("Reset", VEC2_ZERO)) {
        options_conf = conf;
      }
    }
    igEnd();
  }

  if (demo) {
    igShowDemoWindow(&demo);
  }

  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}