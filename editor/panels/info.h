#pragma once
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <stdio.h>
#include <string.h>
#include <iconfonts/IconsFontAwesome4.h>
#include <glad/gl.h>

#include "engine/ecs/world.h"
#include "engine/core/gfx.h"
#include "engine/util/ui.h"
#include "editor.h"
#include "engine.h"
#include "log.h"

#define INFO_TITLE ICON_FA_INFO " Info"

void info_render();
