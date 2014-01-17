// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

// Fixed-size memory allocator
class FixedSizeAllocator {
public:
  FixedSizeAllocator(int size_bytes_per_object, int numObjects);
  ~FixedSizeAllocator();
  // Allocate an objcet memory and returns its address
  void* Allocate();
  // Returns true if the deallocation is succeeded
  bool Deallocate(void* object);
  // Returns true if the pool is full and cannot allocate more
  bool IsFull() const {
    return !IsDataInBound(free_data_head_);
  }
  int GetNumObjects() const {
    return num_objects_;
  }
  int GetNumFreeObjects() const {
    return num_free_objects_;
  }
  int GetNumTotalAllocs() const {
    return num_total_allocs_;
  }

  // Next allocator to be used when this allocator is full
  // Singly-Linked List is used to avoid memory copy when the pool needs to be resized
  FixedSizeAllocator* next_;

private:
  // Return true if the data is allocated from this allocator
  // It just checks if the data pointer is in the range of the memory bound
  bool IsDataInBound(uint8_t* data) const {
    return (data >= data_pool_) && (data < (data_pool_ + num_objects_ * GetDataSize()));
  }
  // Get the actual size that an object takes
  int GetDataSize() const {
    return (size_bytes_ + num_objects_);
  }
  // Set the next free data on the data
  void SetNextFreeData(uint8_t* data, uint8_t* nextData) {
    *(reinterpret_cast<uint8_t**>(data)) = nextData;
  }
  // Get the next free data from the data
  uint8_t* GetNextFreeData(uint8_t* data) {
    return *(reinterpret_cast<uint8_t**>(data));
  }
  // Return the object pointer to be returned in Allocate()
  uint8_t* GetObject(uint8_t* data) {
    return data + kTagSize;
  }
  // Return the actual data position from the allocated object pointer
  // The data pointer includes the tag info
  uint8_t* GetData(uint8_t* object) {
    return object - kTagSize;
  }
  // Sets the next free data as un-assigned state on the data
  void SetNextFreeDataUnassigned(uint8_t* data) {
    SetNextFreeData(data, reinterpret_cast<uint8_t*>(0xdeaddead));
  }
  // True if the data is not assigned yet
  bool IsDataUnassigned(uint8_t* data) {
    uint8_t* unassignedState = reinterpret_cast<uint8_t*>(0xdeaddead);
    bool result = (data == unassignedState);
    return result;
  }

  // Per-object size in bytes
  int size_bytes_;
  // Number of objects in this pool
  int num_objects_;
  // Number of available objects
  int num_free_objects_;
  // Accumulated number of allocation for statistics
  int num_total_allocs_;
  // Data pool
  uint8_t* data_pool_;
  // Points to the first available free data in the data pool
  uint8_t* free_data_head_;
  // Every allocated pointer will have the extra tag
  // At the beginning of the allocated memory buffer
  // The Tag will be interpreted as a next free data pointer
  static const int kTagSize = sizeof(uint8_t*);
  // Un-assigned state value
  // The meaning of having un-assigned state is that there are more spaces after the data position
  static const int kUnassignedState = 0xdeaddead;
#if defined(DG_DEBUG)
  static int alloc_id_;
  TreeMap<void*, int> allocated_objects_; // For debug
#endif
};

#if defined(DG_DEBUG)
int FixedSizeAllocator::alloc_id_ = 1;
#endif

FixedSizeAllocator::FixedSizeAllocator(int size_bytes_per_object, int numObjects) :
    size_bytes_(size_bytes_per_object),
    num_objects_(numObjects),
    num_free_objects_(numObjects),
    data_pool_(NULL),
    free_data_head_(NULL),
    next_(NULL),
    num_total_allocs_(0) {
  Check(size_bytes_ > 0);
  Check(num_objects_ > 0);
  const int kTotalSize = num_objects_ * GetDataSize();
  data_pool_ = new uint8_t[kTotalSize];
  free_data_head_ = data_pool_;
  SetNextFreeDataUnassigned(free_data_head_);
}

