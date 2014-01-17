// Simple.vsh

mat3 xll_constructMat3( mat4 m) {
  return mat3( vec3( m[0]), vec3( m[1]), vec3( m[2]));
}
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

uniform vec4 GlobalCameraPosition;
uniform vec4 GlobalLightDirection;
uniform mat4 GlobalLightViewProjectionMatrix;
uniform mat4 GlobalViewProjectionMatrix;
uniform mat4 GlobalWorldMatrix;

vec4 GetWorldPos(in vec4 position) {
  return (position * GlobalWorldMatrix);
}

void CalcTangentBasis(in VertexShaderInput xlat_var_input, out mat3 outTangentBasis) {
  vec3 tangent;
  vec3 normal;
  vec3 binormal;
  tangent = normalize((xlat_var_input.tangent.xyz  * xll_constructMat3(GlobalWorldMatrix)));
  normal = normalize((xlat_var_input.normal.xyz  * xll_constructMat3(GlobalWorldMatrix)));
  binormal = (normalize(cross(normal, tangent)) * xlat_var_input.tangent.w);
  // For OpenGL, no need for transpose()
  outTangentBasis = mat3(tangent, binormal, normal);
}

PixelShaderInput Main(in VertexShaderInput xlat_var_input) {
  vec4 world_pos;
  PixelShaderInput xlat_var_output;
  mat3 tangent_basis;
  vec3 eye_dir_in_world;
  world_pos = GetWorldPos(xlat_var_input.position);
  xlat_var_output.interpolated_position = xlat_var_output.position = (world_pos * GlobalViewProjectionMatrix);
  xlat_var_output.tex_coord = vec4(xlat_var_input.tex_coord.x , xlat_var_input.tex_coord.y , 0.000000, 0.000000);
  CalcTangentBasis(xlat_var_input, tangent_basis);
  xlat_var_output.light_dir = vec4(normalize((GlobalLightDirection.xyz  * tangent_basis)), 1.00000);
  eye_dir_in_world = normalize((GlobalCameraPosition.xyz  - (world_pos.xyz  / world_pos.w)));
  xlat_var_output.eye_dir = vec4(normalize(eye_dir_in_world * tangent_basis), 1.0);
  xlat_var_output.half_dir = vec4(normalize(xlat_var_output.light_dir.xyz + xlat_var_output.eye_dir.xyz), 1.0);
  xlat_var_output.shadow_position = world_pos * GlobalLightViewProjectionMatrix;
  return xlat_var_output;
}

attribute vec4 a_position; 
attribute vec4 a_normal;
attribute vec4 a_tex_coord; 
attribute vec4 a_tangent;

varying vec4 v_tex_coord;
varying vec4 v_eye_dir;
varying vec4 v_light_dir;
varying vec4 v_shadow_position;
varying vec4 v_half_dir;
varying vec4 v_interpolated_position;

void main() {
  PixelShaderInput var_out;
  VertexShaderInput var_in;
  var_in.position = a_position;
  var_in.normal = a_normal;
  var_in.tex_coord = a_tex_coord;
  var_in.tangent = a_tangent;
  var_out = Main(var_in);
  gl_Position = vec4(var_out.position);
  v_tex_coord = vec4(var_out.tex_coord);
  v_eye_dir = vec4(var_out.eye_dir);
  v_light_dir = vec4(var_out.light_dir);
  v_shadow_position = vec4(var_out.shadow_position);
  v_half_dir = vec4(var_out.half_dir);
  v_interpolated_position = vec4(var_out.interpolated_position);
}
