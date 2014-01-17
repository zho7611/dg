// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace batcher {

class PackagingJob : public BatchJob {
public:
  void Usage() {
    String message;
    message.AddRight(TXT("Batcher.exe -packer [-l | -x | -d | -a] [PackageName] ([ChunkName])\n"));
    message.AddRight(TXT("\n"));
    message.AddRight(TXT("\t PackageName: the package path\n"));
    message.AddRight(TXT("\t ChunkName: the chunk name in the package\n"));
    message.AddRight(TXT("\n"));
    message.AddRight(TXT("\t -l : List package chunks\n"));
    message.AddRight(TXT("\t -x : Extract a chunk\n"));
    message.AddRight(TXT("\t -d : Delete a chunk\n"));
    message.AddRight(TXT("\t -a : Add a chunk\n"));
    message.AddRight(TXT("\t -t : Unit test. No other parameters are needed\n"));
    g_console.Print(message.Get());
  }

  void List(Package& package) {
    Package::ChunksIter it = package.chunks_.Begin();
    Package::ChunksIter ie = package.chunks_.End();
    for (; it != ie; ++it) {
      g_console.PrintFormat(TXT("\"%s\", %d Bytes\n"),
          (*it).first_.GetConst(),
          (*it).second_->size());
    }
  }

  void Extract(Package& package, const Cstr* chunk_name) {
    ResourcePath resource_path(package.GetResourcePath().Get(), chunk_name);
    Ptr<Chunk> chunk = CreateReadChunk(resource_path.Get());
    File file(chunk_name, File::kWrite);
    file.StreamWriteFlatMultiple(chunk->data(), chunk->size());
  }

  void Delete(Package& package, const Cstr* chunk_name) {
    package.RemoveChunk(chunk_name);
  }

  void Add(Package& package, const Cstr* chunk_path, const Cstr* chunk_name) {
    File file(chunk_path, File::kRead);
    file.PrepareBuffer();
    Ptr<Chunk> chunk = package.OpenChunk(chunk_name, Package::kWrite);
    if (chunk) {
      chunk->CopyFrom(file.data(), file.size());
    }
  }

  bool UnitTest() {
    bool is_ok = true;
    Ptr<Package> package = LoadResource<Package>(TXT("dev/res/test/result/test.pak"), LoadResourceOption_Write);
    PackagingJob::Add(*package, TXT("dev/res/test/zhologo.png"), TXT("logo.png"));
    PackagingJob::Extract(*package, TXT("logo.png"));
    PackagingJob::Delete(*package, TXT("logo.png"));
    if (!File::DeleteFile(TXT("logo.png"))) {
      is_ok = false;
    }
    // No need to save the package, but need to set un-dirty
    package->resource_state().SetDirty(false);
    return is_ok;
  }

  bool Run(CommandLine* command_line) {
    if (!command_line) {
      return false;
    }
    if (command_line->NumOptions() <= 2) {
      Usage();
      return false;
    }
    // Skip first two options: 1. file_name 2: packer option itself
    for (int idx = 2; idx < command_line->NumOptions(); ++idx) {
      const String option = command_line->GetOption(idx);
      String next_option;
      if ((idx + 1) < command_line->NumOptions()) {
        next_option = command_line->GetOption(idx+1);
      }
      if (option.IsEqual(TXT("-l"))) {
        if (next_option.IsEmpty()) {
          PackagingJob::Usage();
          return false;
        }
        Ptr<Package> package(LoadResource<Package>(next_option.Get()));
        PackagingJob::List(*package);
      } else if (option.IsEqual(TXT("-x"))) {
        if (next_option.IsEmpty()) {
          PackagingJob::Usage();
          return false;
        }
        Ptr<Package> package(LoadResource<Package>(next_option.Get()));
        PackagingJob::Extract(*package, next_option.Get());
      } else if (option.IsEqual(TXT("-d"))) {
        if (next_option.IsEmpty()) {
          PackagingJob::Usage();
          return false;
        }
        Ptr<Package> package(LoadResource<Package>(next_option.Get()));
        PackagingJob::Delete(*package, next_option.Get());
        SaveResource(package.ptr());
      } else if (option.IsEqual(TXT("-a"))) {
        if (next_option.IsEmpty()) {
          PackagingJob::Usage();
          return false;
        }
        Ptr<Package> package(LoadResource<Package>(next_option.Get()));
        const Cstr* chunk_name = next_option.Get();
        PackagingJob::Add(*package, chunk_name, chunk_name);
        SaveResource(package.ptr());
      } else if (option.IsEqual(TXT("-t"))) {
        if (!PackagingJob::UnitTest()) {
          return false;
        }
        DG_LOG_LINE(TXT("packer.unittest.ok"));
      }
    }
    return true;
  }
};

} // namespace batcher
} // namespace dg
