// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

JsonObject::JsonObject() {}

JsonObject::~JsonObject() {
  ordered_keys_.Clear();
  map_.Clear();
}

void JsonObject::Add(const String& key, JsonValue& value) {
  map_.Add(key, value);
  ordered_keys_.PushBack(key);
}

JsonValue::JsonValue() {}

JsonValue::~JsonValue() {}

void Json::EatWhiteSpace(const Cstr* buffer, int& out_index) {
  while (buffer[out_index] != TXT('\0')) {
    switch (buffer[out_index]) {
    case string_util::kSpaceChar:
    case string_util::kTabChar:
    case string_util::kCarriageReturnChar:
    case string_util::kLineFeedChar:
      ++out_index;
      continue;
    default:
      return;
    }
  }
}

bool Json::ParseByteOrderMark(const Cstr* buffer, int& out_index) {
  const uint8_t kLittleEndianUTF16[] = { 0xff, 0xfe };
  const uint8_t kBigEndianUTF16[] = { 0xfe, 0xff };
  if (MyMemCmp(kLittleEndianUTF16, buffer, 2) == 0) {
    ++out_index;
    return true;
  } else if (MyMemCmp(kBigEndianUTF16, buffer, 2) == 0) {
    return false;
  }
  // Assuming that it is just plain string
  return true;
}

bool Json::Parse(const Cstr* buffer, JsonValue& out_value) {
  int index = 0;
  if (!ParseByteOrderMark(buffer, index)) {
    DG_LOG_LINE(TXT("error: json.parse.failed: parsing-bom-failed"));
    return false;
  }
  return ParseValue(buffer, index, out_value);
}

Json::Token Json::NextToken(const Cstr* buffer, int& out_index) {
  EatWhiteSpace(buffer, out_index);
  Cstr ch = buffer[out_index];
  out_index++;
  switch (ch) {
  case TXT('{'):
    return Token_CurlyOpen;
  case TXT('}'):
    return Token_CurlyClose;
  case TXT('['):
    return Token_SquaredOpen;
  case TXT(']'):
    return Token_SquaredClose;
  case TXT(','):
    return Token_Comma;
  case TXT('"'):
    return Token_String;
  case TXT('0'):
  case TXT('1'):
  case TXT('2'):
  case TXT('3'):
  case TXT('4'):
  case TXT('5'):
  case TXT('6'):
  case TXT('7'):
  case TXT('8'):
  case TXT('9'):
  case TXT('-'):
    return Token_Number;
  case TXT(':'):
    return Token_Colon;
  }
  out_index--;
  int length = string_util::GetLength(buffer);
  int remainingLength = length - out_index;
  // Parsing the 'false'
  if (remainingLength >= 5) {
    if (string_util::ToLower(buffer[out_index]) == 'f' &&
        string_util::ToLower(buffer[out_index+1]) == 'a' &&
        string_util::ToLower(buffer[out_index+2]) == 'l' &&
        string_util::ToLower(buffer[out_index+3]) == 's' &&
        string_util::ToLower(buffer[out_index+4]) == 'e') {
      out_index += 5;
      return Token_False;
    }
  }
  // Parsing the 'true'
  if (remainingLength >= 4) {
    if (string_util::ToLower(buffer[out_index]) == 't' &&
        string_util::ToLower(buffer[out_index+1]) == 'r' &&
        string_util::ToLower(buffer[out_index+2]) == 'u' &&
        string_util::ToLower(buffer[out_index+3]) == 'e') {
      out_index += 4;
      return Token_True;
    }
  }
  // Parsing the 'null'
  if (remainingLength >= 4) {
    if (string_util::ToLower(buffer[out_index]) == 'n' &&
        string_util::ToLower(buffer[out_index+1]) == 'u' &&
        string_util::ToLower(buffer[out_index+2]) == 'l' &&
        string_util::ToLower(buffer[out_index+3]) == 'l') {
      out_index += 4;
      return Token_Null;
    }
  }
  if (remainingLength > 0) {
    String bufferRemaining(buffer, out_index, 255);
    DG_LOG(TXT("error: json.nexttoken.failed: \n%s\n"), bufferRemaining.Get());
  }
  return Token_None;
}

Json::Token Json::LookAhead(const Cstr* buffer, int& out_index) {
  int index = out_index;
  return NextToken(buffer, index);
}

bool Json::ParseValue(const Cstr* buffer, int& out_index, JsonValue& out_value) {
  switch (LookAhead(buffer, out_index)) {
  case Token_String:
    out_value.value_type_ = JsonValue::ValueType_String;
    return ParseString(buffer, out_index, out_value.string_value_);
  case Token_Number:
    return ParseNumber(buffer, out_index, out_value);
  case Token_CurlyOpen:
    out_value.value_type_ = JsonValue::ValueType_Object;
    return ParseObject(buffer, out_index, out_value.object_value_);
  case Token_SquaredOpen:
    out_value.value_type_ = JsonValue::ValueType_Array;
    return ParseArray(buffer, out_index, out_value.array_value_);
  case Token_True:
    NextToken(buffer, out_index);
    out_value.value_type_ = JsonValue::ValueType_Boolean;
    out_value.primitive_value_.boolean_value_ = true;
    return true;
  case Token_False:
    NextToken(buffer, out_index);
    out_value.value_type_ = JsonValue::ValueType_Boolean;
    out_value.primitive_value_.boolean_value_ = false;
    return true;
  case Token_Null:
    NextToken(buffer, out_index);
    DG_UNIMPLEMENTED();
    return true;
  case Token_None:
    break;
  }
  // Json has no value
  Check(0);
  return false;
}

