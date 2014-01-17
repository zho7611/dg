// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class FirstType, class SecondType>
class Pair {
public:
  Pair() { }
  Pair(FirstType& first, SecondType& second) :
    first_(first), second_(second) { }

  typedef FirstType FirstType;
  typedef SecondType SecondType;
  FirstType first_;
  SecondType second_;
};

template<class FirstType, class SecondType>
Pair<FirstType, SecondType> MakePair(FirstType& first, SecondType& second) {
  return Pair<FirstType, SecondType>(first, second);
}

} // namespace dg
