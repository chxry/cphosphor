#include "audio.h"

map_t(cm_Source*) sounds;

void audio_callback(void* udata, unsigned char* stream, int size) {
  cm_process((void*)stream, size / 2);
}

void audio_init(float volume) {
  SDL_AudioSpec fmt, got;
  memset(&fmt, 0, sizeof(fmt));
  fmt.freq = 44100;
  fmt.format = AUDIO_S16;
  fmt.channels = 2;
  fmt.samples = 1024;
  fmt.callback = audio_callback;

  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &fmt, &got, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  SDL_PauseAudioDevice(dev, 0);

  cm_init(got.freq);
  cm_set_master_gain(volume);
  log_info("Loaded audio at %iHz on \"%s\".", got.freq, SDL_GetCurrentAudioDriver());
}

void audio_play(const char* name) {
  cm_Source** src;
  if ((src = map_get(&sounds, name))) {
    cm_stop(*src);
    cm_play(*src);
  } else {
    asset_t asset = asset_load(name);
    cm_Source* src = cm_new_source_from_mem(asset.data, asset.len);
    cm_play(src);
    map_set(&sounds, name, src);
  }
}

void audio_setvolume(float volume) {
  cm_set_master_gain(volume);
}
