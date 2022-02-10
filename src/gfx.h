#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <log.h>

void window_init(char* title, int w, int h);
void window_loop();
void window_destroy();