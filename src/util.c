#include "util.h"

const char* read_file(const char* path) {
  char* buf = 0;
  long length;
  FILE* f = fopen(path, "rb");
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = (char*)malloc((length + 1) * sizeof(char));
  if (buf) {
    fread(buf, sizeof(char), length, f);
  }
  fclose(f);
  buf[length] = '\0';
  return buf;
}