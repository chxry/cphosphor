#include "ui.h"

ImFont* display_font;
ImFont* large_icons;
unsigned int fmod_logo;

void imgui_init(bool docking) {
  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  if (docking) {
    io->ConfigWindowsMoveFromTitleBarOnly = true;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  } else {
    io->IniFilename = NULL;
  }
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

  // asset system isnt initialized yet, probably should use physfs here
  ImFontConfig config = *ImFontConfig_ImFontConfig();
  ImFontAtlas_AddFontFromFileTTF(io->Fonts, "res/fonts/roboto.ttf", 16, &config, NULL);
  config.MergeMode = true;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontAtlas_AddFontFromFileTTF(io->Fonts, "res/fonts/fontawesome.ttf", 12, &config, icon_ranges);
  config.MergeMode = false;
  display_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "res/fonts/comfortaa.ttf", 54, &config, NULL);
  large_icons = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "res/fonts/fontawesome.ttf", 54, &config, icon_ranges);
  ImFontAtlas_Build(io->Fonts);

  glGenTextures(1, &fmod_logo);
  glBindTexture(GL_TEXTURE_2D, fmod_logo);
  int w, h, n;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load("res/tex/fmod.png", &w, &h, &n, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");
}

void asset_selector(char* label, char* selected, char* type) {
  int i;
  void* asset;
  if (igBeginCombo(label, selected, ImGuiComboFlags_None)) {
    vec_foreach(&(map_get(&assets, type)->assets), asset, i) {
      char* a = *((char**)asset);
      if (igSelectable_Bool(a, strcmp(a, selected) == 0, ImGuiSelectableFlags_None, (ImVec2){0, 0})) {
        strcpy(selected, a);
      }
    }
    igEndCombo();
  }
  if (igBeginDragDropTarget()) {
    ImGuiPayload* payload = igAcceptDragDropPayload("DND_ASSET", ImGuiDragDropFlags_None);
    if (payload) {
      strcpy(selected, payload->Data);
    }
  }
}

void ui_info(int offset) {
  ImVec2 size;
  igGetContentRegionAvail(&size);
  igPushFont(display_font);
  igText(ENGINE_NAME);
  igPopFont();
  igText("github.com/chxry/phosphor");
  igSameLine(size.x - offset, 0);
  igText("v" ENGINE_VER);
  igSeparator();
  igPushTextWrapPos(size.x);
  if (igTreeNode_Str("Debug Info")) {
    if (igTreeNode_Str("Peformance")) {
      ImGuiIO io = *igGetIO();
      igText("%.1ffps %.1fms", io.Framerate, io.DeltaTime * 1000);
      igTreePop();
    }
    if (igTreeNode_Str("System")) {
      igText("%s", cpu_name ? cpu_name : "Unknown CPU.");
      igText("%s", glGetString(GL_RENDERER));
      igTreePop();
    }
    if (igTreeNode_Str("Textures")) {
      igText("Shadowmap");
      igImage((void*)(intptr_t)shadowmap, (ImVec2){size.x - 48, size.x - 48}, (ImVec2){0, 1}, (ImVec2){1, 0}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
      igTreePop();
    }
    igTreePop();
  }
  if (igTreeNode_Str("Acknowledgments")) {
    if (igTreeNode_Str("Simple Directmedia Layer")) {
      igTextWrapped("Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>\n\n" ZLIB_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("Lua")) {
      igTextWrapped("Copyright © 1994-2022 Lua.org, PUC-Rio.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("PhysicsFS")) {
      igTextWrapped("Copyright (c) 2001-2022 Ryan C. Gordon <icculus@icculus.org> and others.\n\n" ZLIB_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("Dear ImGui")) {
      igTextWrapped("Copyright (c) 2014-2022 Omar Cornut.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("cimgui")) {
      igTextWrapped("Copyright (c) 2015 Stephan Dilly.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("Bullet Physics")) {
      igTextWrapped("Copyright (c) 2005-2014 Erwin Coumans.\n\n" ZLIB_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("bulletCapi")) {
      igTextWrapped("Copyright (c) chriscamacho.\n\n" GPL_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("stb")) {
      igTextWrapped("Copyright (c) 2017 Sean Barrett.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("cglm")) {
      igTextWrapped("Copyright (c) 2015 Recep Aslantas <info@recp.me>.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("tinyobjloader")) {
      igTextWrapped("Copyright (c) 2012-2019 Syoyo Fujita and many contributors.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("vec & map")) {
      igTextWrapped("Copyright (c) 2014,2017 rxi.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("glad")) {
      igTextWrapped("Copyright (c) 2013-2022 David Herberth.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("FMOD")) {
      igTextWrapped("Copyright (c) 2004-2022 Firelight Technologies Pty Ltd.");
      igTreePop();
    }
    if (igTreeNode_Str("FMOD_SDL")) {
      igTextWrapped("Copyright (c) 2018-2021 Ethan Lee.\n\n" ZLIB_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("IconFontCppHeaders")) {
      igTextWrapped("Copyright (c) 2017 Juliette Foucaut and Doug Binks.\n\n" ZLIB_LICENSE);
      igTreePop();
    }
    igTreePop();
  }
  igPopTextWrapPos();
}

void ui_helpmarker(char* info) {
  igSameLine(0, 4);
  igTextDisabled(ICON_FA_QUESTION_CIRCLE);
  if (igIsItemHovered(ImGuiHoveredFlags_None)) {
    igBeginTooltip();
    igPushTextWrapPos(400);
    igText(info);
    igPopTextWrapPos();
    igEndTooltip();
  }
}

void splash_render(char* msg, int x, int y) {
  glViewport(0, 0, x, y);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  igNewFrame();
  igSetNextWindowPos((ImVec2){0, 0}, ImGuiCond_Always, (ImVec2){0, 0});
  igSetNextWindowSize((ImVec2){x, y}, ImGuiCond_Always);
  igBegin("splash", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

  igPushFont(display_font);
  igSetWindowFontScale(1.5);
  ImVec2 size;
  igCalcTextSize(&size, ENGINE_NAME, 0, 0, 0);
  igSetCursorPosX((x - size.x) / 2);
  igSetCursorPosY((y - size.y) / 2);
  igText(ENGINE_NAME);
  igSetWindowFontScale(0.4);
  igSetCursorPosX((x - size.x) / 2);
  igText(msg);
  igSameLine(x / 2 + size.x / 2 - 36, 0);
  igText("v" ENGINE_VER);
  igPopFont();
  igSetCursorPos((ImVec2){(x - 92) / 2, y - 56.0});
  igImage((void*)(intptr_t)fmod_logo, (ImVec2){92, 24}, (ImVec2){0, 1}, (ImVec2){1, 0}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
  igEnd();

  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
  SDL_GL_SwapWindow(window);
}
