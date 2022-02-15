SRC = $(shell find src/ lib/cglm/src/ -type f -name '*.c') lib/glad/build/src/gl.c lib/log/src/log.c lib/ini/src/ini.c lib/vec/src/vec.c lib/microtar/src/microtar.c
OBJS = $(SRC:.c=.o)
TARGET = out

CFLAGS = -Wall -O2 -c -DIMGUI_IMPL_API="extern \"C\"" -DLOG_USE_COLOR -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS
CFLAGS += -Isrc -Ilib -Ilib/glad/build/include -Ilib/log/src -Ilib/ini/src -Ilib/vec/src -Ilib/microtar/src -Ilib/cimgui -Ilib/cimgui/imgui -Ilib/cglm/include -I/usr/include/SDL2
LDFLAGS = -ldl -lSDL2 -lm

$(TARGET): $(OBJS) cimgui glad
	tar -cf res.tar res
	clang++ $(LDFLAGS) $(OBJS) lib/cimgui/libcimgui.a imgui_sdl.o imgui_opengl3.o -o $(TARGET)

cimgui: 
	make -C lib/cimgui static
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_sdl.cpp -c -o imgui_sdl.o
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_opengl3.cpp -c -o imgui_opengl3.o

glad: 
	cd lib/glad && python -m glad --extensions='' --api="gl:core" --out-path=build c

%.o: %.c
	clang $(CFLAGS) $< -o $@

run: $(TARGET)
	./out

format:
	clang-format -i $(shell find src/ -type f -name '*.[ch]')

clean:
	rm -rf ${OBJS} ${TARGET} ${RES} imgui_sdl.o imgui_opengl3.o