SRC = $(shell find src/ lib/cglm/src/ -type f -name '*.c') lib/glad/glad.c lib/log/src/log.c lib/ini/src/ini.c lib/vec/src/vec.c
OBJS = $(SRC:.c=.o)
TARGET = out

CFLAGS = -Wall -O2 -c -DIMGUI_IMPL_API="extern \"C\"" -DLOG_USE_COLOR -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS
CFLAGS += -Isrc -Ilib -Ilib/log/src -Ilib/ini/src -Ilib/vec/src -Ilib/cimgui -Ilib/cimgui/imgui -Ilib/cglm/include -Ilib/stb -I/usr/include/SDL2
LDFLAGS = -ldl -lSDL2 -lm

$(TARGET): $(OBJS) cimgui
	clang++ $(LDFLAGS) $(OBJS) lib/cimgui/cimgui.so imgui_sdl.o imgui_opengl3.o -o $(TARGET)

cimgui: 
	make -C lib/cimgui
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_sdl.cpp -c -o imgui_sdl.o
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_opengl3.cpp -c -o imgui_opengl3.o

%.o: %.c
	clang $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

format:
	clang-format -i $(shell find src/ -type f -name '*.[ch]')