bool Json::ParseString(const Cstr* buffer, int& out_index, String& out_string) {
  String result;
  EatWhiteSpace(buffer, out_index);
  Cstr ch = buffer[out_index++];
  int length = string_util::GetLength(buffer);
  bool complete = false;
  while (!complete) {
    if (out_index == length) {
      break;
    }
    ch = buffer[out_index++];
    if (ch == string_util::kDoubleQuoteChar) {
      complete = true;
      break;
    } else if (ch == string_util::kBackSlashChar) {
      if (out_index == length) {
        break;
      }
      ch = buffer[out_index++];
      switch (ch) {
      case string_util::kDoubleQuoteChar:
        result.AddCharRight(string_util::kDoubleQuoteChar);
        break;
      case string_util::kBackSlashChar:
        result.AddCharRight(string_util::kBackSlashChar);
        break;
      case string_util::kSlashChar:
        result.AddCharRight(string_util::kSlashChar);
        break;
      case TXT('n'): // Newline
        result.AddCharRight(string_util::kLineFeedChar);
        break;
      case TXT('r'): // Carrage return
        result.AddCharRight(string_util::kCarriageReturnChar);
        break;
      case TXT('t'): // Horizontal tab
        result.AddCharRight(string_util::kTabChar);
        break;
      default:
        DG_UNIMPLEMENTED();
        break;
      } // End of switch
    } // End of else if
    else {
      result.AddCharRight(ch);
    }
  } // End of while
  if (!complete) {
    Check(0);
    return false;
  }
  out_string = result;
  return true;
}

bool Json::ParseNumber(const Cstr* buffer, int& out_index, JsonValue& out_value) {
  EatWhiteSpace(buffer, out_index);
  int lastIndex = GetLastIndexOfNumber(buffer, out_index);
  int charLength = (lastIndex - out_index) + 1;
  String numberStr(buffer, out_index, charLength);
  if (numberStr.IsInteger()) {
    out_value.value_type_ = JsonValue::ValueType_Ingeter;
    out_value.primitive_value_.integer_value_ = numberStr.ToInt();
  } else {
    out_value.value_type_ = JsonValue::ValueType_Float;
    out_value.primitive_value_.float_value_ = numberStr.ToFloat();
  }
  out_index = lastIndex + 1;
  return true;
}

int Json::GetLastIndexOfNumber(const Cstr* buffer, int& out_index) {
  int lastIndex = out_index;
  int length = string_util::GetLength(buffer);
  for (; lastIndex < length; lastIndex++) {
    switch (buffer[lastIndex]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '+':
    case '-':
    case '.':
    case 'e':
    case 'E':
      continue;
    }
    break;
  }
  return lastIndex - 1;
}

bool Json::ParseObject(const Cstr* buffer, int& out_index, JsonObject& out_object) {
  NextToken(buffer, out_index); // Eat '{'
  bool done = false;
  while (!done) {
    switch (LookAhead(buffer, out_index)) {
    case Token_None:
      return true;
    case Token_Comma:
      NextToken(buffer, out_index);
      break;
    case Token_CurlyClose:
      NextToken(buffer, out_index);
      return true;
    default: {
      // Parse name
      String name;
      if (!ParseString(buffer, out_index, name)) {
        Check(0);
        return false;
      }
      // Parse ':'
      {
        Token token = NextToken(buffer, out_index);
        if (token != Token_Colon) {
          Check(0);
          return false;
        }
      }
      // Parse value
      // Add the empty value first to reuse it in ParseValue()
      {
        JsonValue jsonValue;
        out_object.Add(name, jsonValue);
        if (!ParseValue(buffer, out_index, out_object.Get(name))) {
          Check(0);
          return false;
        }
      }
    } // End of default
    } // End of switch
  } // End of while
  Check(0);
  return false;
}

bool Json::ParseArray(const Cstr* buffer, int& out_index, JsonArray& out_array) {
  // Parse '['
  NextToken(buffer, out_index);
  bool done = false;
  while (!done) {
    Token token = LookAhead(buffer, out_index);
    if (token == Token_None) {
      // Check if you missed double quotes in string
      Check(0);
      return false;
    } else if (token == Token_Comma) {
      NextToken(buffer, out_index);
    } else if (token == Token_SquaredClose) {
      NextToken(buffer, out_index);
      break;
    } else {
      JsonValue value;
      int arrayIndex = out_array.PushBack(value) - 1;
      if (!ParseValue(buffer, out_index, out_array.Get(arrayIndex))) {
        Check(0);
        return false;
      }
    }
  }
  return true;
}

} // namespace dg
