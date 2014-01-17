// Line.vsh

uniform mat4 GlobalViewProjectionMatrix;

attribute vec4 a_position; 
attribute vec4 a_color; 

varying vec4 v_color;

void main() {
  gl_Position = a_position * GlobalViewProjectionMatrix;
  v_color = a_color;
}