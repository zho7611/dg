// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

int StreamReadAuto(Stream* stream, bool& out_value);
int StreamReadAuto(Stream* stream, short& out_value);
int StreamReadAuto(Stream* stream, unsigned short& out_value);
int StreamReadAuto(Stream* stream, int& out_value);
int StreamReadAuto(Stream* stream, unsigned int& out_value);
int StreamReadAuto(Stream* stream, float& out_value);
int StreamWriteAuto(Stream* stream, bool value);
int StreamWriteAuto(Stream* stream, short value);
int StreamWriteAuto(Stream* stream, unsigned short value);
int StreamWriteAuto(Stream* stream, int value);
int StreamWriteAuto(Stream* stream, unsigned int value);
int StreamWriteAuto(Stream* stream, float value);

template<class T> void StreamSneakFlat(Stream* stream, T& out_value);
template<class T> int StreamReadFlat(Stream* stream, T& out_value);
template<class T> void StreamReadAuto(Stream* stream, T& out_value);
template<class T> void StreamReadClass(Stream* stream, T& out_class);
template<class T> void StreamReadClass(Stream* stream, T*& out_class);
template<class T> void StreamReadClass(Stream* stream, Ptr<T>& out_class);
template<class T> void StreamReadPointer(Stream* stream, Ptr<T>& out_resource);
template<class T> int StreamReadFlatMultiple(Stream* stream, T* out_value, int num_elems);
template<class T> int StreamWriteFlat(Stream* stream, const T& value);
template<class T> void StreamWriteAuto(Stream* stream, T& value);
template<class T> void StreamWriteClass(Stream* stream, T& in_class);
template<class T> void StreamWriteClass(Stream* stream, T*& in_class);
template<class T> void StreamWriteClass(Stream* stream, Ptr<T>& in_class);
template<class T> void StreamWritePointer(Stream* stream, Ptr<T>& resource);
template<class T> int StreamWriteFlatMultiple(Stream* stream, const T* value, int num_elems);

// Serialization for container classes
template<class T1, class T2> void StreamReadClass(Stream* stream, Pair<T1, T2>& out_class);
template<class T1, class T2> void StreamWriteClass(Stream* stream, Pair<T1, T2>& in_class);
template<class T> void StreamReadClass(Stream* stream, Array<T>& out_class);
template<class T> void StreamWriteClass(Stream* stream, Array<T>& in_class);
template<class T> void StreamReadClass(Stream* stream, FixedArray<T>& out_class);
template<class T> void StreamWriteClass(Stream* stream, FixedArray<T>& in_class);
template<class KeyType, class ValueType> void StreamReadClass(Stream* stream, TreeMap<KeyType, ValueType>& out_class);
template<class KeyType, class ValueType> void StreamWriteClass(Stream* stream, TreeMap<KeyType, ValueType>& in_class);
template<class T> void StreamReadClass(Stream* stream, StringMap<T>& out_class);
template<class T> void StreamWriteClass(Stream* stream, StringMap<T>& in_class);
template<class T> void StreamReadClass(Stream* stream, List<T>& out_class);
template<class T> void StreamWriteClass(Stream* stream, List<T>& in_class);

template<class T>
void StreamSneakFlat(Stream* stream, T& out_value) {
  int read_size = StreamReadFlat(stream, out_value);
  stream->set_position(stream->position() - read_size);
}

template<class T>
int StreamReadFlat(Stream* stream, T& out_value) {
  return stream->ReadBytes(
    reinterpret_cast<uint8_t*>(&out_value),
    sizeof(T));
}

template<class T>
int StreamWriteFlat(Stream* stream, const T& value) {
  return stream->WriteBytes(
    reinterpret_cast<const uint8_t*>(&value),
    sizeof(T));
}

template<class T1, class T2>
void StreamReadClass(Stream* stream, Pair<T1, T2>& out_class) {
  StreamReadAuto(stream, out_class.first_);
  StreamReadAuto(stream, out_class.second_);
}

template<class T1, class T2>
void StreamWriteClass(Stream* stream, Pair<T1, T2>& in_class) {
  StreamWriteAuto(stream, in_class.first_);
  StreamWriteAuto(stream, in_class.second_);
}

template<class T>
void StreamReadClass(Stream* stream, Array<T>& out_class) {
  int num_items;
  StreamReadFlat(stream, num_items);
  out_class.Resize(num_items);
  for (int idx = 0; idx < num_items; ++idx) {
    T& item = out_class.Get(idx);
    StreamReadAuto(stream, item);
  }
}

template<class T>
void StreamWriteClass(Stream* stream, Array<T>& in_class) {
  int num_items = in_class.Count();
  StreamWriteFlat(stream, num_items);
  for (int idx = 0; idx < num_items; ++idx) {
    T& item = in_class.Get(idx);
    StreamWriteAuto(stream, item);
  }
}

template<class T>
void StreamReadClass(Stream* stream, FixedArray<T>& out_class) {
  int num_items;
  StreamReadFlat(stream, num_items);
  out_class.Resize(num_items);
  for (int idx = 0; idx < num_items; ++idx) {
    T& item = out_class.Get(idx);
    StreamReadAuto(stream, item);
  }
}

template<class T>
void StreamWriteClass(Stream* stream, FixedArray<T>& in_class) {
  int num_items = in_class.Count();
  StreamWriteFlat(stream, num_items);
  for (int idx = 0; idx < num_items; ++idx) {
    T& item= in_class.Get(idx);
    StreamWriteAuto(stream, item);
  }
}