FixedSizeAllocator::~FixedSizeAllocator() {
  // Check if there are memory leaks
  if (num_free_objects_ != num_objects_) {
#if defined(DG_DEBUG)
    TreeMap<void*, int>::Iterator it = allocated_objects_.Begin();
    TreeMap<void*, int>::Iterator ie = allocated_objects_.End();
    for (; it != ie; ++it) {
      OutputDebugFuncFormat(
          TXT("memorypool.fixedsizeallocator.error: not-freed allocid:%d size:%d ptr:0x%x\n"),
          it.Value(),
          this->size_bytes_,
          it.Key());
    }
#endif
    // Memory leak detected!
    // Goto #memory-pool-leak-detected with allocid which was printed in the debug output
    DG_UNREACHABLE();
  }
#ifdef DG_DEBUG
  // Check if all free data have proper next pointers
  if (free_data_head_) {
    uint8_t* next_free = GetNextFreeData(free_data_head_);
    int free_count = 0;
    while (next_free && !IsDataUnassigned(next_free)) {
      next_free = GetNextFreeData(next_free);
      free_count++;
    }
    Check(free_count <= num_objects_);
  }
#endif
  DG_SAFE_DELETE_ARRAY(data_pool_);
}

void* FixedSizeAllocator::Allocate() {
  if (!free_data_head_) {
    // There is no data available!
    DG_UNREACHABLE();
    return NULL;
  }
  // Get the object pointer
  uint8_t* object = GetObject(free_data_head_);
  // Get the next free data from the head
  uint8_t* nextFreeData = GetNextFreeData(free_data_head_);
  // If the free data is not assigned, then try retrieving the next free data
  if (IsDataUnassigned(nextFreeData)) {
    // Retrieve the next available free data
    free_data_head_ = free_data_head_ + GetDataSize();
    // If the data is still in bound, set the next free data unassigned
    // If not in bound, the free data head will be pointing to the out-of-bound postiion,
    // Which indicates that the pool is full
    if (IsDataInBound(free_data_head_)) {
      SetNextFreeDataUnassigned(free_data_head_);
    } else {
      // Not enough space in this pool anymore
      free_data_head_ = NULL;
      Check(num_free_objects_ == 1);
    }
  } else {
    // If the free data is assigned, just get the next free data from it
    free_data_head_ = nextFreeData;
  }
  num_free_objects_--;
  Check(num_free_objects_ >= 0);
  num_total_allocs_++;
#if defined(DG_DEBUG)
  // #memory-pool-leak-detected
  // Break at the following line and set the StopAllocId to catch the allocation point
  static int StopAllocId = -1;
  if (StopAllocId == alloc_id_) {
    Check(0);
  }
  allocated_objects_.Insert(reinterpret_cast<void*>(object), alloc_id_++);
#endif
  return reinterpret_cast<void*>(object);
}

bool FixedSizeAllocator::Deallocate(void* object) {
  // Convert the object into data
  uint8_t* data = GetData(reinterpret_cast<uint8_t*>(object));
  // If the data is not allocated in this allocator, returns false
  if (!IsDataInBound(data)) {
    return false;
  }
  SetNextFreeData(data, free_data_head_);
  free_data_head_ = data;
  num_free_objects_++;
#if defined(DG_DEBUG)
  allocated_objects_.Erase(allocated_objects_.Find(object));
#endif
  return true;
}

template<int MaxAllocSize = 1024>
class DynamicSizeAllocator {
public:
  DynamicSizeAllocator() {
    InitAllocators();
  }
  ~DynamicSizeAllocator() {
    DestroyAllocators();
  }
  void* Allocate(int size_bytes) {
    if (size_bytes < MaxAllocSize) {
      return FixedAllocate(size_bytes);
    }
    return DefaultAllocate(size_bytes);
  }
  void Deallocate(void* buffer, int size_bytes) {
    if (size_bytes < MaxAllocSize) {
      return FixedDeallocate(buffer, size_bytes);
    }
    return DefaultDeallocate(buffer, size_bytes);
  }
  void ReportAllocatorsUsage();
  static void* DefaultAllocate(int size_bytes) {
    return new uint8_t[size_bytes];
  }
  static void DefaultDeallocate(void* buffer, int size_bytes) {
    delete [] (uint8_t*)buffer;
  }

private:
  void InitAllocators();
  void DestroyAllocators();
  void* FixedAllocate(int size_bytes);
  void FixedDeallocate(void* buffer, int size_bytes);

  // Initially set number of objects per allocator
  static const int kInitialNumObjects_ = 16;
  // Increment rate of the newly created fixed allocators' size
  static const float kIncrementRateAllocatorNumObjects_;
  // Fixed sized allocators array. The size is the index of the array
  FixedSizeAllocator* allocators_[MaxAllocSize];
};

