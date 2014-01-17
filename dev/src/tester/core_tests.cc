// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "tester.h"

using namespace dg;

static bool TestCompress(const uint8_t* source, int source_length = 0) {
  const int kCompressedLength = RunLengthCompress::Compress(source, source_length, 0, 0);
  ScopedDelete<uint8_t> compressedBuffer = new uint8_t[kCompressedLength];
  RunLengthCompress::Compress(source, source_length, compressedBuffer.ptr(), kCompressedLength);
  const int kUncompressedLength = RunLengthCompress::Uncompress(compressedBuffer.ptr(), kCompressedLength, 0, 0);
  if (kUncompressedLength != source_length) {
    return false;
  }
  ScopedDelete<uint8_t> uncompressed_buffer = new uint8_t[kUncompressedLength];
  RunLengthCompress::Uncompress(compressedBuffer.ptr(), kCompressedLength, uncompressed_buffer.ptr(), kUncompressedLength);
  if (MyMemCmp(source, uncompressed_buffer.ptr(), source_length) != 0) {
    return false;
  }
  return true;
}

TEST(RunLengthCompress) {
  const Cstr test_string1[] = TXT("AAAABBBB");
  const Cstr test_string2[] = TXT("ABABABABAB");
  EXPECT_TRUE(TestCompress((uint8_t*)test_string1, sizeof(test_string1)));
  EXPECT_TRUE(TestCompress((uint8_t*)test_string2, sizeof(test_string2)));;
  EXPECT_TRUE(TestCompress((uint8_t*)TXT(""), 0));
  {
    uint8_t buffer[] = { 0xff, 0x0, 0xff, 0x0 };
    EXPECT_TRUE(TestCompress(buffer, sizeof(buffer)));
  }
}

TEST(Interlocked) {
  Interlocked locked;
  EXPECT_TRUE(locked.Get() == 0);
  locked.Increment();
  EXPECT_TRUE(locked.Get() == 1);
  locked.Decrement();
  EXPECT_TRUE(locked.Get() == 0);
  locked.Decrement();
  EXPECT_TRUE(locked.Get() == -1);
}

class ThreadTest : public Thread {
protected:
  virtual void ThreadMain() {
    Os::SleepMilliSeconds(1);
    count_.Increment();
    EndThread();
  }

public:
  static Interlocked count_;
};

Interlocked ThreadTest::count_;

TEST(Thread) {
  const int kNumThreads = 10;
  ThreadTest threads[kNumThreads];
  for (int idx = 0; idx < kNumThreads; ++idx) {
    DG_LOG_LINE(TXT("test.thread.run: %d/%d"), idx, kNumThreads);
    threads[idx].Run();
  }
  for (int idx = 0; idx < kNumThreads; ++idx) {
    DG_LOG_LINE(TXT("test.thread.waiting: %d/%d"), idx, kNumThreads);
    threads[idx].WaitForComplete();
  }
  EXPECT_TRUE(ThreadTest::count_.Get() == kNumThreads);
}

template<class GuidType>
static bool GuidTestCase(Tester& tester) {
  GuidType guid1, guid2;
  EXPECT_TRUE(guid1.MakeUnique());
  EXPECT_TRUE(guid2.MakeUnique());
  EXPECT_TRUE(guid1.IsValid());
  EXPECT_TRUE(guid2.IsValid());
  EXPECT_TRUE(guid1 != guid2);
  // Massive guid uniqueness test
  {
    Array<GuidType> guids;
    const int kNumGuids = 100;
    GuidType guid;
    // Fill in the guids
    for (int idx = 0; idx < kNumGuids; ++idx) {
      EXPECT_TRUE(guid.MakeUnique());
      guids.PushBack(guid);
    }
    DG_LOG_LINE(TXT("testcase.guid: begin"));
    // Test uniqueness for all items
    for (int left_idx = 0; left_idx < kNumGuids; ++left_idx) {
      const GuidType& left_guid = guids.Get(left_idx);
      for (int right_idx = 0; right_idx < kNumGuids; ++right_idx) {
        if (left_idx != right_idx) {
          const GuidType& right_guid = guids.Get(right_idx);
          EXPECT_TRUE(left_guid != right_guid);
        }
      }
    }
    DG_LOG_LINE(TXT("testcase.coreguid: end"));
  }
  return true;
}

