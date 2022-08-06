#include "engine.h"

char* cpu_name;

void engine_init() {
  srand(time(NULL));
  FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
  if (cpuinfo == NULL) {
    log_error("Could not open /proc/cpuinfo.");
    return;
  }
  size_t i = 0;
  while (getline(&cpu_name, &i, cpuinfo) > 0) {
    if (strstr(cpu_name, "model name")) {
      strtok(cpu_name, ":");
      cpu_name = strtok(NULL, ":") + 1;
      break;
    }
  }
  fclose(cpuinfo);
  log_info("Phosphor v" ENGINE_VER ".");
}
