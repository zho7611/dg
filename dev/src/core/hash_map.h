// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

struct Uint32HashFunc {
  unsigned int operator()(unsigned int key) const {
    return GetUintHashCode(key);
  }
};

struct CstringHashFunc {
  unsigned int operator()(const Cstr* key) const {
    return GetStringHashCode(key);
  }
};

template<class T>
struct PtrHashFunc {
  unsigned int operator()(const Ptr<T>& key) const {
    return Uint32HashFunc()(static_cast<unsigned int>(key.Get()));
  }
};

template<class T>
struct PointerHashFunc {
  // It's not safe for 64-bit OS
  unsigned int operator()(const T* key) const {
    return Uint32HashFunc()(reinterpret_cast<int>(key) & 0xffff);
  }
};

template<class KeyType, class ValueType, class HashFuncType>
class HashMap {
private:
  struct Slot;

public:
  class Iterator {
  private:
    friend class HashMap;
    Slot* current_;

  public:
    Iterator() : current_(NULL) { }
    ~Iterator() { }
    Iterator& operator++() {
      if (current_)
        current_ = current_->next_list_;
      return *this;
    }
    Iterator& operator++(int idx) {
      if (current_)
        current_ = current_->next_list_;
      return *this;
    }
    Iterator& operator--() {
      if (current_)
        current_ = current_->prev_list_;
      return *this;
    }
    Iterator& operator--(int idx) {
      DG_UNREF(idx);
      if (current_)
        current_ = current_->prev_list_;
      return *this;
    }
    bool operator!=(Iterator& it) {
      return (it.current_ != current_);
    }
    bool operator==(Iterator& it) {
      return (it.current_ == current_);
    }
    ValueType operator->() {
      if (current_)
        return current_->value_;
      DG_UNREACHABLE();
      return ValueType();
    }
    ValueType operator*() {
      if (current_)
        return current_->value_;
      DG_UNREACHABLE();
      return ValueType();
    }
    KeyType& Key() {
      Check(current_);
      return current_->key_;
    }
    ValueType& Value() {
      Check(current_);
      return current_->value_;
    }
  };

  static const int kDefaultNumBuckets = 10;

  HashMap();
  HashMap(int num_buckets);
  ~HashMap();
  Iterator Insert(const KeyType& key, const ValueType& value);
  bool Remove(const KeyType& key);
  int Count() const;
  bool IsFound(const KeyType& key) const;
  Iterator Find(const KeyType& key) const;
  Iterator Begin() const;
  Iterator End() const;
  bool Erase(Iterator it);
  void Clear();
  bool IsEmpty() const;
  const ValueType& GetConst(const KeyType& key) const;
  ValueType& Get(const KeyType& key);

private:
  struct Slot {
    KeyType key_;
    ValueType value_;
    Slot* prev_;
    Slot* next_;
    Slot* prev_list_; // Doubly linked list for iterator traversal
    Slot* next_list_;

    Slot () : next_(0), prev_(0), prev_list_(0), next_list_(0) {}
    Slot(KeyType key, ValueType value) :
      key_(key), value_(value),
      prev_(0), next_(0), prev_list_(0), next_list_(0) {}
    ~Slot () {
      if (prev_) prev_->next_ = next_;
      if (next_) next_->prev_ = prev_;
      if (prev_list_) prev_list_->next_list_ = next_list_;
      if (next_list_) next_list_->prev_list_ = prev_list_;
    }
  };

  Slot** bucket_;
  int num_buckets_;
  Slot* head_;
  Slot* tail_;
  int num_slots_;

  Slot* FindSlot(const KeyType key) const;
  Slot* FindSlotByHashAndKey(unsigned int hashCode, const KeyType key) const;
  void Initialize();
  void Shutdown();
};

template<class KeyType, class ValueType, class HashFuncType>
HashMap<KeyType, ValueType, HashFuncType>::HashMap(void) :
  num_buckets_(kDefaultNumBuckets) {
  Initialize();
}

template<class KeyType, class ValueType, class HashFuncType>
HashMap<KeyType, ValueType, HashFuncType>::HashMap(int num_buckets) :
  num_buckets_(num_buckets) {
  Initialize();
}

template<class KeyType, class ValueType, class HashFuncType>
HashMap<KeyType, ValueType, HashFuncType>::~HashMap() {
  Shutdown();
}

template<class KeyType, class ValueType, class HashFuncType>
typename HashMap<KeyType, ValueType, HashFuncType>::Iterator
HashMap<KeyType, ValueType, HashFuncType>::Insert(const KeyType& key, const ValueType& value) {
  const unsigned int kHashCode = HashFuncType()(key) % num_buckets_;
  Check(!FindSlotByHashAndKey(kHashCode, key));
  Slot* last_slot = bucket_[kHashCode];
  // Make sure LastSlot is the last slot
  while (last_slot && last_slot->next_) {
    last_slot = last_slot->next_;
  }
  Slot* new_slot = new Slot(key, value);
  if (last_slot) {
    new_slot->prev_ = last_slot;
    last_slot->next_ = new_slot;
  } else {
    bucket_[kHashCode] = new_slot;
  }
  if (tail_) {
    new_slot->prev_list_ = tail_;
    tail_->next_list_ = new_slot;
    tail_ = new_slot;
  }
  if (!head_) {
    Check(!tail_);
    head_ = new_slot;
    tail_ = new_slot;
  }
  ++num_slots_;
  Iterator it;
  it.current_ = new_slot;
  return it;
}

