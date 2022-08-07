#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <fmod.h>
#include <fmod_errors.h>
#include <cglm/cglm.h>
#include <vec.h>

#include "ecs/world.h"
#include "assets.h"
#include "log.h"

extern FMOD_SYSTEM* fmod_system;
extern void FMOD_SDL_Register(FMOD_SYSTEM* system);

void fmod_checked(FMOD_RESULT result);
void audio_init(float volume);
void audio_start();
void audio_update(vec3 pos, vec3 dir);
void audio_play(FMOD_SOUND* sound, vec3 pos);
void audio_setvolume(float volume);
