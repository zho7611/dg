// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
class Stack {
public:
  ~Stack();
  void Push(T& item);
  void Pop();
  bool PopAndGet(T& item);
  bool Get(T& item) const;
  T& Last();
  const T& Last() const;
  void Clear();

private:
  Array<T> items_;
};

template<class T>
Stack<T>::~Stack() {
  Check(items_.IsEmpty());
}

template<class T>
void Stack<T>::Push(T& item) {
  items_.PushBack(item);
}

template<class T>
void Stack<T>::Pop(void) {
  items_.PopBack();
}

template<class T>
bool Stack<T>::PopAndGet(T& item) {
  typename Array<T>::Iterator it = items_.Last();
  if (it != items_.End()) {
    item = *it;
    items_.PopBack();
    return true;
  }
  return false;
}

template<class T>
bool Stack<T>::Get(T& item) const {
  typename Array<T>::Iterator it = items_.Last();
  if (it != items_.End()) {
    item = *it;
    return true;
  }
  return false;
}

template<class T>
T& Stack<T>::Last() {
  return items_.Last();
}

template<class T>
const T& Stack<T>::Last() const {
  return items_.Last();
}

template<class T>
void Stack<T>::Clear() {
  items_.Clear();
}

} // namespace dg
