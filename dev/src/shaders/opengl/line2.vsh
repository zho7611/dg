// Line2.vsh

uniform vec4 GlobalInvSurfaceSize;

attribute vec4 a_position; 
attribute vec4 a_color; 

varying vec4 v_color;

void main() {
  gl_Position = a_position;
  gl_Position.xy = ((vec2(2.0 * (a_position - 0.5)) * GlobalInvSurfaceSize.xy ) - 1.0);
  gl_Position.y *= -1.0;
  v_color = a_color;
}
