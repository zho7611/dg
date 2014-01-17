// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Interlocked {
public:
  Interlocked() : count_(0) {}
  void Increment();
  void Decrement();
  long Get() const {
    return count_;
  }

private:
  volatile long count_;
};

} // namespace dg
