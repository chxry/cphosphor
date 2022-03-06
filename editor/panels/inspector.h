#pragma once
#include <string.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cglm/cglm.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "engine/world.h"
#include "editor.h"

#define INSPECTOR_TITLE ICON_FA_INFO_CIRCLE " Inspector"
#define TRANSFORM_COMPONENT ICON_FA_ARROWS " Transform"
#define MODEL_COMPONENT ICON_FA_CUBE " Model"
#define COLLIDER_COMPONENT ICON_FA_CUBES " Collider"

void inspector_render();