template<int MaxAllocSize>
const float DynamicSizeAllocator<MaxAllocSize>::kIncrementRateAllocatorNumObjects_ = 2.0f;

static DynamicSizeAllocator<>* DynamicSizeAllocator_ = NULL;

template<int MaxAllocSize>
void DynamicSizeAllocator<MaxAllocSize>::InitAllocators() {
  for (int idx = 0; idx < MaxAllocSize; ++idx) {
    allocators_[idx] = NULL;
  }
}

template<int MaxAllocSize>
void DynamicSizeAllocator<MaxAllocSize>::DestroyAllocators() {
  FixedSizeAllocator* allocator = NULL;
  FixedSizeAllocator* allocatorToDelete = NULL;
  for (int idx = 0; idx < MaxAllocSize; ++idx) {
    allocator = allocators_[idx];
    while (allocator) {
      allocatorToDelete = allocator;
      allocator = allocator->next_;
      DG_SAFE_DELETE(allocatorToDelete);
    }
  }
}

template<int MaxAllocSize>
void* DynamicSizeAllocator<MaxAllocSize>::FixedAllocate(int size_bytes) {
  FixedSizeAllocator*& allocator = allocators_[size_bytes];
  if (!allocator) {
    // Create new allocator
    allocator = new FixedSizeAllocator(size_bytes, kInitialNumObjects_);
  }
  // If the allocator is already full, then create another
  if (allocator->IsFull()) {
    FixedSizeAllocator* nextAllocator = allocator;
    int newNumObjects = int(kIncrementRateAllocatorNumObjects_ * allocator->GetNumObjects());
    allocator = new FixedSizeAllocator(size_bytes, newNumObjects);
    allocator->next_ = nextAllocator;
  }
  void* buffer = allocator->Allocate();
  Check(buffer);
  return buffer;
}

template<int MaxAllocSize>
void DynamicSizeAllocator<MaxAllocSize>::FixedDeallocate(void* buffer, int size_bytes) {
  FixedSizeAllocator* allocator = allocators_[size_bytes];
  while (allocator) {
    if (allocator->Deallocate(buffer)) {
      return;
    }
    allocator = allocator->next_;
  }
  // The buffer has not been allocated by this allocator
  DG_UNREACHABLE();
}

template<int MaxAllocSize>
void DynamicSizeAllocator<MaxAllocSize>::ReportAllocatorsUsage() {
  FixedSizeAllocator* allocator = NULL;
  for (int idx = 0; idx < MaxAllocSize; ++idx) {
    allocator = allocators_[idx];
    int num_alloc_per_idx = 0;
    int num_alloc_objects = 0;
    int total_allocs = 0;
    while (allocator) {
      num_alloc_per_idx++;
      num_alloc_objects += allocator->GetNumObjects() - allocator->GetNumFreeObjects();
      total_allocs += allocator->GetNumTotalAllocs();
      allocator = allocator->next_;
    }
    if (num_alloc_per_idx > 0) {
      DG_LOG_LINE(TXT("memorypool.report: idx:% 5d/%d blocks:%-5d objects:%-5d allocs:%-5d"),
          idx,
          MaxAllocSize,
          num_alloc_per_idx,
          num_alloc_objects,
          total_allocs);
    }
  }
}

void* memory_pool::AllocMemoryPool(size_t size_bytes) {
  if (DynamicSizeAllocator_) {
    return DynamicSizeAllocator_->Allocate(size_bytes);
  }
  return DynamicSizeAllocator<>::DefaultAllocate(size_bytes);
}

void memory_pool::FreeMemoryPool(void* buffer, size_t size_bytes) {
  if (DynamicSizeAllocator_) {
    DynamicSizeAllocator_->Deallocate(buffer, size_bytes);
  } else {
    DynamicSizeAllocator<>::DefaultDeallocate(buffer, size_bytes);
  }
}

void memory_pool::ReportMemoryPool() {
  if (DynamicSizeAllocator_) {
    DynamicSizeAllocator_->ReportAllocatorsUsage();
  }
}

void memory_pool::InitMemoryPool() {
  Check(!DynamicSizeAllocator_)
  DynamicSizeAllocator_ = new DynamicSizeAllocator<>;
}

void memory_pool::ClearMemoryPool() {
  DG_SAFE_DELETE(DynamicSizeAllocator_);
}

} // namespace dg
