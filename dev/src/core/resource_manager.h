// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum LoadResourceOption {
  LoadResourceOption_Read = 0,
  LoadResourceOption_Write = 1,
};

Resource* LoadResourceByClassName(const Cstr* class_name, const Cstr* path, LoadResourceOption option);

// Load resource with features: caching, package and serialization
template<class T>
T* LoadResource(const Cstr* path, LoadResourceOption option = LoadResourceOption_Read) {
  ResourcePath resource_path(path);
  path = resource_path.Get();
  return static_cast<T*>(LoadResourceByClassName(T::GetStaticClassNameStr(), path, option));
}

template<>
Chunk* LoadResource<Chunk>(const Cstr* path, LoadResourceOption option);
bool SaveResource(Resource* resource, const Cstr* path = NULL);
bool CloseResource(Resource* resource);

class ResourceManager {
public:
  typedef Resource* (*ResourceCreatorType)();
  StringMap<ResourceCreatorType> class_creators_;
  StringMap<Ptr<Resource> > open_resources_;

  void Init();
  void Clear();
  Resource* FindResource(const Cstr* path);
  bool OpenResource(Resource* resource);
  bool CloseResource(Resource* resource);
  void RegisterClassCreator(const Cstr* class_name, ResourceCreatorType creator);
  virtual void RegisterResourceClasses();
};

// Create memory block to read from the file path
Chunk* CreateReadChunk(const Cstr* path);
// Create memory block to write to the file path
// It actually write when the chunk is released
Chunk* CreateWriteChunk(const Cstr* path);

#define DG_REGISTER_CLASS(ClassName) \
g_resource_manager->RegisterClassCreator(ClassName::GetStaticClassNameStr(), &ClassName::CreateResource);

} // namespace dg