TEST(CoreGuid) {
  EXPECT_TRUE(GuidTestCase<SmallSizeGuid>(Tester::Instance()));
  EXPECT_TRUE(GuidTestCase<BigSizeGuid>(Tester::Instance()));
}

TEST(Array) {
  Array<int> my_array;
  my_array.PushBack(0);
  my_array.PushBack(1);
  EXPECT_TRUE(my_array.Count() == 2);
  Array<int>::Iterator it = my_array.Begin();
  Array<int>::Iterator ie = my_array.End();
  EXPECT_TRUE(it != ie);
  EXPECT_TRUE(*it == 0);
  it++;
  EXPECT_TRUE(*it == 1);
  --it;
  EXPECT_TRUE(*it == 0);
  it--;
  EXPECT_TRUE(it == ie);
  EXPECT_TRUE(1 == my_array.PopBack());
  EXPECT_TRUE(0 == my_array.PopBack());
  EXPECT_TRUE(my_array.IsEmpty());
  it = my_array.Begin();
  ie = my_array.End();
  EXPECT_TRUE(it == ie);
  my_array.PushBack(5);
  my_array.PushBack(6);
  my_array.PushBack(7);
  EXPECT_TRUE(my_array.Count() == 3);
  my_array.Erase(1);
  EXPECT_TRUE(my_array.Count() == 2);
  EXPECT_TRUE(my_array.Get(0) == 5);
  EXPECT_TRUE(my_array.Get(1) == 7);
}

TEST(FixedArray) {
  FixedArray<int> my_array;
  my_array.Resize(2);
  my_array.Get(0) = 0;
  my_array.Get(1) = 1;
  EXPECT_TRUE(my_array.Get(0) == 0);
  EXPECT_TRUE(my_array.Get(1) == 1);
  FixedArray<int>::Iterator it = my_array.Begin();
  FixedArray<int>::Iterator ie = my_array.End();
  EXPECT_TRUE(it != ie);
  EXPECT_TRUE(*it == 0);
  it++;
  EXPECT_TRUE(*it == 1);
  --it;
  EXPECT_TRUE(*it == 0);
  it--;
  EXPECT_TRUE(it == ie);
  my_array.Resize(0);
  EXPECT_TRUE(my_array.IsEmpty());
  it = my_array.Begin();
  ie = my_array.End();
  EXPECT_TRUE(it == ie);
}

TEST(MemoryPool) {
  class SmallObject : public MemoryPool {
  public:
    Cstr member_[256];
  };
  const int kNumObjects = 1024;
  SmallObject* objects[kNumObjects];
  for (int idx = 0; idx < kNumObjects; ++idx) {
    objects[idx] = new SmallObject;
  }
  for (int idx = 0; idx < kNumObjects; idx+=2) {
    DG_SAFE_DELETE(objects[idx]);
  }
  for (int idx = 1; idx < kNumObjects; idx+=2) {
    DG_SAFE_DELETE(objects[idx]);
  }
}

TEST(List) {
  List<int> list;
  list.PushBack(0);
  list.PushBack(1);
  list.PushBack(2);
  list.PushBack(3);
  // Added 4 items. The count should be 4
  EXPECT_TRUE(list.Count() == 4);
  // Erased the end iterator. It doesn't affect the count
  list.Erase(list.End());
  EXPECT_TRUE(list.Count() == 4);
  // Begin iterator affects the count
  list.Erase(list.Begin());
  EXPECT_TRUE(list.Count() == 3);
  // Checking the content
  EXPECT_TRUE(list.Get(2) == 3);
  EXPECT_TRUE(list.Get(1) == 2);
  EXPECT_TRUE(list.Get(0) == 1);
  // Clear the list
  list.Clear();
  EXPECT_TRUE(list.Count() == 0);
}

