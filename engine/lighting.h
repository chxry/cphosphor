#pragma once
#include <glad/gl.h>
#include <cglm/cglm.h>

#include "world.h"
#include "gfx.h"

#define SHADOW_RES 8192

extern unsigned int gbuffer;
extern unsigned int shadow_shader;

void gbuffer_init(int width, int height);
void gbuffer_resize(int width, int height);
void gbuffer_render_shadows(mat4 light_view, mat4 light_projection);
void gbuffer_render(mat4 light_view, mat4 light_projection);