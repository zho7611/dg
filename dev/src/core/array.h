// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Auto grow array
template<class T>
class Array {
public:
  typedef ArrayIterator< Array<T> > Iterator;
  typedef T ValueType;

  Array();
  Array(const Array<T>& rhs);
  ~Array();
  Array<T>& CopyFrom(const Array<T>& rhs);
  Array<T>& operator=(const Array<T>& rhs);
  T& Get(int index);
  T* ptr() { return items_; }
  const T* GetConstPtr() const { return items_; }
  const T& GetConst(int index) const;
  void Set(int index, const T& value);
  int PushBack(const T& item);
  int PopBack();
  T& First();
  T& Last();
  const T& First() const;
  const T& Last() const;
  int Count()const;
  void Clear();
  void Fill(const T& item);
  bool IsEmpty() const;
  // Returns non-negative (>=0) index if the search succeeded
  // Becareful that it can be 0
  int FindSlow(const T& item) const;
  void GrowTo(int new_alloc_size);
  void ShrinkTo(int new_alloc_size);
  void Resize(int new_size);
  void Erase(int idx);
  void Erase(Iterator idx);
  Iterator Begin();
  Iterator End();

private:
  void InternalCopy(const Array<T>& rhs);
  void InternalGrow();

#if defined(DG_DEBUG)
  // This should be called when alloc_size_ or items_ is changed
  void CacheDebug();
#endif

  // Number of items actually valid in items_
  int count_;
  // Number of allocated items in items_. alloc_size_ can be different from count_
  int alloc_size_;
  // Pointer to the items array buffer
  T* items_;
#if defined(DG_DEBUG)
#define DG_ARRAY_DEBUG_CACHE_SIZE (16)
  // This is for checking the items in the debugger easily
  T* items_debug_[DG_ARRAY_DEBUG_CACHE_SIZE];
#endif
};

template<class T>
Array<T>::Array() : alloc_size_(0), count_(0), items_(0) {
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
Array<T>::Array(const Array<T>& rhs) : alloc_size_(0), count_(0), items_(0) {
  if (this != &rhs)
    CopyFrom(rhs);
}

template<class T>
Array<T>::~Array() {
  Clear();
}

template<class T>
Array<T>& Array<T>::operator=(const Array<T>& rhs) {
  return CopyFrom(rhs);
}

template<class T>
int Array<T>::Count() const {
  return count_;
}

template<class T>
void Array<T>::InternalCopy(const Array<T>& rhs) {
  alloc_size_ = rhs.alloc_size_;
  count_ = rhs.count_;
  if (rhs.alloc_size_ > 0) {
    Check(!items_);
    items_ = new T[rhs.alloc_size_];
    for (int idx = 0; idx < count_; ++idx) {
      items_[idx] = rhs.items_[idx];
    }
  }
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
void Array<T>::Fill(const T& item) {
  Check(items_);
  for (int idx = 0; idx < count_; ++idx) {
    items_[idx] = item;
  }
  // It doesn't need CacheDebug() as it changes neighter count_ nor items_
}

template<class T>
void Array<T>::Clear() {
  if (items_) {
    delete [] items_;
    items_ = NULL;
  }
  alloc_size_ = 0;
  count_ = 0;
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
void Array<T>::ShrinkTo(int new_alloc_size) {
  // Do nothing for now
  DG_UNREF(new_alloc_size);
  Check(new_alloc_size < alloc_size_);
}

template<class T>
void Array<T>::GrowTo(int new_alloc_size) {
  T* new_array = NULL;
  if (new_alloc_size > 0) {
    new_array = new T[new_alloc_size];
  }
  if (items_) {
    int copy_count = Min(new_alloc_size, count_);
    for (int idx = 0; idx < copy_count; ++idx) {
      new_array[idx] = items_[idx];
    }
    delete [] items_;
  }
  items_ = new_array;
  alloc_size_ = new_alloc_size;
#if defined(DG_DEBUG)
  CacheDebug();
#endif
}

template<class T>
void Array<T>::Resize(int new_size) {
  if (new_size == alloc_size_) return;
  if (new_size > alloc_size_) GrowTo(new_size);
  else ShrinkTo(new_size);
  count_ = new_size;
}

template<class T>
void Array<T>::InternalGrow() {
  int grow_to_size;
  if (alloc_size_ == 0) {
    grow_to_size = 16;
  } else {
    grow_to_size = alloc_size_ << 1;
  }
  GrowTo(grow_to_size);
}

// Pushes the item and returns the count
template<class T>
int Array<T>::PushBack(const T& item) {
  if (count_ == alloc_size_) {
    InternalGrow();
  }
  Check(items_);
  items_[count_++] = item;
  return count_;
}

template<class T>
T& Array<T>::Get(int index) {
  Check(items_ && (index >= 0) && (index < count_));
  return items_[index];
}

template<class T>
const T& Array<T>::GetConst(int index) const {
  Check(items_ && (index >= 0) && (index < count_));
  return items_[index];
}

template<class T>
void Array<T>::Set(int index, const T& value) {
  Check((index >= 0) && (index < count_));
  items_[index] = value;
}

template<class T>
Array<T>& Array<T>::CopyFrom(const Array<T>& rhs) {
  if (this == &rhs) {
    return *this;
  }
  Clear();
  InternalCopy(rhs);
  return *this;
}

template<class T>
bool Array<T>::IsEmpty() const {
  return (count_ == 0);
}

template<class T>
T& Array<T>::First() {
  Check(Count()> 0);
  return items_[0];
}

template<class T>
const T& Array<T>::First() const {
  Check(Count()> 0);
  return items_[0];
}

template<class T>
T& Array<T>::Last() {
  Check(Count()> 0);
  return items_[Count()-1];
}

template<class T>
const T& Array<T>::Last() const {
  Check(Count()> 0);
  return items_[Count()-1];
}

// Pops the last item and returns the count
template<class T>
int Array<T>::PopBack() {
  Check(Count()> 0);
  --count_;
  return count_;
}

template<class T>
int Array<T>::FindSlow(const T& item) const {
  if (items_) {
    for (int idx = 0; idx < count_; ++idx) {
      if (items_[idx] == item) {
        return idx;
      }
    }
  }
  return -1;
}

template<class T>
void Array<T>::Erase(Iterator idx) {
  Iterator it = Begin();
  Iterator ie = End();
  for (; it != ie; ++it) {
    if (idx == it) {
      Erase(it.GetIndex());
    }
  }
}

template<class T>
void Array<T>::Erase(int idx) {
  for (int i = idx; i < (count_-1); ++i) {
    items_[i] = items_[i+1];
  }
  Resize(Count()-1);
}

template<class T>
typename Array<T>::Iterator Array<T>::Begin() {
  if (Count() > 0) {
    return Iterator(this, 0);
  }
  return End();
}

template<class T>
typename Array<T>::Iterator Array<T>::End() {
  return Iterator(this, -1);
}

#if defined(DG_DEBUG)
template<class T>
void Array<T>::CacheDebug() {
  for (int idx = 0; idx < DG_ARRAY_DEBUG_CACHE_SIZE; ++idx) {
    if (idx >= alloc_size_) {
      items_debug_[idx] = NULL;
    } else {
      items_debug_[idx] = &items_[idx];
    }
  }
}
#endif

} // namespace dg
