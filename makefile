SRC = $(shell find engine/ lib/cglm/src/ -type f -name '*.c') lib/glad/build/src/gl.c lib/vec/src/vec.c lib/microtar/src/microtar.c lib/map/src/map.c lib/cmixer/src/cmixer.c lib/parson/parson.c
OBJS = $(SRC:.c=.o)
GAME = game.o
GAMESRC = $(shell find game/ -type f -name '*.c')
GAMEOBJS += $(GAMESRC:.c=.o)
EDITOR = editor.o
EDITORSRC = $(shell find editor/ -type f -name '*.c')
EDITOROBJS += $(EDITORSRC:.c=.o)
RES = res.tar

CFLAGS = -Wall -O2 -DIMGUI_IMPL_API="extern \"C\"" -c
CFLAGS += -I. -Iengine -Igame -Ieditor -Ilib -Ilib/glad/build/include -Ilib/vec/src -Ilib/microtar/src -Ilib/map/src -Ilib/cmixer/src -Ilib/cimgui -Ilib/cimgui/imgui -Ilib/cglm/include -I/usr/include/SDL2
LDFLAGS = -ldl -lSDL2 -lm -llua

$(GAME): $(OBJS) $(GAMEOBJS) $(RES)
	clang++ $(LDFLAGS) $(OBJS) $(GAMEOBJS) lib/cimgui/libcimgui.a imgui_sdl.o imgui_opengl3.o -o $(GAME)

$(EDITOR): $(OBJS) $(EDITOROBJS) $(RES)
	clang++ $(LDFLAGS) $(OBJS) $(EDITOROBJS) lib/cimgui/libcimgui.a imgui_sdl.o imgui_opengl3.o -o $(EDITOR)

cimgui: 
	make -C lib/cimgui static
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_sdl.cpp -o imgui_sdl.o
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_opengl3.cpp -o imgui_opengl3.o

glad: 
	cd lib/glad && python -m glad --extensions='' --api="gl:core" --out-path=build --reproducible c

$(RES): res
	tar -cf $(RES) res

%.o: %.c
	clang $(CFLAGS) $< -o $@

.PHONY: game editor
game: $(GAME)
	./$(GAME)
editor: $(EDITOR)
	./$(EDITOR)

format:
	clang-format -i $(shell find engine/ game/ editor/ -type f -name '*.[ch]')

clean:
	rm -rf $(OBJS) $(GAMEOBJS) $(EDITOROBJS) $(GAME) $(EDITOR) $(RES) imgui_sdl.o imgui_opengl3.o