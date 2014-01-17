// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class RefCount : public MemoryPool {
public:
  RefCount() : ref_count_(0) {
  }
  virtual ~RefCount() {
    Check(ref_count_ == 0);
  }
  virtual void AddRef() {
    ++ref_count_;
  }
  virtual void Release() {
    if (--ref_count_ == 0) {
      delete this;
    }
  }

protected:
  int ref_count_;
};
}
