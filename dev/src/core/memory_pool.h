// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

namespace memory_pool {

void InitMemoryPool();
void ClearMemoryPool();
// Allocate memory from the pool
void* AllocMemoryPool(size_t size_bytes);
// Free memory from the pool
void FreeMemoryPool(void* buffer, size_t size_bytes);
void ReportMemoryPool();

} // namespace memory_pool

class MemoryPool {
public:
  static void* operator new(size_t size_bytes) {
    return memory_pool::AllocMemoryPool(size_bytes);
  }
  static void operator delete(void* buffer, size_t size_bytes) {
    memory_pool::FreeMemoryPool(buffer, size_bytes);
  }
};

} // namespace dg
