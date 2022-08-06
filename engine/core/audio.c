#include "audio.h"

FMOD_SYSTEM* fmod_system;

void fmod_checked(FMOD_RESULT result) {
  if (result != FMOD_OK) {
    log_error("FMOD: %s", FMOD_ErrorString(result));
  }
}

void audio_init(float volume) {
  fmod_checked(FMOD_System_Create(&fmod_system, FMOD_VERSION));
  fmod_checked(FMOD_System_Init(fmod_system, 512, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_PROFILE_ENABLE, 0));
  FMOD_SDL_Register(fmod_system);
  audio_setvolume(volume);
  log_info("Loaded FMOD.");
}

void audio_update(vec3 pos, vec3 dir) {
  vec3 right, up;
  glm_vec3_crossn(GLM_YUP, dir, right);
  glm_vec3_cross(dir, right, up);
  fmod_checked(FMOD_System_Set3DListenerAttributes(fmod_system, 0, pos, GLM_VEC3_ZERO, dir, up));
  fmod_checked(FMOD_System_Update(fmod_system));
}

void audio_play(FMOD_SOUND* sound, vec3 pos) {
  FMOD_CHANNEL* channel;
  fmod_checked(FMOD_System_PlaySound(fmod_system, sound, 0, false, &channel));
  fmod_checked(FMOD_Channel_Set3DAttributes(channel, pos, GLM_VEC3_ZERO));
}

void audio_setvolume(float volume) {
  FMOD_CHANNELGROUP* master;
  fmod_checked(FMOD_System_GetMasterChannelGroup(fmod_system, &master));
  fmod_checked(FMOD_ChannelGroup_SetVolume(master, volume));
}
