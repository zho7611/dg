// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class ArrayType>
class ArrayIterator {
public:
  ArrayIterator(ArrayType* items, int index = -1) : index_(index), items_(items) {}
  ArrayIterator(const ArrayIterator& rhs) : index_(rhs.index_), items_(rhs.items_) {}
  ~ArrayIterator() {}
  ArrayIterator& operator=(const ArrayIterator& rhs) {
    index_ = rhs.index_;
    items_ = rhs.items_;
    return *this;
  }
  // Pre-increment
  ArrayIterator& operator++() {
    ++index_;
    if (index_ >= items_->Count()) index_ = -1;
    return *this;
  }
  // Post-increment
  ArrayIterator operator++(int) {
    ArrayIterator saved = *this;
    ++index_;
    if (index_ >= items_->Count()) index_ = -1;
    return saved;
  }
  // Pre-decrement
  ArrayIterator& operator--() {
    --index_;
    if (index_ < 0) index_ = -1;
    return *this;
  }
  // Post-decrement
  ArrayIterator operator--(int) {
    ArrayIterator saved = *this;
    --index_;
    if (index_ < 0) index_ = -1;
    return saved;
  }
  bool operator!=(ArrayIterator& it) {
    return (it.index_ != index_);
  }
  bool operator==(ArrayIterator& it) {
    return (it.index_ == index_);
  }
  typename ArrayType::ValueType& operator*() {
    return Value();
  }
  const typename ArrayType::ValueType& operator*() const {
    return Value();
  }
  typename ArrayType::ValueType& Value() {
    Check(index_ >= 0);
    return items_->Get(index_);
  }
  typename ArrayType::ValueType& Value() const {
    Check(index_ >= 0);
    return items_->GetConst(index_);
  }
  int GetIndex() const {
    return index_;
  }

private:
  int index_;
  ArrayType* items_;
};

} // namespace dg
