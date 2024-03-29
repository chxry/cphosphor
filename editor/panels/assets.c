#include "assets.h"

char* assets_path = "";
char* assets_asset = "";

void asset_button(void* data, char* dir, char* name) {
  PHYSFS_Stat stat;
  PHYSFS_stat(name, &stat);
  ImVec2 pos;
  igGetCursorPos(&pos);
  char buf[32];
  sprintf(buf, "##%s", name);
  if (igSelectable_Bool(buf, strcmp(name, assets_asset) == 0, ImGuiSelectableFlags_None, (ImVec2){80, 80})) {
    if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
      assets_path = name;
      assets_asset = "";
    } else {
      assets_asset = name;
    }
  }
  if (stat.filetype != PHYSFS_FILETYPE_DIRECTORY) {
    if (igBeginDragDropSource(ImGuiDragDropFlags_None)) {
      char buf[128];
      snprintf(buf, sizeof(buf), "%s/%s", dir, name);
      igSetDragDropPayload("DND_ASSET", buf, strlen(buf), ImGuiCond_Always);
      igText("%s", buf);
      igEndDragDropSource();
    }
  }
  igSetCursorPos(pos);
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 10});
  igBeginChild_Str(name, (ImVec2){80, 80}, false, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
  ImVec2 size;
  char* icon = stat.filetype == PHYSFS_FILETYPE_DIRECTORY ? ICON_FA_FOLDER : ICON_FA_FILE;
  igPushFont(large_icons);
  igCalcTextSize(&size, icon, 0, 0, 0);
  igSetCursorPosX((80 - size.x) / 2);
  igText(icon);
  igPopFont();
  igSetWindowFontScale(0.8);
  igCalcTextSize(&size, name, 0, 0, 0);
  igSetCursorPosX((80 - size.x) / 2);
  igText(name);
  igEndChild();
  igPopStyleVar(1);
  igSameLine(0, 8);
}

void assets_render() {
  if (assets_open) {
    if (igBegin(ASSETS_TITLE, &assets_open, ImGuiWindowFlags_None)) {
      igBeginDisabled(!assets_path[0]);
      if (igButton(ICON_FA_ARROW_LEFT, (ImVec2){0, 0})) {
        assets_path = "";
        assets_asset = "";
      }
      igEndDisabled();
      igSameLine(0, 8);
      if (assets_asset[0]) {
        char name[128];
        snprintf(name, sizeof(name), "%s/%s", assets_path, assets_asset);
        PHYSFS_Stat stat;
        PHYSFS_stat(name, &stat);
        int i = 0;
        char* units[] = {"B", "kB", "MB", "GB", "TB"};
        while (stat.filesize > 1024) {
          stat.filesize /= 1024;
          i++;
        }
        igText("%s (%i%s)", name, stat.filesize, units[i]);
      } else {
        igText("%s/", assets_path);
      }
      PHYSFS_enumerateFilesCallback(assets_path, asset_button, NULL);
    }
    igEnd();
  }
}