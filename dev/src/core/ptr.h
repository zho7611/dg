// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Pointer wrapper that only has the constructor with default null value
template<class T>
class InitPtr {
public:
  InitPtr(T* ptr = NULL) : ptr_(ptr) {}
  T* ptr() const {
    return ptr_;
  }
  T*& ref() {
    return ptr_;
  }
  T& operator*() const {
    Check(ptr_);
    return *ptr_;
  }
  T* operator->() const {
    Check(ptr_);
    return ptr_;
  }

private:
  T* ptr_;
};

template<class T>
class Ptr {
public:
  Ptr(T* ptr = NULL, bool is_add_ref = true) : ptr_(ptr) {
    if (is_add_ref) {
      DG_SAFE_ADDREF(ptr_);
    }
  }
  Ptr(const Ptr& rhs) {
    DG_SAFE_ADDREF(rhs.ptr_);
    ptr_ = rhs.ptr_;
  }
  template<class Y> Ptr(const Ptr<Y>& rhs) {
    DG_SAFE_ADDREF(rhs.ptr_);
    ptr_ = static_cast<T*>(rhs.ptr_);
  }
  ~Ptr() {
    DG_SAFE_RELEASE(ptr_);
  }
  T* ptr() const { return ptr_; }
  T*& ref() { return ptr_; }
  Ptr& operator=(const Ptr& rhs) {
    DG_SAFE_ADDREF(rhs.ptr_);
    DG_SAFE_RELEASE(ptr_);
    ptr_ = rhs.ptr_;
    return *this;
  }
  Ptr& operator=(T* rhs) {
    DG_SAFE_ADDREF(rhs);
    DG_SAFE_RELEASE(ptr_);
    ptr_ = rhs;
    return *this;
  }
  template<class Y>
  Ptr& operator=(const Ptr<Y>& rhs) {
    DG_SAFE_ADDREF(rhs.ptr_);
    DG_SAFE_RELEASE(ptr_);
    ptr_ = static_cast<T*>(rhs.ptr_);
    return *this;
  }
  T& operator*() const { Check(ptr_); return *ptr_; }
  T* operator->() const { Check(ptr_); return ptr_; }
  bool operator==(Ptr& rhs) const { return (ptr_ == rhs.ptr_); }
  bool operator!=(Ptr& rhs) const { return (ptr_ != rhs.ptr_); }
  template<class Y>
  bool operator==(Ptr<Y>& rhs) const { return (ptr_ == rhs.ptr_); }
  template<class Y>
  bool operator!=(Ptr<Y>& rhs) const { return (ptr_ != rhs.ptr_); }
  // Unspecified bool type not to use 'operator bool()', which causes
  // Unintended bool-to-int implicit conversion
  // The idea comes from the 'boost::smart_ptr'
  // #unspecified_bool_type
  typedef T* Ptr<T>::*UnspecifiedBoolType;
  operator UnspecifiedBoolType() const {
    return ptr_ == 0 ? 0 : &Ptr<T>::ptr_;
  }

private:
  template<class Y> friend class Ptr;
  T* ptr_;
};

} // namespace dg