TEST(Json) {
  // Test Value
  {
    Json json;
    JsonValue value;
    bool ret = json.Parse(TXT("{ \"foo\" : \"bar\", \"jiho\" : 123 }"), value);
    EXPECT_TRUE(ret);
    JsonObject object = value.GetJsonObject(); // Test for JsonObject copy constructor
    JsonObject object2;
    object2 = object; // Test for JsonObject assignment operator
    String barStr = object2.Get(TXT("foo")).GetString();
    EXPECT_TRUE(barStr.IsEqual(TXT("bar")));
  }
  // Test Array
  {
    Json json;
    JsonValue value;
    bool ret = json.Parse(
        TXT("{\"menu\": {")
        TXT("  \"id\": \"file\",")
        TXT("  \"value\": \"File\",")
        TXT("  \"popup\": {")
        TXT("    \"menuitem\": [")
        TXT("      {\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},")
        TXT("      {\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},")
        TXT("      {\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}")
        TXT("    ]")
        TXT("  }")
        TXT("}}"),
        value);
    EXPECT_TRUE(ret);
    JsonArray& items = value.Get(TXT("menu")).Get(TXT("popup")).Get(TXT("menuitem")).GetJsonArray();
    EXPECT_TRUE(items.Count() == 3);
    JsonValue& thirdValue = items.Get(2);
    EXPECT_TRUE(thirdValue.Get(TXT("onclick")).GetString().IsEqual(TXT("CloseDoc()")));
  }
  // Test True/False, Number
  {
    Json json;
    JsonValue value;
    bool ret = json.Parse(
        TXT("{\"True\": True, \"False\": False, \"Integer\": 1234567890, \"Float\": -16E7 }"),
        value);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(value.Get(TXT("True")).GetBoolean() == true);
    EXPECT_TRUE(value.Get(TXT("False")).GetBoolean() == false);
    EXPECT_TRUE(value.Get(TXT("Float")).GetFloat() == -16e7);
    EXPECT_TRUE(value.Get(TXT("Integer")).GetInteger() == 1234567890);
    EXPECT_TRUE(value.Has(TXT("Integer")));
    EXPECT_TRUE(value.Has(TXT("InvalidKey")) == false);
  }
}

TEST(CriticalSection) {
  CriticalSection cs;
  {
    cs.Enter();
    cs.Leave();
    {
      ScopedEnter<CriticalSection> scoped(cs);
    }
  }
}

TEST(HashMap) {
  // Test aware case
  {
    typedef HashMap<String, int, StringHashFunc> StringHashMap;
    StringHashMap str_map(3);
    str_map.Insert(TXT("haha"), 10);
    str_map.Insert(TXT("hoho"), 20);
    StringHashMap::Iterator it = str_map.Find(TXT("haha"));
    StringHashMap::Iterator ie = str_map.End();
    EXPECT_TRUE(it != ie);
    EXPECT_TRUE(str_map.Erase(it));
    EXPECT_TRUE(str_map.Count() == 1);
    EXPECT_TRUE(*str_map.Begin() == 20);
  }
}

TEST(Uint32HashMap) {
  typedef HashMap<unsigned int, int, Uint32HashFunc> Uint32HashMap;
  Uint32HashMap map(300);
  map.Insert(0x1234, 10);
  map.Insert(0x4567, 20);
  Uint32HashMap::Iterator it = map.Find(0x1234);
  Uint32HashMap::Iterator ie = map.End();
  EXPECT_TRUE(it != ie);
  EXPECT_TRUE(map.Erase(it));
  EXPECT_TRUE(map.Count() == 1);
  EXPECT_TRUE(*map.Begin() == 20);
}

TEST(Name) {
  const int kMaxSize = Name::kMaxNameLength;
  // Name with container test
  {
    List<Name> temp;
    temp.PushBack(TXT("haha"));
    temp.PushBack(TXT("hoho"));
    temp.PushBack(TXT("kuku"));
  }
  // Constructor test
  Name name(TXT("A"));
  // AddLeft() and AddRight() test
  {
    // Name Setting test
    name = TXT("Center");
    name.AddRight(TXT("Right"));
    name.AddLeft(TXT("Left"));
    EXPECT_TRUE(string_util::IsEqual(name.GetConst(), TXT("LeftCenterRight")));
  }
  // Maximum content length test
  {
    name.Clear();
    for (int idx = 0; idx < kMaxSize; ++idx) {
      name.AddRight(Name(TXT("A")));
    }
    EXPECT_TRUE(string_util::GetLength(name.GetConst()) == kMaxSize);
    Cstr buff[kMaxSize+1];
    for (int idx = 0; idx < kMaxSize; ++idx) {
      buff[idx] = TXT('A');
    }
    buff[kMaxSize] = TXT('\0');
    if (MyStrCmp(name.GetConst(), buff) != 0) {
      EXPECT_TRUE(0);
    }
  }
}

