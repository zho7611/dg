// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
class Chunk;

// Non auto-grow array
// Use Resize() explicitly
// For auto-grow array, use Array class
template<class T>
class FixedArray {
public:
  typedef ArrayIterator<FixedArray<T> > Iterator;
  typedef T ValueType;

  FixedArray();
  FixedArray(int size);
  FixedArray(int size, const T& initial_value);
  FixedArray(const FixedArray<T>& rhs);
  ~FixedArray();
  FixedArray<T>& operator=(const FixedArray<T>& rhs) {
    return CopyFrom(rhs);
  }
  FixedArray<T>& CopyFrom(const FixedArray<T>& rhs);
  void Resize(int new_size);
  int Count()const;
  void Clear();
  void Fill(const T& item);
  const T& GetConst(int index) const;
  T& Get(int index);
  void Set(int index, T& value);
  bool IsEmpty() const;
  int FindSlow(const T& value) const;
  Iterator Begin();
  Iterator End();

private:
  bool IsValidIndex(int index) const {
    return (items_ && (index >= 0) && (index < count_));
  }
  void Alloc(int size);
#if defined(DG_DEBUG)
  // This should be called when count_ or items_ is changed
  void CacheDebug();
#endif

  int count_;
  T* items_;
#if defined(DG_DEBUG)
#define DG_ARRAY_DEBUG_CACHE_SIZE (16)
  // This is for checking the items in the debugger easily
  T* items_debug_[DG_ARRAY_DEBUG_CACHE_SIZE];
#endif
};

template<class T>
FixedArray<T>::FixedArray() :
  count_(0),
  items_(0) {
#if defined(DG_DEBUG)
    CacheDebug();
#endif
}

template<class T>
FixedArray<T>::FixedArray(int size) :
  count_(0),
  items_(0) {
  Alloc(size);
}

template<class T>
FixedArray<T>::FixedArray(int size, const T& initial_value) :
  count_(0),
  items_(0) {
  Alloc(size);
  Fill(initial_value);
}

template<class T>
FixedArray<T>::FixedArray(const FixedArray<T>& rhs) :
  count_(0),
  items_(0) {
  CopyFrom(rhs);
}

template<class T>
FixedArray<T>::~FixedArray() {
  Clear();
}

template<class T>
void FixedArray<T>::Alloc(int size) {
  Clear();
  if (size > 0) {
    items_ = new T[size];
    count_ = size;
  }
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
void FixedArray<T>::Clear() {
  if (items_) {
    delete [] items_;
  }
  count_ = 0;
  items_ = NULL;
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
FixedArray<T>& FixedArray<T>::CopyFrom(const FixedArray<T>& rhs) {
  if (rhs.count_ != count_) {
    Alloc(rhs.count_);
  }
  for (int idx = 0; idx < count_; ++idx) {
    items_[idx] = rhs.items_[idx];
  }
  return *this;
}

template<class T>
T& FixedArray<T>::Get(int index) {
  Check(IsValidIndex(index));
  return items_[index];
}

template<class T>
const T& FixedArray<T>::GetConst(int index) const {
  Check(IsValidIndex(index));
  return items_[index];
}

template<class T>
void FixedArray<T>::Set(int index, T& value) {
  Check(IsValidIndex(index));
  items_[index] = value;
}

template<class T>
void FixedArray<T>::Resize(int new_size) {
  if (new_size == 0) {
    Clear();
    return;
  }
  T* newItems = new T[new_size];
  const int kCopyCount = Min(new_size, count_);
  for (int idx = 0; idx < kCopyCount; ++idx) {
    newItems[idx] = items_[idx];
  }
  delete [] items_;
  items_ = newItems;
  count_ = new_size;
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
int FixedArray<T>::Count() const {
  return count_;
}

template<class T>
void FixedArray<T>::Fill(const T& item) {
  Check(items_);
  for (int idx = 0; idx < count_; ++idx) {
    items_[idx] = item;
  }
}

template<class T>
bool FixedArray<T>::IsEmpty() const {
  return count_ == 0;
}

// Returns non-negative index if the search succeeded
template<class T>
int FixedArray<T>::FindSlow(const T& value) const {
  if (items_) {
    for (int idx = 0; idx < count_; ++idx) {
      if (items_[idx] == value) {
        return idx;
      }
    }
  }
  return -1;
}

template<class T>
typename FixedArray<T>::Iterator FixedArray<T>::Begin() {
  if (Count() > 0) {
    return Iterator(this, 0);
  }
  return End();
}

template<class T>
typename FixedArray<T>::Iterator FixedArray<T>::End() {
  return Iterator(this, -1);
}

#if defined(DG_DEBUG)
template<class T>
void FixedArray<T>::CacheDebug() {
  for (int idx = 0; idx < DG_ARRAY_DEBUG_CACHE_SIZE; ++idx) {
    if (idx >= count_) {
      items_debug_[idx] = NULL;
    } else {
      items_debug_[idx] = &items_[idx];
    }
  }
}
#endif
} // namespace dg
