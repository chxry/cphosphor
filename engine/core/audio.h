#pragma once
#include <SDL2/SDL.h>
#include <cmixer.h>
#include <map.h>

#include "engine.h"
#include "log.h"

void audio_init(float volume);
void audio_play(const char* name);
void audio_setvolume(float volume);