template <class KeyType, class ValueType>
void StreamReadClass(Stream* stream, TreeMap<KeyType, ValueType>& out_class) {
  int num_items;
  StreamReadFlat(stream, num_items);
  KeyType key;
  ValueType value;
  for (int idx = 0; idx < num_items; ++idx) {
    StreamReadAuto(stream, key);
    StreamReadAuto(stream, value);
    out_class.Insert(key, value);
  }
}

template <class KeyType, class ValueType>
void StreamWriteClass(Stream* stream, TreeMap<KeyType, ValueType>& in_class) {
  StreamWriteFlat(stream, in_class.Count());
  typename TreeMap<KeyType, ValueType>::Iterator it = in_class.Begin();
  typename TreeMap<KeyType, ValueType>::Iterator end = in_class.End();
  for (; it != end; ++it) {
    StreamWriteAuto(stream, it.Key());
    StreamWriteAuto(stream, it.Value());
  }
}

template<class T>
void StreamReadClass(Stream* stream, StringMap<T>& out_class) {
  StreamReadClass(stream, out_class.map_);
}

template<class T>
void StreamWriteClass(Stream* stream, StringMap<T>& in_class) {
  StreamWriteClass(stream, in_class.map_);
}

template<class T>
void StreamReadClass(Stream* stream, List<T>& out_class) {
  int num_items;
  StreamReadFlat(stream, num_items);
  for (int idx = 0; idx < num_items; ++idx) {
    T item;
    StreamReadAuto(stream, item);
    out_class.PushBack(item);
  }
}

template<class T>
void StreamWriteClass(Stream* stream, List<T>& in_class) {
  int num_items = in_class.Count();
  StreamWriteFlat(stream, num_items);
  typename List<T>::Iterator it = in_class.Begin();
  typename List<T>::Iterator ie = in_class.End();
  for (; it != ie; ++it) {
    T item = *it;
    StreamWriteAuto(stream, item);
  }
}

template<class T>
void StreamReadClass(Stream* stream, T& out_class) {
  out_class.ReadFrom(stream);
}

template<class T>
void StreamWriteClass(Stream* stream, T& in_class) {
  in_class.WriteTo(stream);
}

template<class T>
int StreamReadFlatMultiple(Stream* stream, T* out_value, int num_elems) {
  return stream->ReadBytes(
    reinterpret_cast<uint8_t*>(out_value),
    sizeof(T)*num_elems);
}

template<class T>
int StreamWriteFlatMultiple(Stream* stream, const T* value, int num_elems) {
  return stream->WriteBytes(
    reinterpret_cast<const uint8_t*>(value),
    sizeof(T)*num_elems);
}

inline int StreamReadAuto(Stream* stream, bool& out_value) {
  return StreamReadFlat(stream, out_value);
}

inline int StreamWriteAuto(Stream* stream, bool value) {
  return StreamWriteFlat(stream, value);
}

inline int StreamReadAuto(Stream* stream, short& out_value) {
  return StreamReadFlat(stream, out_value);
}

inline int StreamWriteAuto(Stream* stream, short value) {
  return StreamWriteFlat(stream, value);
}

inline int StreamReadAuto(Stream* stream, unsigned short& out_value) {
  return StreamReadFlat(stream, out_value);
}

inline int StreamWriteAuto(Stream* stream, unsigned short value) {
  return StreamReadFlat(stream, value);
}

inline int StreamReadAuto(Stream* stream, int& out_value) {
  return StreamReadFlat(stream, out_value);
}

inline int StreamWriteAuto(Stream* stream, int value) {
  return StreamWriteFlat(stream, value);
}

inline int StreamReadAuto(Stream* stream, unsigned int& out_value) {
  return StreamReadFlat(stream, out_value);
}

inline int StreamWriteAuto(Stream* stream, unsigned int value) {
  return StreamWriteFlat(stream, value);
}

inline int StreamReadAuto(Stream* stream, float& out_value) {
  return StreamReadFlat(stream, out_value);
}

inline int StreamWriteAuto(Stream* stream, float value) {
  return StreamWriteFlat(stream, value);
}

template<class T>
void StreamReadAuto(Stream* stream, T& out_value) {
  StreamReadClass(stream, out_value);
}

template<class T>
void StreamWriteAuto(Stream* stream, T& value) {
  StreamWriteClass(stream, value);
}

template<class T>
void StreamReadClass(Stream* stream, T*& out_class) {
  out_class = CreateClass<T>();
  out_class->ReadFrom(stream);
}

template<class T>
void StreamWriteClass(Stream* stream, T*& in_class) {
  Check(in_class);
  in_class->WriteTo(stream);
}

template<class T>
void StreamReadClass(Stream* stream, Ptr<T>& out_class) {
  out_class = CreateClass<T>();
  out_class->ReadFrom(stream);
}

template<class T>
void StreamWriteClass(Stream* stream, Ptr<T>& value) {
  Check(value);
  value->WriteTo(stream);
}

template<class T>
void StreamReadPointer(Stream* stream, Ptr<T>& out_resource) {
  Check(!out_resource);
  ResourcePath resource_path;
  resource_path.ReadFrom(stream);
  String class_name;
  class_name.ReadFrom(stream);
  if (!resource_path.IsEmpty()) {
    out_resource = static_cast<T*>(LoadResourceByClassName(
      class_name.Get(), resource_path.Get(), LoadResourceOption_Read));
  } else {
    out_resource = 0;
  }
}

template<class T>
void StreamWritePointer(Stream* stream, Ptr<T>& resource) {
  ResourcePath resource_path;
  String class_name;
  if (resource) {
    resource_path = resource->GetResourcePath();
    class_name = resource->GetClassNameStr();
  }
  resource_path.WriteTo(stream);
  class_name.WriteTo(stream);
}

} // namespace dg
