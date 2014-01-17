// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
class PointerIndexMap {
public:
  T* GetPointer(int index);
  int GetIndex(T* pointer);
  int Add(T* pointer);
  void Clear();

private:
  TreeMap<T*, int> index_by_pointer_;
  Array<T*> pointer_by_index_;
};

template<class T>
T* PointerIndexMap<T>::GetPointer(int index) {
  return pointer_by_index_(index);
}

template<class T>
int PointerIndexMap<T>::GetIndex(T* pointer) {
  return index_by_pointer_(pointer);
}

template<class T>
int PointerIndexMap<T>::Add(T* pointer) {
  typename TreeMap<T*, int>::Iterator it = index_by_pointer_.Find(pointer);
  if (it != index_by_pointer_.End()) {
    return it.Value();
  }
  pointer_by_index_.PushBack(pointer);
  int idx = pointer_by_index_.Count()-1;
  index_by_pointer_.Insert(pointer, idx);
  return idx;
}

template<class T>
void PointerIndexMap<T>::Clear() {
  index_by_pointer_.Clear();
  pointer_by_index_.Clear();
}

} // namespace dg
