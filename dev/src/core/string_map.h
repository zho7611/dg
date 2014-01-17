// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
class StringMap {
  friend class Chunk;
public:
  typedef typename TreeMap<String, T>::Iterator Iterator;

  StringMap() {}
  T GetCopy(const String& key);
  T GetCopy(const Cstr* key);
  T& Get(const String& key);
  const T& GetConst(const String& key) const;
  Iterator Add(const String& key, T& value);
  Iterator Add(const Cstr* key, T& value);
  bool Erase(const Iterator it);
  bool Remove(const String& key);
  bool Remove(const Cstr* key);
  bool IsFound(const String& key) const;
  bool IsFound(const Cstr* key) const;
  Iterator Find(const Cstr* key);
  Iterator Find(const String& key);
  bool Find(const String& key, T& value);
  bool Find(const Cstr* key, T& value);
  T FindOrNull(const String& key);
  T FindOrNull(const Cstr* key);
  void Clear();
  Iterator Begin();
  Iterator End();
  int Count()const;
  bool IsEmpty() const;

private:
  TreeMap<String, T> map_;
};

template<class T>
bool StringMap<T>::Erase(const Iterator it) {
  return map_.Erase(it);
}

// Returns the iterator which points to the added item
template<class T>
typename StringMap<T>::Iterator StringMap<T>::Add(const String& key, T& value) {
  Check(!key.IsEmpty());
  return map_.Insert(key, value);
}

// Returns the iterator which points to the added item
template<class T>
typename StringMap<T>::Iterator StringMap<T>::Add(const Cstr* key, T& value) {
  Check(key);
  return map_.Insert(key, value);
}

template<class T>
bool StringMap<T>::Remove(const String& key) {
  Check(!key.IsEmpty());
  return map_.Remove(key);
}

template<class T>
bool StringMap<T>::Remove(const Cstr* key) {
  Check(key);
  return map_.Remove(key);
}

template<class T>
bool StringMap<T>::IsFound(const String& key) const {
  Check(!key.IsEmpty());
  return (map_.Find(key) != map_.End());
}

template<class T>
bool StringMap<T>::IsFound(const Cstr* key) const {
  Check(key);
  return (map_.Find(key) != map_.End());
}

template<class T>
typename StringMap<T>::Iterator StringMap<T>::Find(const Cstr* key) {
  Check(key);
  String keyStr(key);
  Check(!keyStr.IsEmpty());
  return map_.Find(keyStr);
}

template<class T>
typename StringMap<T>::Iterator StringMap<T>::Find(const String& key) {
  Check(!key.IsEmpty());
  return map_.Find(key);
}

template<class T>
bool StringMap<T>::Find(const String& key, T& value) {
  Check(!key.IsEmpty());
  Iterator it = map_.Find(key);
  if (it == map_.End()) {
    return false;
  }
  value = it.Value();
  return true;
}

template<class T>
bool StringMap<T>::Find(const Cstr* key, T& value) {
  Check(key);
  Iterator it = map_.Find(key);
  if (it == map_.End()) {
    return false;
  }
  value = it.Value();
  return true;
}

template<class T>
T StringMap<T>::GetCopy(const String& key) {
  Check(!key.IsEmpty());
  Check(IsFound(key));
  T value;
  Find(key, value);
  return value;
}

template<class T>
T StringMap<T>::GetCopy(const Cstr* key) {
  Check(key);
  Check(IsFound(key));
  T value;
  Find(key, value);
  return value;
}

template<class T>
void StringMap<T>::Clear() {
  map_.Clear();
}

template<class T>
typename StringMap<T>::Iterator StringMap<T>::Begin() {
  return map_.Begin();
}

template<class T>
typename StringMap<T>::Iterator StringMap<T>::End() {
  return map_.End();
}

template<class T>
bool StringMap<T>::IsEmpty() const {
  return map_.IsEmpty();
}

template<class T>
T& StringMap<T>::Get(const String& key) {
  Check(!key.IsEmpty());
  return map_.Get(key);
}

template<class T>
const T& StringMap<T>::GetConst(const String& key) const {
  Check(!key.IsEmpty());
  return map_.GetConst(key);
}

template<class T>
int StringMap<T>::Count() const {
  return map_.Count();
}

template<class T>
T StringMap<T>::FindOrNull(const String& key) {
  Check(!key.IsEmpty());
  T value;
  if (Find(key, value)) {
    return value;
  }
  return NULL;
}

template<class T>
T StringMap<T>::FindOrNull(const Cstr* key) {
  Check(key);
  T value;
  if (Find(key, value)) {
    return value;
  }
  return NULL;
}

} // namespace dg
