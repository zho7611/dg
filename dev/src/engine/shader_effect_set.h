// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

// All ShaderEffect derived classes are defined here
namespace dg {

class DepthShaderEffect : public ShaderEffect {
public:

  DG_DECLARE_RESOURCE(DepthShaderEffect, ShaderEffect);
};

class SimpleShaderEffect : public ShaderEffect {
public:

protected:
  virtual void SetupEffect();
  void SetDepthTexture();
  void SetShadowTexture();

  DG_DECLARE_RESOURCE(SimpleShaderEffect, ShaderEffect);
};

class BlendShaderEffect : public ShaderEffect {
public:

protected:
  virtual void SetupEffect();

  DG_DECLARE_RESOURCE(BlendShaderEffect, ShaderEffect);
};

class BlurShaderEffect : public ShaderEffect {
public:
  BlurShaderEffect();
  ~BlurShaderEffect();
  enum BlurPhase {
      BlurPhase_None,
      BlurPhase_DownSample,
      BlurPhase_Blur,
  };
  void SetBlurPhase(BlurPhase phase) {
    blur_phase_ = phase;
  }
  BlurPhase GetBlurPhase() const {
    return blur_phase_;
  }

protected:
  virtual void SetupEffect();
  BlurPhase blur_phase_;

  DG_DECLARE_RESOURCE(BlurShaderEffect, ShaderEffect);
};

class SpriteShaderEffect : public ShaderEffect {
public:
  void PreRender(Sprite::Type sprite_type);
  void PostRender();
  void SubmitQuad(
      Sprite* sprite, Texture* texture, const Rect& rect,
      const Rect& tex_coord_rect, const ColorInt& color);

  DG_DECLARE_RESOURCE(SpriteShaderEffect, ShaderEffect);
};

class LineShaderEffect : public ShaderEffect {
public:
  LineShaderEffect();
  void PreRender();
  void PostRender();
  void SubmitLine(const Vector3& start, const Vector3& end, const ColorInt& color);

private:
  bool is_begun_;

  DG_DECLARE_RESOURCE(LineShaderEffect, ShaderEffect);
};

// 2-dimentional Line shader effect
class Line2ShaderEffect : public ShaderEffect {
public:
  Line2ShaderEffect() : is_begun_(false) {}
  void PreRender();
  void PostRender();
  void SubmitLine(Point2 start_point, Point2 end_point, ColorInt color);

private:
  bool is_begun_;

  DG_DECLARE_RESOURCE(Line2ShaderEffect, ShaderEffect);
};

class ImguiShaderEffect : public ShaderEffect {
public:
  ImguiShaderEffect() : is_begun_(false) {}
  void PreRender();
  void PostRender();

private:
  bool is_begun_;

  DG_DECLARE_RESOURCE(ImguiShaderEffect, ShaderEffect);
};

class DefaultShaderEffect : public ShaderEffect {
public:

  DG_DECLARE_RESOURCE(DefaultShaderEffect, ShaderEffect);
};

} // namespace dg
