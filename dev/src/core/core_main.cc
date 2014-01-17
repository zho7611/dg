// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Config* g_config = NULL;
ResourceManager* g_resource_manager = NULL;
Logger* g_logger = NULL;
NamePool* g_name_pool = NULL;

void CoreInit() {
  Check(g_resource_manager == NULL);
  g_resource_manager = new ResourceManager;
  g_logger = new Logger;
  g_name_pool = new NamePool;
}

void CoreClear() {
  DG_SAFE_DELETE(g_name_pool);
  DG_SAFE_DELETE(g_logger);
  DG_SAFE_DELETE(g_resource_manager);
}

} // namespace dg
