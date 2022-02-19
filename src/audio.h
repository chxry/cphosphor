#pragma once
#include <SDL2/SDL.h>
#include <cmixer.h>
#include <map.h>

#include "game/game.h"
#include "log.h"

void audio_init();
void audio_play(const char* name);
void audio_setvolume(float volume);
