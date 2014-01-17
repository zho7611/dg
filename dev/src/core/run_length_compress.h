// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Simple run-length encoding/decoding
class RunLengthCompress {
public:
  // Returns the compressed bytes
  static int Compress(const uint8_t* source_buffer, int source_length, uint8_t* target_buffer, int target_length);
  static int Uncompress(const uint8_t* source_buffer, int source_length, uint8_t* target_buffer, int target_length);
};

} // namespace dg