TEST(String) {
  const int kMaxLength = 255;
  {
    String temp;
  }
  {
    String hello(TXT("Hello"));
    EXPECT_TRUE(string_util::GetLength(hello.Get()) == 5);
  }
  String str(TXT("A"));
  for (int idx = 0; idx < kMaxLength-1; ++idx) {
    str.AddRight(String(TXT("A")));
  }
  EXPECT_TRUE(str.size()== kMaxLength);
  Cstr buff[kMaxLength+1];
  for (int idx = 0; idx < kMaxLength; ++idx) {
    buff[idx] = TXT('A');
  }
  buff[kMaxLength] = TXT('\0');
  if (!string_util::IsEqual(str.Get(), buff)) {
    EXPECT_TRUE(0);
  }
  // Test Add small string
  {
    String small_str(TXT("small"));
    EXPECT_TRUE(small_str.size() == 5);
    small_str.AddRight(String(TXT("small")));
    EXPECT_TRUE(string_util::IsEqual(small_str.Get(), TXT("smallsmall")));
    small_str.AddRight(
      TXT("12345678901234567890123456789012345678901234567890")
      TXT("12345678901234567890123456789012345678901234567890")
      TXT("123456789012345678901234567890"));
    EXPECT_TRUE(string_util::IsEqual(small_str.Get(),
      TXT("smallsmall")
      TXT("12345678901234567890123456789012345678901234567890")
      TXT("12345678901234567890123456789012345678901234567890")
      TXT("123456789012345678901234567890")));
  }
  // Test Subtract
  {
    String subtract(TXT("abcdefg"));
    subtract.SubtractLeft(TXT("xxx"));
    EXPECT_TRUE(subtract.IsEqual(TXT("abcdefg")));
    subtract.SubtractLeft(TXT("abc"));
    EXPECT_TRUE(subtract.IsEqual(TXT("defg")));
    subtract.SubtractRight(TXT("bc"));
    EXPECT_TRUE(subtract.IsEqual(TXT("defg")));
    subtract.SubtractRight(TXT("fg"));
    EXPECT_TRUE(subtract.IsEqual(TXT("de")));
  }
  // Test Split
  {
    String source(TXT("abc-def-ghi"));
    Array<String> splited;
    const Cstr kSeparators[] = TXT("-");
    int num_splited = string_util::Split(splited, source.Get(), kSeparators);
    const int kNumItems = 3;
    EXPECT_TRUE(num_splited == kNumItems);
    EXPECT_TRUE(splited.Count() == kNumItems);
    EXPECT_TRUE(splited.Get(0).IsEqual(TXT("abc")));
    EXPECT_TRUE(splited.Get(1).IsEqual(TXT("def")));
    EXPECT_TRUE(splited.Get(2).IsEqual(TXT("ghi")));
    // Test Fail
    {
      String failTest(TXT("abc"));
      splited.Clear();
      num_splited = string_util::Split(splited, failTest.Get(), kSeparators);
      EXPECT_TRUE(num_splited == 1);
      EXPECT_TRUE(splited.Count() == 1);
    }
  }
  // Test GetToken
  {
    String source(TXT("\"a bc \" \tdef"));
    bool is_except_quote = true;
    int token_size = 0;
    const Cstr kSeparators[] = TXT(" \t");
    const Cstr* next_token = string_util::GetToken(
        source.Get(), kSeparators, token_size, is_except_quote);
    EXPECT_TRUE(token_size == 7);
    EXPECT_TRUE(string_util::IsEqual(next_token, TXT("def")));
    // With is_except_quote=false
    {
      is_except_quote = false;
      next_token = string_util::GetToken(
          source.Get(), kSeparators, token_size, is_except_quote);
      EXPECT_TRUE(token_size == 2);
      EXPECT_TRUE(string_util::IsEqual(next_token, TXT("bc \" \tdef")));
    }
  }
  // Test LeftTrim
  {
    String source(TXT("   abc"));
    const Cstr* trimmed = string_util::LeftTrim(source.Get(), TXT(" \t"));
    EXPECT_TRUE(string_util::IsEqual(trimmed, TXT("abc")));
    String source2(TXT("abc  "));
    trimmed = string_util::LeftTrim(source2.Get(), TXT(" \t"));
    EXPECT_TRUE(string_util::IsEqual(trimmed, TXT("abc  ")));
  }
  // Test IndexOf
  {
    String temp(TXT("1234"));
    int idx = temp.IndexOf(TXT('2'));
    EXPECT_TRUE(idx == 1);
    idx = temp.LastIndexOf(TXT('3'));
    EXPECT_TRUE(idx == 2);
    idx = temp.IndexOf(TXT('5'));
    EXPECT_TRUE(idx == -1);
    idx = temp.LastIndexOf(TXT('5'));
    EXPECT_TRUE(idx == -1);
    String any(TXT("23"));
    idx = temp.IndexOfAny(any.GetConst());
    EXPECT_TRUE(idx == 1);
    idx = temp.LastIndexOfAny(any.GetConst());
    EXPECT_TRUE(idx == 2);
    any.Set(TXT("56"));
    idx = temp.IndexOfAny(any.GetConst());
    EXPECT_TRUE(idx == -1);
    idx = temp.LastIndexOfAny(any.GetConst());
    EXPECT_TRUE(idx == -1);
  }
  // Test Substring
  {
    String temp(TXT("abcdefg"));
    EXPECT_TRUE(temp.Substring(2, 5).IsEqual(TXT("cde")));
    EXPECT_TRUE(temp.Left(3).IsEqual(TXT("abc")));
    EXPECT_TRUE(temp.Right(3).IsEqual(TXT("efg")));
    EXPECT_TRUE(temp.Mid(2, 3).IsEqual(TXT("cde")));
  }
  // Test ANSI/Unicode String
  {
    AnsiStr ansi_str[] = "abc";
    UnicodeStr unicode_str[] = L"abc";
    AnsiStr target_ansi_str[100];
    UnicodeStr target_unicode_str[100];
    int length;
    length = string_util::AnsiToUnicode(ansi_str, -1, target_unicode_str, 0);
    EXPECT_TRUE(length == 4);
    string_util::AnsiToUnicode(ansi_str, -1, target_unicode_str, length);
    EXPECT_TRUE(string_util::IsEqual(target_unicode_str, unicode_str));
    length = string_util::UnicodeToAnsi(unicode_str, -1, target_ansi_str, 0);
    EXPECT_TRUE(length == 4);
    string_util::UnicodeToAnsi(unicode_str, -1, target_ansi_str, length);
    EXPECT_TRUE(string_util::IsEqual(target_ansi_str, ansi_str));
  }
}

