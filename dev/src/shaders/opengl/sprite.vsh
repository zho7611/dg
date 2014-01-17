// Sprite.vsh

uniform vec4 GlobalInvSurfaceSize;

attribute vec4 a_position;
attribute vec4 a_tex_coord;
attribute vec4 a_color;

varying vec4 v_color;
varying vec4 v_tex_coord;

void main() {
  // A_position is in screen coordinates. [0, 0] is top-left
  gl_Position = a_position;
  gl_Position.xy = 2.0*(a_position.xy)* GlobalInvSurfaceSize.xy - 1.0;
  // Y coordinate should be flipped
  gl_Position.y *= -1.00000;
  v_tex_coord = a_tex_coord;
  v_color = a_color;
}
