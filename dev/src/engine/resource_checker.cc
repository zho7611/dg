// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

ResourceChecker::ResourceChecker() :
  creation_index_(0),
  is_loggin_(false) {
}

ResourceChecker::~ResourceChecker() {
  Check(CheckClear());
}

bool ResourceChecker::CheckClear() {
  ResourcesType::Iterator it = resources_.Begin();
  ResourcesType::Iterator ie = resources_.End();
  for (; it != ie; ++it) {
    Resource* resource = it.Key();
    ResourceCheckDesc& desc = it.Value();
    Check(resource);
    if (desc.reference_count != 0) {
      // Add break point at #resource-check-reference-count with the desc.creation_index_
      LogErrorCode(ErrorCode_ReferenceCountNotCleared);
    }
    Check(desc.reference_count == 0);
    if (desc.reference_count != 0) {
      return false;
    }
  }
  return true;
}

void ResourceChecker::AddResource(Resource* resource) {
#ifdef DG_DEBUG
  static int s_creation_index_for_debug = -1;
  if (s_creation_index_for_debug == creation_index_) {
    // #resource-check-reference-count
    creation_index_ = creation_index_;
  }
#endif
  Check(resource);
  if (resources_.IsFound(resource)) {
    resources_.Get(resource).reference_count++;
  } else {
    resources_.Get(resource).reference_count = 1;
    resources_.Get(resource).creation_index = creation_index_++;
  }
  if (is_loggin_) {
    DG_LOG_LINE(
        TXT("resourcechecker.addresource: resource:0x%x refcount:%d creationindex:%d"),
        resource,
        resources_.GetConst(resource).reference_count,
        resources_.GetConst(resource).creation_index);
  }
}

void ResourceChecker::RemoveResource(Resource* resource) {
  Check(resource);
  Check(resources_.IsFound(resource));
  if (is_loggin_) {
    DG_LOG_LINE(
        TXT("resourcechecker.deleteresource: resource:0x%x refcount:%d creationindex:%d"),
        resource,
        resources_.GetConst(resource).reference_count,
        resources_.GetConst(resource).creation_index);
  }
  ResourcesType::Iterator it = resources_.Find(resource);
  ResourcesType::Iterator ie = resources_.End();
  if (it != ie) {
    ResourceCheckDesc& desc = it.Value();
    desc.reference_count--;
    Check(desc.reference_count >= 0);
    if (desc.reference_count == 0) {
      resources_.Erase(it);
    }
  }
}

void ResourceChecker::InitDeviceAll() {
  ResourcesType::Iterator it = resources_.Begin();
  ResourcesType::Iterator ie = resources_.End();
  for (int idx = 0; it != ie; ++it, ++idx) {
    Resource* resource = it.Key();
    Check(resource);
    resource->OnInitDevice();
    DG_LOG_LINE(
        TXT("resourcechecker.opendeviceall: idx:%d resource:0x%x refcount:%d creationindex:%d"),
        idx,
        resource,
        resources_.GetConst(resource).reference_count,
        resources_.GetConst(resource).creation_index);
  }
}

void ResourceChecker::ClearDeviceAll() {
  ResourcesType::Iterator it = resources_.Begin();
  ResourcesType::Iterator ie = resources_.End();
  DG_LOG_LINE(TXT("resourcechecker.cleardeviceresourceforall: count=%d"), resources_.Count());
  for (int idx = 0; it != ie; ++it, ++idx) {
    Resource* resource = it.Key();
    Check(resource);
    DG_LOG_LINE(
        TXT("resourcechecker.cleardeviceresourceforall: idx:%d resource:0x%x refcount:%d creationindex:%d"),
        idx,
        resource,
        resources_.GetConst(resource).reference_count,
        resources_.GetConst(resource).creation_index);
    resource->OnClearDevice();
  }
}

} // namespace dg