TEST(Util) {
  // Min max bound test
  EXPECT_TRUE(MyMin(10, 20) == 10);
  EXPECT_TRUE(MyMax(10, 20) == 20);
  EXPECT_TRUE(MyMin(20, 10) == 10);
  EXPECT_TRUE(MyMax(20, 10) == 20);
  EXPECT_TRUE(BoundMinMax(5, 1, 10) == 5);
  EXPECT_TRUE(BoundMinMax(1, 5, 10) == 5);
  EXPECT_TRUE(BoundMinMax(10, 1, 5) == 5);
  // Enum test
  enum MyEnumTest {
    MyEnumTest_None = 0,
    MyEnumTest_1 = 1,
    MyEnumTest_2 = 2,
    MyEnumTest_4 = 4
  };
  MyEnumTest enum_test = MyEnumTest_None;
  EnumWrite(enum_test, MyEnumTest_1);
  EnumWrite(enum_test, MyEnumTest_4);
  EXPECT_TRUE(enum_test == (MyEnumTest_1 | MyEnumTest_4));
  EnumErase(enum_test, MyEnumTest_1);
  EXPECT_TRUE(enum_test == MyEnumTest_4);
  EXPECT_TRUE(EnumCheck(enum_test, MyEnumTest_4));
  EXPECT_TRUE(!EnumCheck(enum_test, MyEnumTest_1));
}

TEST(ModuleDir) {
  String module_dir;
  Os::GetModuleDirectory(module_dir);
  int length = module_dir.size();
  EXPECT_TRUE(length > 0);
  EXPECT_TRUE(*module_dir.Get(length-1) != string_util::kPathSeparatorChar);
}

TEST(ModuleFileName) {
  String cur_file_name;
  String cur_dir;
  Os::GetModuleFileName(cur_file_name);
  Os::GetModuleDirectory(cur_dir);
  EXPECT_TRUE(!cur_file_name.IsEqual(cur_dir));
}

