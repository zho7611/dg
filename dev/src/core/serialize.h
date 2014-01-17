// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
class Chunk;

class Serialize {
public:
  enum SerializeState {
      SerializeState_Closed = 0,
      SerializeState_Opened = 1,
      SerializeState_Updated = 2,
      SerializeState_Dirty = 3,
  };

  virtual void ReadFrom(class Stream* stream) DG_PURE;
  virtual void WriteTo(class Stream* out_stream) DG_PURE;
};

} // namespace dg
