// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class LeftType, class RightType>
class Bidirectional {
public:
  typedef typename TreeMap<LeftType, RightType>::Iterator LeftToRightIterator;
  typedef typename TreeMap<RightType, LeftType>::Iterator RightToLeftIterator;

  bool Remove(const LeftType& left);
  bool Remove(const RightType& right);
  void Insert(const LeftType& left, const RightType& right);
  void Reset(const LeftType& left, const RightType& right);
  const RightType& GetConst(const LeftType& key) const;
  const LeftType& GetConst(const RightType& key) const;
  bool IsFound(const LeftType& key) const;
  bool IsFound(const RightType& key) const;
  LeftToRightIterator Find(const LeftType& key) const;
  RightToLeftIterator Find(const RightType& key) const;
  LeftToRightIterator Begin() const;
  RightToLeftIterator BeginRight() const;
  LeftToRightIterator End() const;
  RightToLeftIterator EndRight() const;
  int Count() const {
    return left_to_right_.Count();
  }

private:
  TreeMap<LeftType, RightType> left_to_right_;
  TreeMap<RightType, LeftType> right_to_left_;
};

template<class LeftType, class RightType>
bool Bidirectional<LeftType, RightType>::IsFound(const LeftType& key) const {
  return left_to_right_.IsFound(key);
}

template<class LeftType, class RightType>
bool Bidirectional<LeftType, RightType>::IsFound(const RightType& key) const {
  return right_to_left_.IsFound(key);
}

template<class LeftType, class RightType>
void Bidirectional<LeftType, RightType>::Insert(const LeftType& left, const RightType& right) {
  left_to_right_.Insert(left, right);
  right_to_left_.Insert(right, left);
}

template<class LeftType, class RightType>
const RightType& Bidirectional<LeftType, RightType>::GetConst(const LeftType& key) const {
  return left_to_right_.GetConst(key);
}

template<class LeftType, class RightType>
const LeftType& Bidirectional<LeftType, RightType>::GetConst(const RightType& key) const {
  return right_to_left_.GetConst(key);
}

template<class LeftType, class RightType>
bool Bidirectional<LeftType, RightType>::Remove(const LeftType& left) {
  LeftToRightIterator it = left_to_right_.Find(left);
  if (it != left_to_right_.End()) {
    return right_to_left_.Remove(it.Value()) && left_to_right_.Remove(left);
  }
  return left_to_right_.Remove(left);
}

template<class LeftType, class RightType>
bool Bidirectional<LeftType, RightType>::Remove(const RightType& right) {
  RightToLeftIterator it = right_to_left_.Find(right);
  if (it != right_to_left_.End()) {
    return left_to_right_.Remove(it.Value()) && right_to_left_.Remove(right);
  }
  return right_to_left_.Remove(right);
}

template<class LeftType, class RightType>
void Bidirectional<LeftType, RightType>::Reset(const LeftType& left, const RightType& right) {
  Remove(left);
  Remove(right);
  Insert(left, right);
}

template<class LeftType, class RightType>
typename Bidirectional<LeftType, RightType>::LeftToRightIterator
Bidirectional<LeftType, RightType>::Find(const LeftType& key) const {
  return left_to_right_.Find(key);
}

template<class LeftType, class RightType>
typename Bidirectional<LeftType, RightType>::RightToLeftIterator
Bidirectional<LeftType, RightType>::Find(const RightType& key) const {
  return right_to_left_.Find(key);
}

template<class LeftType, class RightType>
typename Bidirectional<LeftType, RightType>::LeftToRightIterator
Bidirectional<LeftType, RightType>::Begin() const {
  return left_to_right_.Begin();
}

template<class LeftType, class RightType>
typename Bidirectional<LeftType, RightType>::RightToLeftIterator
Bidirectional<LeftType, RightType>::BeginRight() const {
  return right_to_left_.Begin();
}

template<class LeftType, class RightType>
typename Bidirectional<LeftType, RightType>::LeftToRightIterator
Bidirectional<LeftType, RightType>::End() const {
  return left_to_right_.End();
}

template<class LeftType, class RightType>
typename Bidirectional<LeftType, RightType>::RightToLeftIterator
Bidirectional<LeftType, RightType>::EndRight() const {
  return right_to_left_.End();
}

} // namespace dg
