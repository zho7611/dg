// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

const uint8_t kRunLengthEscapeCode = 0xff;

// If the target_buffer is NULL, this method returns the compressed length without actually compressing
int RunLengthCompress::Compress(
    const uint8_t* source_buffer, int source_length,
    uint8_t* target_buffer, int target_length) {
  const int kMinRepeatCount = 2;
  const int kMaxCount = 254;
  if (source_length < 1) {
    return 0;
  }
  int count = 0;
  uint8_t value;
  uint8_t lastValue = source_buffer[0];
  int target_idx = 0;
  for (int srcIdx = 1; srcIdx < source_length; ++srcIdx) {
    value = source_buffer[srcIdx];
    if ((value == lastValue) && (count <= kMaxCount)) {
      count++;
    } else {
      // If the count is too small, then just repeat the value without count
      if ((count <= kMinRepeatCount) && (lastValue != kRunLengthEscapeCode)) {
        if (target_buffer) {
          target_buffer[target_idx++] = lastValue;
          Check(target_idx <= target_length);
        } else {
          target_idx++;
        }
        while (count-- > 0) {
          if (target_buffer) {
            target_buffer[target_idx++] = lastValue;
            Check(target_idx <= target_length);
          } else {
            target_idx++;
          }
        }
      } else {
        if (target_buffer) {
          target_buffer[target_idx++] = kRunLengthEscapeCode;
          Check(target_idx <= target_length);
          target_buffer[target_idx++] = count;
          Check(target_idx <= target_length);
          target_buffer[target_idx++] = lastValue;
          Check(target_idx <= target_length);
        } else {
          target_idx += 3;
        }
      }
      lastValue = value;
      count = 0;
    }
  }
  if (target_buffer) {
    target_buffer[target_idx++] = kRunLengthEscapeCode;
    Check(target_idx <= target_length);
    target_buffer[target_idx++] = count;
    Check(target_idx <= target_length);
    target_buffer[target_idx++] = lastValue;
    Check(target_idx <= target_length);
  } else {
    target_idx += 3;
  }
  return target_idx;
}

// If the target_buffer is NULL, this method returns the uncompressed length without actually uncompressing
int RunLengthCompress::Uncompress(
    const uint8_t* source_buffer, int source_length,
    uint8_t* target_buffer, int target_length) {
  uint8_t value;
  int count;
  int src_idx = 0;
  int target_idx = 0;
  while (src_idx < source_length) {
    if (source_buffer[src_idx] == kRunLengthEscapeCode) {
      // Eat the espace value
      ++src_idx;
      count = source_buffer[src_idx++];
      value = source_buffer[src_idx++];
      while (count >= 0) {
        if (target_buffer) {
          target_buffer[target_idx++] = value;
          Check(target_idx <= target_length);
        } else {
          target_idx++;
        }
        --count;
      }
    } else {
      if (target_buffer) {
        target_buffer[target_idx++] = source_buffer[src_idx++];
        Check(target_idx <= target_length);
      } else {
        src_idx++;
        target_idx++;
      }
    }
  }
  return target_idx;
}

} // namespace dg
