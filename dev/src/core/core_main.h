// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

extern class ResourceManager* g_resource_manager;
extern class Config* g_config;
extern class Logger* g_logger;
extern class NamePool* g_name_pool;

void CoreInit();

void CoreClear();

} // namespace dg
