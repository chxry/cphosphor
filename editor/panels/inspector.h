#pragma once
#include <string.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cglm/cglm.h>
#include <iconfonts/IconsFontAwesome4.h>
#include <map.h>

#include "engine/ecs/world.h"
#include "editor.h"

#define INSPECTOR_TITLE ICON_FA_INFO_CIRCLE " Inspector"

void inspector_render();
