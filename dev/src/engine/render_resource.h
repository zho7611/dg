// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Resources that are managed by the renderer
class RenderResource : public Resource {
protected:
  virtual bool InitResource();
  virtual bool ClearResource();
  virtual void SetCurrent() {}
};

} // namespace dg
