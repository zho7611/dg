// Imgui.vsh
uniform vec4 Viewport;

attribute vec4 VertexPosition;
attribute vec2 VertexTexCoord;
attribute vec4 VertexColor;

varying vec2 texCoord;
varying vec4 vertexColor;

void main(void) {
    vertexColor = VertexColor;
    texCoord = VertexTexCoord;
    gl_Position = vec4(VertexPosition.xy * 2.0 / Viewport.xy - 1.0, 0.f, 1.0);
};