// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

#include "../core/core_first.h"

namespace dg {

#define EXPECT_TRUE(expression) { if (!(expression)) { Tester::Instance().SetFailed(TXT(#expression)); TEST_REPORT(#expression, __FILE__, __LINE__); } }

#define EXPECT_FALSE(expression) { if ((expression)) { Tester::Instance().SetFailed(TXT(#expression)); TEST_REPORT(#expression, __FILE__, __LINE__); } }

#define REGISTER_TEST(test_name) Tester::Instance().AddTest(TXT(#test_name), test_##test_name);

#define TEST(test_name) \
  void test_##test_name(); \
  bool IsTestAdded_##test_name = Tester::Instance().AddTest(TXT(#test_name), test_##test_name); \
  void test_##test_name()

#define TEST_REPORT(message, fileName, lineNumber) { AssertFunc(TXT(message), TXT(fileName), lineNumber); Check(0); }

class Tester {
public:
  typedef void (*TestFunc)(void);

  enum TestState {
      TestState_None,
      TestState_Passed,
      TestState_Failed,
  };

  struct TestItem {
    String name_;
    TestState state_;
    TestFunc func_;
  };

  typedef List<String> TestsOrderType;
  typedef StringMap<TestItem> TestsType;

  String current_test_name_;
  TestsType tests_;
  TestsOrderType tests_order_;

  // Return the total value difference
  int GetPixelDiff(const File& file1, const File& file2, int& out_num_diff_bytes);
  // True if two image files are identical
  bool TestImageCompare(const Cstr* testee, const Cstr* original);
  bool TestFileCompare(const Cstr* testee, const Cstr* original);
  bool TestFileCompareOrCopyToOriginal(const Cstr* path, bool is_image_test = true);
  void SetFailed(const Cstr* msg);
  void SetStatePassedIfNotFailed(const Cstr* name);
  void BeginTest(const Cstr* name);
  void EndTest();
  bool IsTestPassed(const Cstr* name);
  bool AddTest(const Cstr* name, TestFunc callback);
  void RunTest(const Cstr* name);
  void RunTestSet(const FixedArray<String>& test);
  void RunAllTests();
  void DependsOn(const Cstr* name) {
    if (!IsTestPassed(name)) {
      const TestItem& item = tests_.Get(name);
      item.func_();
      SetStatePassedIfNotFailed(name);
    }
    EXPECT_TRUE(IsTestPassed(name));
  }
  static Tester& Instance() {
    static Tester instance;
    return instance;
  }
};

#define TEST_DEPENDS_ON(expression) \
  Tester::Instance().DependsOn(TXT(#expression));

}
