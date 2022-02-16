#include "ui.h"

bool mouse_captured;
bool debug_overlay = true;
bool options = false;
bool demo = false;

conf_t options_conf;

void ui_init(SDL_Window* window, SDL_GLContext* ctx) {
  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  io->IniFilename = NULL;
  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 4.0;
  style->FrameRounding = 4.0;
  style->PopupRounding = 4.0;
  asset_t font = asset_load("jetbrains mono.ttf");
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, font.data, font.len, 16, NULL, NULL);
  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");
  options_conf = conf;
}

void ui_processevent(SDL_Event* e) {
  ImGui_ImplSDL2_ProcessEvent(e);
  if (e->type == SDL_KEYDOWN) {
    SDL_Scancode code = e->key.keysym.scancode;
    if (code == conf.binds[KEYBIND_DEBUG]) { // USE A SWITCH
      debug_overlay = !debug_overlay;
    } else if (code == conf.binds[KEYBIND_MENU]) {
      options = !options;
      SDL_SetRelativeMouseMode(!options);
      mouse_captured = !options;
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
      igText("%.1f %.1f %.1f", player_pos[0], player_pos[1], player_pos[2]);
    }
    igEnd();
  }

  if (options) {
    igSetNextWindowSize((ImVec2){480, 320}, ImGuiCond_Once);
    if (igBegin("Options", &options, ImGuiWindowFlags_None)) {
      igBeginChild_Str("optionscontent", (ImVec2){0, -igGetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_None);
      if (igBeginTabBar("options", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (igBeginTabItem("Video", NULL, ImGuiTabItemFlags_NoCloseButton)) {
          igInputInt2("Resolution", (int*)&options_conf, ImGuiInputTextFlags_None);
          igCheckbox("Fullscreen", &options_conf.fullscreen);
          igCombo_Str("MSAA", &options_conf.msaa, "None\0x2\0x4\0x8\0x16", 0);
          igSliderFloat("FOV", &options_conf.fov, 30, 90, "%.1f", ImGuiSliderFlags_None);
          igEndTabItem();
        }
        if (igBeginTabItem("Input", NULL, ImGuiTabItemFlags_NoCloseButton)) {
          igSliderFloat("Sensitivity", &options_conf.sens, 0.02, 0.4, "%.2f", ImGuiSliderFlags_None);
          for (int i = 0; i < KEYBINDS; i++) {
            if (igButton(SDL_GetScancodeName(conf.binds[i]), VEC2_ZERO)) {
              log_warn("TODO");
            }
            igSameLine(0, 4);
            igText(keybind_names_h[i]);
          }
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
  } else if (!mouse_captured) {
    SDL_SetRelativeMouseMode(true);
    mouse_captured = true;
  }

  if (demo) {
    igShowDemoWindow(&demo);
  }

  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}