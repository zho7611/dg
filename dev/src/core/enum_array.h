// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Act like an FixedArray, but indexed by enum type rather than int
template<class EnumType, class ItemType>
class EnumArray : public FixedArray<ItemType> {
public:
  EnumArray();
  EnumArray(int size);
  EnumArray(int size, ItemType initial_value);
  EnumArray(const EnumArray<EnumType, ItemType>& rhs);
  ItemType& Get(EnumType index);
  const ItemType& GetConst(EnumType index) const;
};

template<class EnumType, class ItemType>
EnumArray<EnumType, ItemType>::EnumArray() :
  FixedArray<ItemType>() {}

template<class EnumType, class ItemType>
EnumArray<EnumType, ItemType>::EnumArray(int size) :
  FixedArray<ItemType>(size) {}

template<class EnumType, class ItemType>
EnumArray<EnumType, ItemType>::EnumArray(int size, ItemType initial_value) :
  FixedArray<ItemType>(size, initial_value) {}

template<class EnumType, class ItemType>
EnumArray<EnumType, ItemType>::EnumArray(const EnumArray<EnumType, ItemType>& rhs) :
  FixedArray<ItemType>(rhs) {}

template<class EnumType, class ItemType>
ItemType& EnumArray<EnumType, ItemType>::Get(EnumType index) {
  return FixedArray<ItemType>::Get(static_cast<int>(index));
}

template<class EnumType, class ItemType>
const ItemType& EnumArray<EnumType, ItemType>::GetConst(EnumType index) const {
  return FixedArray<ItemType>::GetConst(static_cast<int>(index));
}

} // namespace dg
