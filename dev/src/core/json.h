// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.
// Found in the LICENSE file.

#pragma once

namespace dg {

class Json;
class JsonValue;

typedef Array<JsonValue> JsonArray;

class JsonObject {
public:
  typedef StringMap<JsonValue> ItemsType;
  typedef List<String> OrderedKeysType;
  typedef OrderedKeysType::Iterator OrderedKeysIterator;

  JsonObject();
  ~JsonObject();
  OrderedKeysIterator Begin() {
    return ordered_keys_.Begin();
  }
  OrderedKeysIterator End() {
    return ordered_keys_.End();
  }
  JsonValue& Get(const String& key) {
    return map_.Get(key);
  }
  bool IsFound(const String& key) const {
    return map_.IsFound(key);
  }
  void Add(const String& key, JsonValue& value);

private:
  // The key-value map that has the real value in it
  ItemsType map_;
  // The list that has only the keys in FIFO order
  OrderedKeysType ordered_keys_;
};

class JsonValue {
public:
  friend class Json;
  enum ValueType {
    ValueType_None,
    ValueType_Ingeter,
    ValueType_Float,
    ValueType_Boolean,
    ValueType_String,
    ValueType_Object,
    ValueType_Array,
  };

  JsonValue();
  ~JsonValue();
  ValueType GetType() {
    return value_type_;
  }
  String& GetString() {
    Check(value_type_ == ValueType_String);
    return string_value_;
  }
  JsonObject& GetJsonObject() {
    Check(value_type_ == ValueType_Object);
    return object_value_;
  }
  JsonArray& GetJsonArray() {
    Check(value_type_ == ValueType_Array);
    return array_value_;
  }
  int GetInteger() const {
    Check(value_type_ == ValueType_Ingeter);
    return primitive_value_.integer_value_;
  }
  float GetFloat() const {
    Check(value_type_ == ValueType_Float);
    return primitive_value_.float_value_;
  }
  bool GetBoolean() const {
    Check(value_type_ == ValueType_Boolean);
    return primitive_value_.boolean_value_;
  }
  // Returns true if its object has a value with the key
  bool Has(const Cstr* key) const {
    Check(value_type_ == ValueType_Object);
    return object_value_.IsFound(key);
  }
  // Get the object value by the key
  JsonValue& Get(const Cstr* key) {
    Check(value_type_ == ValueType_Object);
    return object_value_.Get(key);
  }

protected:
  ValueType value_type_;
  String string_value_;
  JsonArray array_value_;
  JsonObject object_value_;
  union {
    int integer_value_;
    float float_value_;
    bool boolean_value_;
  } primitive_value_;
};

// JSON(Java Script Object Notation) decoder
// See http://www.json.org/
// Reference: http://techblog.procurios.nl/k/618/news/view/14605/14863/How-do-I-write-my-own-parser-for-JSON.html
class Json {
public:
  enum Token {
    Token_None = 0,
    Token_CurlyOpen,
    Token_CurlyClose,
    Token_SquaredOpen,
    Token_SquaredClose,
    Token_Colon,
    Token_Comma,
    Token_String,
    Token_Number,
    Token_True,
    Token_False,
    Token_Null,
  };

  // Parse the Json string into JsonValue
  // Buffer - It should be a null-terminated string
  bool Parse(const Cstr* buffer, JsonValue& out_value);

protected:
  bool ParseByteOrderMark(const Cstr* buffer, int& out_index);
  bool ParseValue(const Cstr* buffer, int& out_index, JsonValue& out_value);
  bool ParseString(const Cstr* buffer, int& out_index, String& out_string);
  bool ParseNumber(const Cstr* buffer, int& out_index, JsonValue& out_value);
  bool ParseObject(const Cstr* buffer, int& out_index, JsonObject& out_object);
  bool ParseArray(const Cstr* buffer, int& out_index, JsonArray& out_array);
  Token LookAhead(const Cstr* buffer, int& out_index);
  Token NextToken(const Cstr* buffer, int& out_index);
  void EatWhiteSpace(const Cstr* buffer, int& out_index);
  int GetLastIndexOfNumber(const Cstr* buffer, int& out_index);
};

} // namespace dg
