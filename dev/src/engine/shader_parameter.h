// (c) 2013 BY JIHO CHOI
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ShaderParameter {
public:
  friend class ShaderCompiler;

  enum ValueType {
    kUnknownType,
    kVector4Type,
    kMatrixType,
    kTextureType,
  };

  ShaderParameter();
  ~ShaderParameter();
  void SubmitParameter();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  // If the values is null, then it will be initialized by zero
  void SetFloatArray(const float* values, int num_floats);
  void SetVector4(const Vector4* value);
  // If the values is null, then it will be initialized by zero
  // The values are float array and its size should be num_vector4 * 4;
  void SetVector4Array(const float* values, int num_vector4);
  void SetMatrix(const Matrix* matrix);
  void SetTexture(Texture* texture);
  Vector4 GetVector4() const;
  Matrix GetMatrix() const;
  const Texture& GetTexture() const;
  void CopyValueFrom(const ShaderParameter& rhs);
  ShaderParameter& operator =(const ShaderParameter& rhs) {
    return CopyFrom(rhs);
  }
  ShaderParameter& CopyFrom(const ShaderParameter& rhs);
  void SetDefaultValueByType(ValueType type);
  void BindResource();
  // Name
  const String& name() const { return name_; }
  void set_name(const Cstr* in_name) { name_.Set(in_name); }
  // Value_type
  ValueType value_type() const { return value_type_; }
  void set_value_type(ValueType in_value_type) { value_type_ = in_value_type; }
  // Shader_type
  ShaderType shader_type() const { return shader_type_; }
  void set_shader_type(ShaderType in_shader_type) { shader_type_ = in_shader_type; }
  // Register_index
  int register_index() const { return register_index_; }
  void set_register_index(int in_register_index) { register_index_ = in_register_index; }
  // Register_count
  int register_count() { return register_count_; }
  void set_register_count(int in_register_count) { register_count_ = in_register_count; }

private:
  // #todo This structure takes lots of memory. Make it a union or something else
  struct VariantType {
    Array<float> float_array_value;
    Ptr<Texture> texture_value;
  };

  String name_;
  VariantType variant_;
  // Vertex shader or pixel shader
  ShaderType shader_type_;
  ValueType value_type_;
  // Shader constant register index, or texture stage index (for the texture parameter)
  int register_index_;
  // Register count in the shader. A register have 4 floats
  // The element count of the float array could be different from the register count
  // For texture parameter, this value is ignored
  int register_count_;
  // If true, it means that this parameter is no longer used in the shader program
  bool is_disabled_;

protected:
  static void SetShaderConstantVector4(const Cstr* name, ShaderType in_shader_type,
      int in_register_index, const float* data, int in_register_count);
  static void SetShaderConstantMatrix(const Cstr* name, ShaderType in_shader_type,
      int in_register_index, const Matrix* data);
};

} // namespace dg
