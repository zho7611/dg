// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
class ScopedBase {
public:
  ScopedBase(T* ptr = 0) : ptr_(ptr) {}
  // Reference operator
  T& operator*() const {
    Check(ptr_);
    return *ptr_;
  }
  // Pointer operator
  T* operator->() const {
    Check(ptr_);
    return ptr_;
  }
  // Get the pointer
  T* ptr() const {
    Check(ptr_);
    return ptr_;
  }
  // Get the pointer reference
  T*& ref() {
    return ptr_;
  }
  // #unspecified_bool_type
  typedef T* ScopedBase<T>::*UnspecifiedBoolType;
  operator UnspecifiedBoolType() const {
    return ptr_ == 0 ? 0 : &ScopedBase<T>::ptr_;
  }

protected:
  T* ptr_;
};

template<class T>
class ScopedDelete : public ScopedBase<T> {
public:
  ScopedDelete(T* ptr = 0) : ScopedBase<T>(ptr) {}
  ScopedDelete& operator=(T* ptr) {
    DG_SAFE_DELETE(this->ptr_);
    this->ptr_ = ptr;
    return *this;
  }
  ~ScopedDelete() {
    DG_SAFE_DELETE(this->ptr_);
  }
};

template<class T>
class ScopedClose : public ScopedBase<T> {
public:
  ScopedClose(T* ptr = 0) : ScopedBase<T>(ptr) {}
  ScopedClose& operator=(T* ptr) {
    if (this->ptr_) {
      this->ptr_->Close();
    }
    this->ptr_ = ptr;
    return *this;
  }
  ~ScopedClose() {
    if (this->ptr_) {
      this->ptr_->Close();
    }
  }
};

template<class T>
class ScopedRelease : public ScopedBase<T> {
public:
  ScopedRelease(T* ptr = 0) : ScopedBase<T>(ptr) {}
  ScopedRelease& operator=(T* ptr) {
    DG_SAFE_RELEASE(this->ptr_);
    this->ptr_ = ptr;
    return *this;
  }
  ~ScopedRelease() {
    DG_SAFE_RELEASE(this->ptr_);
  }
};

template<class T>
class ScopedEnter {
public:
  ScopedEnter(T& target) : target_(&target) {
    target_->Enter();
  }
  ~ScopedEnter() {
    target_->Leave();
  }

protected:
  T* target_;
};

} // namespace dg
