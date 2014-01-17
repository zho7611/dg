// Simple_depth.vsh

uniform mat4 GlobalViewProjectionMatrix;
uniform mat4 GlobalWorldMatrix;

vec4 GetWorldPos(in vec4 position) {
  return (position * GlobalWorldMatrix);
}

attribute vec4 a_position; 
attribute vec4 a_normal;
attribute vec4 a_tex_coord;
attribute vec4 a_tangent;

varying vec4 v_depth;

void main() {
  vec4 world_pos = GetWorldPos(a_position);
  gl_Position = world_pos * GlobalViewProjectionMatrix;
  v_depth = vec4(gl_Position.z , gl_Position.w , 0.0, 0.0);
}

