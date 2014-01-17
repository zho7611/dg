// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ResourcePath;
typedef unsigned int ClassHash;

// Resource = serialize-able and ref-counted asset
class Resource : public Serialize, public RefCount {
public:
  virtual ~Resource();
  virtual void Release() {
    if (--ref_count_ == 0) {
      OnRelease();
      delete this;
    }
  }
  // This is called when Release() is called.
  // You can override this when you need more than just UnbindResource()
  virtual void OnRelease() { UnbindResource(); }
  // Create the device resources here. Returns true if it is successfully created.
  virtual bool OnInitDevice() { return true; }
  // Delete the device resources here. Returns true if it is successfully deleted.
  virtual bool OnClearDevice() { return true; }
  virtual bool InitResource() { return OnInitDevice(); }
  virtual bool ClearResource() { return OnClearDevice(); }
  int GetResourceVersion() const;
  bool GetResourceStateFilled() const;
  virtual void BindResource();
  virtual void UnbindResource();
  virtual bool SerializeToDevice();
  virtual bool SerializeFromDevice();
  virtual void OnReadDone() { resource_state_.SetLoaded(true); }
  virtual void OnWriteDone() {  }
  ResourceState& resource_state() { return resource_state_; }
  const ResourceState& resource_state() const { return resource_state_; }
  ResourcePath GetResourcePath() const;
  ClassHash GetClassNameHash();
  virtual const Cstr* GetClassNameStr() const { return TXT("Resource"); }
  const Cstr* resource_name() const {
    return resource_name_.GetConst();
  }
  void set_resource_name(const Cstr* name) {
    resource_name_ = name;
  }

  Ptr<Resource> resource_owner_;

protected:
  Resource();

  String resource_name_;
  ResourceState resource_state_;
};

template<class T> T* CreateClass() {
  return new T;
}

inline int Resource::GetResourceVersion() const {
  return 0;
}

#define DG_DECLARE_RESOURCE(ClassName, SuperClassName) \
public: \
typedef SuperClassName Super; \
static const Cstr* GetStaticClassNameStr() { return TXT(#ClassName); } \
virtual const Cstr* GetClassNameStr() const { return GetStaticClassNameStr(); } \
static Resource* CreateResource() { \
  return CreateClass<ClassName>(); \
}

template<class TargetType, class SrcType>
TargetType* SafeCast(Ptr<SrcType>& pointer) {
  if (TargetType::GetStaticClassNameStr() == pointer->GetClassNameStr()) {
    return static_cast<TargetType*>(pointer.ptr());
  } else {
    return NULL;
  }
}

} // namespace dg
