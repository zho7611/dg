// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ShaderCompiler {
public:
  static bool CompileShader(
      ShaderProfile profile,
      const Cstr* include_base_dir,
      const BlobBase* source_blob,
      Shader* out_shader);
};

} // namespace dg
