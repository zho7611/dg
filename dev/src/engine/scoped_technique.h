// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class View;

class ScopedTechnique {
public:
  virtual void PreRender(View* view, bool is_clear_scene) = 0;
  virtual void PostRender(View* view) = 0;
};

class ScopedTechniqueNone : public ScopedTechnique {
public:
  ScopedTechniqueNone() {
    g_renderer.set_phase(Renderer::Phase_None);
  }
  virtual void PreRender(View* view, bool is_clear_scene) {
    DG_UNREF(view);
    DG_UNREF(is_clear_scene);
  }
  virtual void PostRender(View* view) {
    DG_UNREF(view);
  }
};

class ScopedDepthTechnique : public ScopedTechnique {
public:
  ScopedDepthTechnique() {
    g_renderer.set_phase(Renderer::Phase_Depth);
  }
  virtual void PreRender(View* view, bool is_clear_scene);
  virtual void PostRender(View* view);
};

class ScopedShadowTechnique : public ScopedTechnique {
public:
  ScopedShadowTechnique() {
    g_renderer.set_phase(Renderer::Phase_Shadow);
  }
  virtual void PreRender(View* view, bool is_clear_scene);
  virtual void PostRender(View* view);
};

class ScopedNormalTechnique : public ScopedTechnique {
public:
  ScopedNormalTechnique() {
    g_renderer.set_phase(Renderer::Phase_Normal);
  }
  virtual void PreRender(View* view, bool is_clear_scene);
  virtual void PostRender(View* view);
};

} // namespace dg
