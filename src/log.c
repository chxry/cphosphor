#include "log.h"

vec_logevent_t log_events;

void log_init() {
  vec_init(&log_events);
}

void log_log(loglevel level, const char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char* buf = malloc(512);
  char time_buf[9];
  time_t now = time(NULL);
  strftime(time_buf, sizeof(time_buf), "%H:%M:%S", localtime(&now));
  printf("\x1b[90m%s ", time_buf);
  switch (level) {
  case level_info:
    printf("\x1b[32;1minfo");
    break;
  case level_error:
    printf("\x1b[31;1merror");
    break;
  case level_lua:
    printf("\x1b[34;1mlua");
    break;
  }
  vsprintf(buf, fmt, va);
  printf(":\x1b[0m %s\n", buf);
  logevent_t event = {
      .level = level,
      .str = realloc(buf, strlen(buf))};
  vec_push(&log_events, event);
  va_end(va);
}
