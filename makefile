ENGINE_NAME = "Phosphor"
ENGINE_VER = "0.2"

SRC = $(shell find engine/ lib/cglm/src/ lib/physfs/src/ -type f -name '*.c') lib/glad/build/src/gl.c lib/vec/src/vec.c lib/map/src/map.c lib/cmixer/src/cmixer.c lib/parson/parson.c
OBJS = $(SRC:.c=.o)
GAME = game.o
GAMESRC = $(shell find game/ -type f -name '*.c')
GAMEOBJS += $(GAMESRC:.c=.o)
EDITOR = editor.o
EDITORSRC = $(shell find editor/ -type f -name '*.c')
EDITOROBJS += $(EDITORSRC:.c=.o)
BULLETSRC=$(shell find lib/bullet3/src/BulletCollision lib/bullet3/src/BulletDynamics lib/bullet3/src/LinearMath -type f -name '*.cpp')
BULLETOBJS=$(BULLETSRC:.cpp=.o)
RES = res.zip

CFLAGS = -Wall -O2 -DIMGUI_IMPL_API="extern \"C\"" -c -DENGINE_VER=\"$(ENGINE_VER)\" -DENGINE_NAME=\"$(ENGINE_NAME)\"
CFLAGS += -g
CFLAGS += -I. -Iengine -Igame -Ieditor -Ilib -Ilib/glad/build/include -Ilib/vec/src -Ilib/map/src -Ilib/cmixer/src -Ilib/cimgui -Ilib/cimgui/imgui -Ilib/cglm/include -Ilib/physfs/src -I/usr/include/SDL2 -Ilib/bulletcapi
LDFLAGS = -ldl -lSDL2 -lm -llua
MAKEFLAGS += --silent
ECHO = echo -e "\033[1m$(1) \033[0m$(2)"
.SUFFIXES:
.PHONY: game editor

setup: glad cimgui bullet

$(GAME): $(OBJS) $(GAMEOBJS) $(RES)
	$(call ECHO,"linking",$(GAME))
	clang++ $(LDFLAGS) $(OBJS) $(GAMEOBJS) $(BULLETOBJS) lib/cimgui/libcimgui.a imgui_sdl.o imgui_opengl3.o bulletcapi.o -o $(GAME)

$(EDITOR): $(OBJS) $(EDITOROBJS) $(RES)
	$(call ECHO,"linking",$(EDITOR))
	clang++ $(LDFLAGS) $(OBJS) $(EDITOROBJS) $(BULLETOBJS) lib/cimgui/libcimgui.a imgui_sdl.o imgui_opengl3.o bulletcapi.o -o $(EDITOR)

cimgui:
	$(call ECHO,"compiling","cimgui")
	make -C lib/cimgui static
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_sdl.cpp -o imgui_sdl.o
	clang++ $(CFLAGS) lib/cimgui/imgui/backends/imgui_impl_opengl3.cpp -o imgui_opengl3.o

glad:
	$(call ECHO,"generating","glad")
	cd lib/glad && python -m glad --extensions='' --api="gl:core" --out-path=build --reproducible c

bullet: $(BULLETOBJS)
	$(call ECHO,"compiling","bullet3")
	clang++ $(CFLAGS) -Ilib/bullet3/src lib/bulletcapi/capi/capi.cpp -o bulletcapi.o

$(RES): $(shell find res/ -type f)
	$(call ECHO,"packaging",$(RES))
	cd res && zip -r ../$(RES) $(shell cd res && find . -type f)

%.o: %.c
	$(call ECHO,"clang",$<)
	clang $(CFLAGS) $< -o $@

vpath %.c lib/bullet3
%.o: %.cpp
	$(call ECHO,"clang++",$<)
	clang++ $(CFLAGS) -Ilib/bullet3/src $< -o $@

game: $(GAME)
	./$(GAME)

editor: $(EDITOR)
	./$(EDITOR)

format:
	$(call ECHO,"clang-format","linting")
	clang-format -i $(shell find engine/ game/ editor/ res/shaders -type f -name '*.[ch]' -o -name '*.vert' -o -name '*.frag')

clean:
	$(call ECHO,"clean","removing objects")
	rm -rf $(OBJS) $(GAMEOBJS) $(EDITOROBJS) $(BULLETOBJS) $(GAME) $(EDITOR) $(RES)