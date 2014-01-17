// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ResourceChecker {
public:
  ResourceChecker();
  ~ResourceChecker();
  void AddResource(Resource* resource);
  void RemoveResource(Resource* resource);
  void InitDeviceAll();
  void ClearDeviceAll();

private:
  struct ResourceCheckDesc {
    int reference_count;
    int creation_index;
    ResourceCheckDesc() : reference_count(0), creation_index(-1) {}
  };
  typedef HashMap<Resource*, ResourceCheckDesc, PointerHashFunc<Resource> > ResourcesType;

  bool CheckClear();

  bool is_loggin_;
  ResourcesType resources_;
  int creation_index_;
};

} // namespace dg
