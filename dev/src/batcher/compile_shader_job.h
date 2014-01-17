// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace batcher {

class CompileShaderJob : public BatchJob {
public:
  virtual void Usage();
  virtual bool Run(CommandLine* command_line);
};

} // namespace batcher
} // namespace dg
