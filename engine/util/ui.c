#include "ui.h"

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
  igText("Floppa Engine");
  igPopFont();
  igText("github.com/chxry/tester");
  igSameLine(size.x - offset, 0);
  igText("v" VERSION);
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
    if (igTreeNode_Str("glad")) {
      igTextWrapped("Copyright (c) 2013-2022 David Herberth.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("cimgui")) {
      igTextWrapped("Copyright (c) 2015 Stephan Dilly.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("Dear ImGui")) {
      igTextWrapped("Copyright (c) 2014-2022 Omar Cornut.\n\n" MIT_LICENSE);
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
    if (igTreeNode_Str("vec, map & cmixer")) {
      igTextWrapped("Copyright (c) 2014,2017 rxi.\n\n" MIT_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("IconFontCppHeaders")) {
      igTextWrapped("Copyright (c) 2017 Juliette Foucaut and Doug Binks.\n\n" ZLIB_LICENSE);
      igTreePop();
    }
    if (igTreeNode_Str("Assets")) {
      igTextWrapped("3D tree01by mozhdeh parizi");
      igTextWrapped("Quick terrain test by Kris Teper");
      igTreePop();
    }
    igTreePop();
  }
  igPopTextWrapPos();
}