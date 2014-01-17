// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void RegisterClasses() {
  DG_REGISTER_CLASS(Application);
  DG_REGISTER_CLASS(View);
  DG_REGISTER_CLASS(Scene);
  DG_REGISTER_CLASS(Model);
  DG_REGISTER_CLASS(Mesh);
  DG_REGISTER_CLASS(Texture);
  DG_REGISTER_CLASS(Material);
  DG_REGISTER_CLASS(Animation);
  DG_REGISTER_CLASS(Skeleton);
  DG_REGISTER_CLASS(Part);
  DG_REGISTER_CLASS(Camera);
  DG_REGISTER_CLASS(VertexShader);
  DG_REGISTER_CLASS(PixelShader);
  DG_REGISTER_CLASS(SkinMesh);
  DG_REGISTER_CLASS(SphereCamera);
  DG_REGISTER_CLASS(ShaderEffect);
  DG_REGISTER_CLASS(DepthShaderEffect);
  DG_REGISTER_CLASS(SimpleShaderEffect);
  DG_REGISTER_CLASS(BlurShaderEffect);
  DG_REGISTER_CLASS(BlendShaderEffect);
  DG_REGISTER_CLASS(SpriteShaderEffect);
  DG_REGISTER_CLASS(LineShaderEffect);
  DG_REGISTER_CLASS(Line2ShaderEffect);
  DG_REGISTER_CLASS(ImguiShaderEffect);
  DG_REGISTER_CLASS(DefaultShaderEffect);
  DG_REGISTER_CLASS(PostEffectShadow);
  DG_REGISTER_CLASS(PostEffectBlur);
  DG_REGISTER_CLASS(Font);
}

} // namespace dg
