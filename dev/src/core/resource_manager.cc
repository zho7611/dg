// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void ResourceManager::RegisterResourceClasses() {
  DG_REGISTER_CLASS(Package);
}

void ResourceManager::Init() {
  RegisterResourceClasses();
}

void ResourceManager::Clear() {
  open_resources_.Clear();
}

// A resource loaded by the LoadResource() can be closed by CloseResource()
// If not, it will be closed automatically when the system exits
// If you want to manually close the resource, then call CloseResource. You should
// Be careful about the reference counting of the resource you are about to close
template<>
Chunk* LoadResource<Chunk>(const Cstr* path, LoadResourceOption option) {
  ResourcePath resource_path(path);
  path = resource_path.Get();
  if (option == LoadResourceOption_Read) {
    Chunk* chunk = CreateReadChunk(path);
    return chunk;
  } else if (option == LoadResourceOption_Write) {
    Chunk* chunk = CreateWriteChunk(path);
    return chunk;
  }
  return 0;
}

// Utility function that creates an instance of the class, read the chunk and add it to the OpenPacagke list
Resource* LoadResourceByClassName(const Cstr* class_name, const Cstr* path, LoadResourceOption option) {
  ResourcePath resource_path(path);
  path = resource_path.Get();
  Check(g_resource_manager->class_creators_.IsFound(class_name));
  Resource* resource = g_resource_manager->FindResource(path);
  if (resource) {
    // It's already opened
    return resource;
  }
  ResourceManager::ResourceCreatorType creator = g_resource_manager->class_creators_.Get(class_name);
  resource = creator();
  resource->set_resource_name(path);
  if (option == LoadResourceOption_Read) {
    Ptr<Chunk> chunk = CreateReadChunk(path);
    if (chunk && !chunk->IsEnd()) {
      if (0) { // Enable when you need this for debugging
        DG_LOG_LINE(TXT("resourcemanager.loadresourcebyclassname: read-resource:%s"), path);
      }
      resource->ReadFrom(chunk.ptr());
    }
  }
  g_resource_manager->OpenResource(resource);
  return resource;
}

bool SaveResource(Resource* resource, const Cstr* path) {
  ResourcePath resource_path(path);
  if (!path) {
    path = resource->GetResourcePath().Get();
  } else {
    path = resource_path.Get();
  }
  Ptr<Chunk> chunk = CreateWriteChunk(path);
  resource->WriteTo(chunk.ptr());
  resource->resource_state().SetDirty(false);
  return true;
}

bool CloseResource(Resource* resource) {
  return g_resource_manager->CloseResource(resource);
}

Resource* ResourceManager::FindResource(const Cstr* path) {
  ResourcePath resource_path(path);
  path = resource_path.Get();
  Ptr<Resource> found = open_resources_.FindOrNull(path);
  return found.ptr();
}

bool ResourceManager::OpenResource(Resource* resource) {
  const Cstr* resource_path_str = resource->GetResourcePath().Get();
  if (!open_resources_.IsFound(resource_path_str)) {
    String path(resource_path_str);
    Ptr<Resource> ptr(resource);
    open_resources_.Add(path, ptr);
  }
  return true;
}

bool ResourceManager::CloseResource(Resource* resource) {
  const Cstr* resource_path_str = resource->GetResourcePath().Get();
  return open_resources_.Remove(resource_path_str);
}

void ResourceManager::RegisterClassCreator(const Cstr* class_name, ResourceCreatorType creator) {
  Check(!class_creators_.IsFound(class_name));
  class_creators_.Add(class_name, creator);
}

Chunk* CreateReadChunk(const Cstr* path) {
  ResourcePath resource_path(path);
  switch (resource_path.GetResourcePathType()) {
  case ResourcePathType_PackageFile:
    return PackageChunk::CreateReadChunk(resource_path.Get());
  case ResourcePathType_File:
    return FileChunk::CreateReadChunk(resource_path.Get());
  }
  return NULL;
}

Chunk* CreateWriteChunk(const Cstr* path) {
  ResourcePath resource_path(path);
  switch (resource_path.GetResourcePathType()) {
  case ResourcePathType_PackageFile:
    return PackageChunk::CreateWriteChunk(resource_path.Get());
  case ResourcePathType_File:
    return FileChunk::CreateWriteChunk(resource_path.Get());
  }
  return NULL;
}

} // namespace dg
