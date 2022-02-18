#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <vec.h>
#include <time.h>

typedef enum {
  level_info,
  level_error,
  level_lua
} loglevel;

typedef struct {
  loglevel level;
  char* str;
} logevent_t;
typedef vec_t(logevent_t) vec_logevent_t;

extern vec_logevent_t log_events;

void log_init();
void log_log(loglevel level, const char* fmt, ...);
#define log_info(...) log_log(level_info, __VA_ARGS__)
#define log_error(...) log_log(level_error, __VA_ARGS__)
#define log_lua(...) log_log(level_lua, __VA_ARGS__)