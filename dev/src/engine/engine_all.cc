// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"
#include "render_resource.cc"
#include "engine_config.cc"
#include "animation.cc"
#include "application.cc"
#include "axis_drawer.cc"
#include "camera.cc"
#include "camera_controller.cc"
#include "channel.cc"
#include "renderer.cc"
#include "font.cc"
#include "font_face.cc"
#include "font_manager.cc"
#include "index_buffer.cc"
#include "joint.cc"
#include "light.cc"
#include "light_manager.cc"
#include "line2_drawer.cc"
#include "line_drawer.cc"
#include "material.cc"
#include "mesh.cc"
#include "mesh_builder.cc"
#include "model.cc"
#include "model_animation_controller.cc"
#include "part.cc"
#include "pixel_shader.cc"
#include "texture.cc"
#include "post_effect.cc"
#include "post_effect_blur.cc"
#include "post_effect_shadow.cc"
#include "render_target.cc"
#include "resource_checker.cc"
#include "scene.cc"
#include "scoped_technique.cc"
#include "shader.cc"
#include "render_state.cc"
#include "render_state_set.cc"
#include "sampler_state.cc"
#include "sampler_state_set.cc"
#include "shader_effect.cc"
#include "shader_effect_set.cc"
#include "shader_effect_parser.cc"
#include "shader_parameter.cc"
#include "skeleton.cc"
#include "skeleton_drawer.cc"
#include "skin_joint.cc"
#include "skin_mesh.cc"
#include "sphere_camera.cc"
#include "sprite.cc"
#include "statistics.cc"
#include "register_classes.cc"
#include "vertex_buffer.cc"
#include "vertex_shader.cc"
#include "view.cc"
#include "shadows_manager.cc"
#include "vertex_decl.cc"
#include "engine_main.cc"

#if defined(DG_USE_IMGUI)
#include "../externals/imgui/imgui.cc"
#include "../externals/imgui/imgui_render.cc"
#endif

#if defined(DG_FONT_FREETYPE)
#include "font_face_freetype.cc"
#include "font_manager_freetype.cc"
#elif defined(DG_FONT_STB_TRUETYPE)
#include "font_face_stb_truetype.cc"
#include "font_manager_pure.cc"
#else
#include "font_face_pure.cc"
#include "font_manager_pure.cc"
#endif

#if defined(DG_TARGET_WIN32)
#include "mouse_win32.cc"
#include "keyboard_win32.cc"
#include "application_win32.cc"
#endif

#if defined(DG_RENDERER_DX9)
#include "render_call_dx9.cc"
#include "renderer_dx9.cc"
#include "shader_compiler_dx9.cc"
#elif defined(DG_RENDERER_GL2)
#include "render_call_gl2.cc"
#include "renderer_gl2.cc"
#include "shader_compiler_gl2.cc"
#endif
