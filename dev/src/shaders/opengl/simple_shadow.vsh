// Simple_shadow.vsh

struct VertexShaderInput {
  vec4 position;
  vec4 normal;
  vec4 tex_coord;
  vec4 tangent;
};

struct PixelShaderInput {
  vec4 position;
  vec4 tex_coord;
  vec4 eye_dir;
  vec4 light_dir;
  vec4 shadow_position;
  vec4 half_dir;
  vec4 interpolated_position;
};

struct PixelShaderInputForShadow {
  vec4 position;
  vec4 tex_coord;
  vec4 depth;
};

uniform mat4 GlobalViewProjectionMatrix;
uniform mat4 GlobalWorldMatrix;

vec4 GetWorldPos(in vec4 position) {
  return (position * GlobalWorldMatrix);
}

PixelShaderInputForShadow Main(in vec4 position, in vec4 normal, in vec4 tex_coord) {
  vec4 world_pos;
  PixelShaderInputForShadow var_out;
  world_pos = GetWorldPos(position);
  var_out.position = (world_pos * GlobalViewProjectionMatrix);
  var_out.tex_coord.xyzw  = vec4(tex_coord.x, tex_coord.y, 0.0, 0.0);
  var_out.depth.xyzw  = vec4(var_out.position.z, var_out.position.w, 0.0, 0.0);
  return var_out;
}

attribute vec4 a_position; 
attribute vec4 a_normal;
attribute vec4 a_tex_coord; 
attribute vec4 a_tangent;

varying vec4 v_tex_coord;
varying vec4 v_depth;

void main() {
  vec4 world_pos = GetWorldPos(a_position);
  gl_Position = world_pos * GlobalViewProjectionMatrix;
  v_tex_coord.xyzw  = vec4(a_tex_coord.x, a_tex_coord.y, 0.0, 0.0);
  v_depth.xyzw  = vec4(gl_Position.z, gl_Position.w, 0.0, 0.0);
}
