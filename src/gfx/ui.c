#include "ui.h"

bool debug_overlay = true;
bool demo = false;

bool options = false;
bool mouse_captured;
conf_t options_conf;
ImVec4 options_color;
int key_select = -1;
void options_reset() {
  options_conf = conf;
  igColorConvertU32ToFloat4(&options_color, conf.crosshair_color);
}

bool console = false;
bool console_scroll;
char console_buf[128] = "";
void console_exec() {
  console_scroll = true;
  if (console_buf[0]) {
    lua_exec(console_buf);
    console_buf[0] = 0;
  }
}

void ui_init(SDL_Window* window, SDL_GLContext* ctx) {
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
  options_reset();
}

void ui_processevent(SDL_Event* e) {
  ImGui_ImplSDL2_ProcessEvent(e);
  if (e->type == SDL_KEYDOWN) {
    SDL_Scancode code = e->key.keysym.scancode;
    if (code == conf.binds[KEYBIND_MENU]) {
      options = !options;
      SDL_SetRelativeMouseMode(!options);
      mouse_captured = !options;
    } else if (code == conf.binds[KEYBIND_CONSOLE]) {
      console = !console;
      console_scroll = true;
    } else if (code == conf.binds[KEYBIND_DEBUG]) {
      debug_overlay = !debug_overlay;
    }
    if (key_select >= 0) {
      options_conf.binds[key_select] = code;
      key_select = -1;
    }
  }
}

void ui_render(SDL_Window* window, int w, int h) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  igNewFrame();
  ImGuiIO io = *igGetIO();

  if (debug_overlay) {
    igSetNextWindowPos((ImVec2){16, 16}, ImGuiCond_Always, VEC2_ZERO);
    igSetNextWindowBgAlpha(0.5);
    if (igBegin("##", &debug_overlay, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize)) {
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
      igBeginChild_Str("##", (ImVec2){0, -igGetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_None);
      if (igBeginTabBar("##", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (igBeginTabItem("Video", NULL, ImGuiTabItemFlags_NoCloseButton)) {
          igInputInt2("Resolution", (int*)&options_conf, ImGuiInputTextFlags_None);
          igCheckbox("Fullscreen", &options_conf.fullscreen);
          igInputInt("FPS Limit", &options_conf.fps, 0, 0, ImGuiInputTextFlags_None);
          igCombo_Str("MSAA", &options_conf.msaa, "None\0x2\0x4\0x8\0x16\0", 0);
          igSliderFloat("FOV", &options_conf.fov, 30, 90, "%.1f", ImGuiSliderFlags_None);
          igEndTabItem();
        }
        if (igBeginTabItem("Input", NULL, ImGuiTabItemFlags_NoCloseButton)) {
          igSliderFloat("Sensitivity", &options_conf.sens, 0.02, 0.4, "%.2f", ImGuiSliderFlags_None);
          for (int i = 0; i < KEYBINDS; i++) {
            if (igButton(key_select == i ? "Cancel" : SDL_GetScancodeName(options_conf.binds[i]), VEC2_ZERO)) {
              key_select = key_select == i ? -1 : i;
            }
            igSameLine(0, 4);
            igText(keybind_names_h[i]);
          }
          igEndTabItem();
        }
        if (igBeginTabItem("Audio", NULL, ImGuiTabItemFlags_NoCloseButton)) {
          igSliderFloat("Volume", &options_conf.volume, 0, 1, "%.1f", ImGuiSliderFlags_None);
          igEndTabItem();
        }
        if (igBeginTabItem("Crosshair", NULL, ImGuiTabItemFlags_NoCloseButton)) {
          igSliderInt("Size", &options_conf.crosshair_size, 1, 100, "%i", ImGuiInputTextFlags_None);
          igSliderInt("Thickness", &options_conf.crosshair_thickness, 1, 10, "%i", ImGuiInputTextFlags_None);
          igColorEdit4("Color", (float*)&options_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
          igEndTabItem();
        }
        igEndTabBar();
      }
      igEndChild();
      if (igButton("Apply", VEC2_ZERO)) {
        options_conf.crosshair_color = igGetColorU32_Vec4(options_color);
        conf = options_conf;
        SDL_SetWindowSize(window, conf.width, conf.height);
        SDL_SetWindowFullscreen(window, conf.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        frame_delay = 1000 / conf.fps;
        audio_setvolume(conf.volume);
        conf_write("conf.json");
      }
      igSameLine(0, 4);
      if (igButton("Reset", VEC2_ZERO)) {
        options_reset();
      }
    }
    igEnd();
  } else if (!mouse_captured) {
    SDL_SetRelativeMouseMode(true);
    mouse_captured = true;
  }

  if (console) {
    igSetNextWindowSize((ImVec2){480, 320}, ImGuiCond_Once);
    char title_buf[32] = "Console | ";
    if (igBegin(strcat(title_buf, LUA_VERSION), &console, ImGuiWindowFlags_None)) {
      igBeginChild_Str("##", (ImVec2){0, -igGetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_None);
      int i;
      logevent_t event;
      vec_foreach(&log_events, event, i) {
        ImVec4 color;
        switch (event.level) {
        case level_error:
          color = (ImVec4){1.0, 0.4, 0.4, 1.0};
          break;
        case level_lua:
          color = (ImVec4){1.0, 1.0, 1.0, 1.0};
          break;
        default:
          color = (ImVec4){0.6, 0.6, 0.6, 1.0};
        }
        igPushStyleColor_Vec4(ImGuiCol_Text, color);
        igText(event.str);
        igPopStyleColor(1);
      }
      if (console_scroll) {
        igSetScrollHereY(1.0);
        igSetKeyboardFocusHere(1);
        console_scroll = false;
      }
      igEndChild();
      if (igInputText("##", console_buf, sizeof(console_buf), ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL)) {
        console_exec();
      }
      igSameLine(0, 4);
      if (igButton("Exec", VEC2_ZERO)) {
        console_exec();
      }
    }
    igEnd();
  }

  if (demo) {
    igShowDemoWindow(&demo);
  }

  ImDrawList* draw = igGetBackgroundDrawList_Nil();
  ImDrawList_AddLine(draw, (ImVec2){w / 2 - conf.crosshair_size, h / 2}, (ImVec2){w / 2 + conf.crosshair_size, h / 2}, conf.crosshair_color, conf.crosshair_thickness);
  ImDrawList_AddLine(draw, (ImVec2){w / 2, h / 2 - conf.crosshair_size}, (ImVec2){w / 2, h / 2 + conf.crosshair_size}, conf.crosshair_color, conf.crosshair_thickness);

  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}