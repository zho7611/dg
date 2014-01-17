// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Package::Package() {
  // Package should be explicitly cleaned before being deleted
  // This happens when you forgot to write the package that you've created
  // Write to disk or make it clear explicitly
  Check(!resource_state_.IsDirty());
}

void Package::ReadFrom(Stream* stream) {
  Check(chunks_.IsEmpty());
  StreamReadClass(stream, chunks_);
  OnReadDone();
}

void Package::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, chunks_);
  OnWriteDone();
}

void Package::RemoveChunk(const Cstr* chunk_name) {
  Check(string_util::GetLength(chunk_name));
  ChunksIter it = chunks_.Begin();
  ChunksIter ie = chunks_.End();
  for (; it != ie; ++it) {
    if ((*it).first_.IsEqual(chunk_name)) {
      chunks_.Erase(it);
      return;
    }
  }
}

Chunk* Package::OpenChunk(const Cstr* chunk_name, OpenChunkOption option) {
  Check(string_util::GetLength(chunk_name));
  ChunksIter it = chunks_.Begin();
  ChunksIter ie = chunks_.End();
  Chunk* chunk = NULL;
  for (; it != ie; ++it) {
    if ((*it).first_.IsEqual(chunk_name)) {
      chunk = (*it).second_.ptr();
      bool is_for_read = (option == kRead) ? true : false;
      if (!is_for_read) {
        chunk->Clear();
      }
      // Rewind to read again
      chunk->set_position(0);
      break;
    }
  }
  if (!chunk && (option == kWrite)) {
    chunk = new Chunk;
    String name(chunk_name);
    Ptr<Chunk> ptr(chunk);
    Pair<String, Ptr<Chunk> > chunk_item(name, ptr);
    chunks_.PushBack(chunk_item);
  }
  if (chunk && (option == kWrite)) {
    SetDirty(true);
  }
  return chunk;
}

void Package::CloseChunk(Chunk* chunk) {
}

void Package::Clear() {
  chunks_.Clear();
  SetDirty(true);
}

} // namespace dg