TEST(WorkingDir) {
  String saved_directory;
  Os::GetWorkingDirectory(saved_directory);
  int length = saved_directory.size();
  EXPECT_TRUE(length > 0);
  // The last character in working directory shouldn't include the path separator
  EXPECT_TRUE(*saved_directory.Get(length-1) != string_util::kPathSeparatorChar);
  String user_directory;
  Os::GetUserDirectory(user_directory);
  Os::SetWorkingDirectory(user_directory.Get());
  String working_directory;
  Os::GetWorkingDirectory(working_directory);
  EXPECT_TRUE(*working_directory.Get(working_directory.size()-1) != string_util::kPathSeparatorChar);
  String module_directory;
  Os::GetModuleDirectory(module_directory);
  EXPECT_TRUE(*module_directory.Get(module_directory.size()-1) != string_util::kPathSeparatorChar);
  Os::SetWorkingDirectory(module_directory.Get());
  Os::GetWorkingDirectory(working_directory);
  EXPECT_TRUE(working_directory.IsEqual(module_directory));
  Os::SetWorkingDirectory(saved_directory.Get());
}

TEST(FilePath) {
  FilePath file_path(TXT("Dir\\File.ext"));
  EXPECT_TRUE(string_util::IsEqual(file_path.GetDir(), TXT("Dir")));
  EXPECT_TRUE(string_util::IsEqual(file_path.GetFileName(), TXT("File.ext")));
  String saved_dir;
  Os::GetWorkingDirectory(saved_dir);
  Os::SetWorkingDirectory(TXT("c:\\windows\\"));
  file_path.MakeAbsolute();
  EXPECT_TRUE(string_util::IsEqual(file_path.GetPath(), TXT("c:\\windows\\Dir\\File.ext")));
  file_path.MakeLocal();
  EXPECT_TRUE(string_util::IsEqual(file_path.GetPath(), TXT("Dir\\File.ext")));
  file_path = TXT("Dir/abc/def/../../../rgb/");
  EXPECT_TRUE(string_util::IsEqual(file_path.GetPath(), TXT("rgb")));
  Os::SetWorkingDirectory(saved_dir.Get());
}

enum TestEnum { A = 0, B = 1, C = 2, Num = 3 };

TEST(EnumArray) {
  EnumArray<TestEnum, int> enum_array(Num);
  enum_array.Get(A) = 1;
  enum_array.Get(B) = 2;
  EXPECT_TRUE(enum_array.Get(A) == 1);
  EXPECT_TRUE(enum_array.Get(B) == 2);
  EXPECT_TRUE(enum_array.Count()== Num);
  EnumArray<TestEnum, int> enum_array2(enum_array);
  EXPECT_TRUE(enum_array2.Get(A) == enum_array.Get(A));
  EXPECT_TRUE(enum_array2.Count()== enum_array.Count());
  EnumArray<TestEnum, int> enum_array3(Num, 3);
  EXPECT_TRUE(enum_array3.Get(A) == 3);
}

struct TestClass {
  int GetResourceType() {
    return 0;
  }
  void AddRef() {}
  void Release() {}
  void ReadFrom(Stream* stream) {
    StreamReadFlat(stream, test_);
    StreamReadFlat(stream, test2_);
  }
  void WriteTo(Stream* out_stream) {
    StreamWriteFlat(out_stream, test_);
    StreamWriteFlat(out_stream, test2_);
  }
  int test_;
  int test2_;
};

TEST(FixedArraySerialize) {
  Chunk chunk;
  TestClass testClass[] = { {0xAAAA, 0xBBBB}, {0xCCCC, 0xDDDD} };
  FixedArray<TestClass*> pointers;
  pointers.Resize(2);
  pointers.Get(0) = &testClass[0];
  pointers.Get(1) = &testClass[1];
  StreamWriteClass(&chunk, pointers);
  {
    chunk.set_position(0);
    FixedArray<TestClass*> pointers2;
    StreamReadClass(&chunk, pointers2);
    int test_result;
    test_result = pointers2.Get(0)->test2_;
    EXPECT_TRUE(test_result == testClass[0].test2_);
    test_result = pointers2.Get(1)->test2_;
    EXPECT_TRUE(test_result == testClass[1].test2_);
    dg::DeleteAll(pointers2);
  }
}

struct MyClassType {
  MyClassType() : val(0) {}
  int val;
};

static void StreamReadClass(Stream* stream, MyClassType& out_class) {
  out_class.val++;
}

TEST(UserDefinedChunkSerialize) {
  Chunk chunk;
  MyClassType mc;
  StreamReadAuto(&chunk, mc);
  EXPECT_TRUE(mc.val == 1);
}
