// Blur.vsh

uniform vec4 BlurVertexTexcoordOffset;
attribute vec4 a_position;
attribute vec2 a_uv;
varying vec2 v_uv;

void main() {
  //For OpenGL vertex texcoord offset is not needed
  // Gl_Position = a_position + BlurVertexTexcoordOffset;
  gl_Position = a_position;
  v_uv = a_uv;
}