template<class KeyType, class ValueType, class HashFuncType>
bool HashMap<KeyType, ValueType, HashFuncType>::Remove(const KeyType& key) {
  const unsigned int kHashCode = HashFuncType()(key) % num_buckets_;
  Slot* fount_slot = FindSlotByHashAndKey(kHashCode, key);
  if (!fount_slot)  {
    return false;
  }
  if (fount_slot->prev_) fount_slot->prev_->next_ = fount_slot->next_;
  // If it is the slot head, reset the slot head
  else bucket_[kHashCode] = fount_slot->next_;
  if (fount_slot->next_) fount_slot->next_->prev_ = fount_slot->prev_;
  if (fount_slot->prev_list_) fount_slot->prev_list_->next_list_ = fount_slot->next_list_;
  if (fount_slot->next_list_) fount_slot->next_list_->prev_list_ = fount_slot->prev_list_;
  if (fount_slot == head_) head_ = fount_slot->next_list_;
  if (fount_slot == tail_) tail_ = fount_slot->prev_list_;
  delete fount_slot;
  --num_slots_;
  return true;
}

template<class KeyType, class ValueType, class HashFuncType>
int HashMap<KeyType, ValueType, HashFuncType>::Count() const {
  return num_slots_;
}

template<class KeyType, class ValueType, class HashFuncType>
bool HashMap<KeyType, ValueType, HashFuncType>::IsFound(const KeyType& key) const {
  Slot* fount_slot = FindSlot(key);
  return (fount_slot != NULL);
}

template<class KeyType, class ValueType, class HashFuncType>
typename HashMap<KeyType, ValueType, HashFuncType>::Iterator
HashMap<KeyType, ValueType, HashFuncType>::Find(const KeyType& key) const {
  Slot* fount_slot = FindSlot(key);
  if (!fount_slot) return End();
  Iterator it;
  it.current_ = fount_slot;
  return it;
}

template<class KeyType, class ValueType, class HashFuncType>
typename HashMap<KeyType, ValueType, HashFuncType>::Iterator
HashMap<KeyType, ValueType, HashFuncType>::Begin() const {
  Iterator it;
  it.current_ = head_;
  return it;
}

template<class KeyType, class ValueType, class HashFuncType>
typename HashMap<KeyType, ValueType, HashFuncType>::Iterator
HashMap<KeyType, ValueType, HashFuncType>::End() const {
  Iterator it;
  it.current_ = NULL;
  return it;
}

template<class KeyType, class ValueType, class HashFuncType>
bool HashMap<KeyType, ValueType, HashFuncType>::Erase(Iterator it) {
  return Remove(it.current_->key_);
}

template<class KeyType, class ValueType, class HashFuncType>
void HashMap<KeyType, ValueType, HashFuncType>::Clear() {
  Iterator it = Begin();
  Iterator ie = End();
  while (it != ie) {
    Erase(it);
    it = Begin();
    ie = End();
  }
  Check(num_slots_ == 0);
  Check(!tail_ && !head_);
}

template<class KeyType, class ValueType, class HashFuncType>
bool HashMap<KeyType, ValueType, HashFuncType>::IsEmpty() const {
  return (Count() == 0);
}

template<class KeyType, class ValueType, class HashFuncType>
typename HashMap<KeyType, ValueType, HashFuncType>::Slot*
HashMap<KeyType, ValueType, HashFuncType>::FindSlot(const KeyType key) const {
  unsigned int hashCode = HashFuncType()(key) % num_buckets_;
  return FindSlotByHashAndKey(hashCode, key);
}

template<class KeyType, class ValueType, class HashFuncType>
typename HashMap<KeyType, ValueType, HashFuncType>::Slot*
HashMap<KeyType, ValueType, HashFuncType>::FindSlotByHashAndKey(unsigned int hashCode, const KeyType key) const {
  Slot* fount_slot = bucket_[hashCode];
  while (fount_slot) {
    if (fount_slot->key_ == key) {
      return fount_slot;
    }
    fount_slot = fount_slot->next_;
  }
  return NULL;
}

template<class KeyType, class ValueType, class HashFuncType>
void HashMap<KeyType, ValueType, HashFuncType>::Initialize() {
  head_ = NULL;
  tail_ = NULL;
  num_slots_ = 0;
  bucket_ = new Slot* [num_buckets_];
  MyMemSet(bucket_, 0, sizeof(Slot*)*num_buckets_);
}

template<class KeyType, class ValueType, class HashFuncType>
void HashMap<KeyType, ValueType, HashFuncType>::Shutdown() {
  Slot* current = head_;
  while (current) {
    Slot* deleteme = current;
    current = current->next_list_;
    if (deleteme) {
      delete deleteme;
    }
    --num_slots_;
  }
  Check(num_slots_ == 0);
  delete [] bucket_;
  bucket_ = NULL;
}

template<class KeyType, class ValueType, class HashFuncType>
const ValueType& HashMap<KeyType, ValueType, HashFuncType>::GetConst(const KeyType& key) const {
  Iterator it = Find(key);
  Check(it != End());
  return it.Value();
}

template<class KeyType, class ValueType, class HashFuncType>
ValueType& HashMap<KeyType, ValueType, HashFuncType>::Get(const KeyType& key) {
  Iterator it = Find(key);
  Iterator ie = End();
  if (it == ie) {
    ValueType temp;
    return Insert(key, temp).Value();
  }
  return it.Value();
}

} // namespace dg
