#pragma once
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "engine/ecs/world.h"
#include "engine/core/renderer.h"
#include "editor.h"

#define ENVIRONMENT_TITLE ICON_FA_GLOBE " Environment"

void environment_render();
