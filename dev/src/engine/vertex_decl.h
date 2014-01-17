// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

struct VertexElement {
  enum Usage {
      Usage_Position,
      Usage_Color,
      Usage_Normal,
      Usage_TexCoord,
      Usage_Tangent,
      Usage_Binormal,
      Usage_JointIndex,
      Usage_JointWeight,
  };
  enum DataType {
      DataType_Float1 = 0,
      DataType_Float2 = 1,
      DataType_Float3 = 2,
      DataType_Float4 = 3,
      DataType_Color = 4,
  };

  Usage usage;
  int usage_index;
  DataType data_type;

  VertexElement() :
    usage(Usage_Position),
    usage_index(0),
    data_type(DataType_Float1)
  {}

  VertexElement(DataType in_data_type, Usage in_usage, int in_usage_index = 0) {
    usage = in_usage;
    usage_index = in_usage_index;
    data_type = in_data_type;
  }

  int GetNumComponents() const {
    switch (data_type) {
    case DataType_Float1:
      return 1;
    case DataType_Float2:
      return 2;
    case DataType_Float3:
      return 3;
    case DataType_Float4:
      return 4;
    default:
      DG_UNIMPLEMENTED();
    }
    return 0;
  }

  int GetElementSize() const;
};

class VertexDecl : public RenderResource {
public:
  // Pos : Vector3
  // Col : Color
  // Nor : Normal
  // Tex : TextureCoordinate
  // Tan : Tangent
  // Bin : Binormal
  // Ji4 : JointIndex(int) * 4
  // Jw4 : jointWeght(float) *4
  enum Preset {
      Preset_None = 0,
      Preset_PosCol,
      Preset_PosTex,
      Preset_PosNorTexTan,
      Preset_PosTexCol,
      Preset_PosNorTexTanJi4Jw4,
      Preset_Custom = 255,
  };

  VertexDecl();
  ~VertexDecl();
  void SubmitVertexDecl();
  void SetCurrent();
  int stride() const;
  void CopyFrom(VertexDecl* decl);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  bool OnInitDevice();
  bool OnClearDevice();
  bool SerializeToDevice() {
    return true;
  }
  bool SerializeFromDevice() {
    return true;
  }
  void InitByPreset(Preset preset);
  const Preset& GetPreset() const {
    return decl_preset_;
  }
  Array<VertexElement>& elements() {
    return elements_;
  }

private:
  Preset decl_preset_;
  Array<VertexElement> elements_;
  VertexLayoutHandle device_vertex_declaration_handle_;
};

} // namespace dg
