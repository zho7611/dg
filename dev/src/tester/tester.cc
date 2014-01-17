// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "tester.h"

using namespace dg;

bool Tester::TestImageCompare(const Cstr* testee, const Cstr* original) {
  File file1(testee);
  File file2(original);
  file1.PrepareBuffer();
  file2.PrepareBuffer();
  if (file1.size() != file2.size()) {
    return false;
  }
  int num_diff_pixels = 0;
  int total_diff_values = GetPixelDiff(file1, file2, num_diff_pixels);
  int num_bytes = file1.size();
  const float kDiffPixelRateThreashold = 0.0001f;
  if (num_bytes == 0) {
    return false;
  }
  float diff_pixel_rate = float(num_diff_pixels)/num_bytes;
  if (diff_pixel_rate < kDiffPixelRateThreashold) {
    return true;
  }
  float diff_pixel_count = float(total_diff_values) / 255.f / num_diff_pixels;
  const float kDiffPixelAmountThreashold = 0.01f;
  if (diff_pixel_count < kDiffPixelAmountThreashold) {
    return true;
  }
  DG_LOG_LINE(TXT("error: tester.testimagecompare.failed: testee:%s original:%s numpixels:%d numdiffpixels:%d diffpixelrate:%f diffpixelamount:%f"),
      testee, original, num_bytes, num_diff_pixels, diff_pixel_rate, diff_pixel_count);
  return false;
}

int Tester::GetPixelDiff(const File& file1, const File& file2, int& out_num_diff_bytes) {
  Check(file1.size() == file2.size());
  // It should be 4-channel color
  Check((file1.size() % 4) == 0);
  int num_bytes = file1.size();
  const uint8_t* data1 = file1.data_const();
  const uint8_t* data2 = file2.data_const();
  int total_diff_values = 0;
  for (int idx = 0; idx < num_bytes; ++idx) {
    int diff_value = (*data1 > *data2) ? (*data1 - *data2) : (*data2 - *data1);
    if (diff_value > 0) {
      out_num_diff_bytes++;
      total_diff_values += diff_value;
    }
    ++data1;
    ++data2;
  }
  return total_diff_values;
}

bool Tester::TestFileCompare(const Cstr* testee, const Cstr* original) {
  if (File::IsFound(original)) {
    return File::IsSameFile(testee, original);
  } else {
    File::RenameFile(testee, original);
    return false;
  }
}

bool Tester::TestFileCompareOrCopyToOriginal(const Cstr* path, bool is_image_test) {
  FilePath filePath(path);
  String file_name = filePath.GetFileNameString();
  file_name.AddLeft(TXT("original_"));
  String original = filePath.GetDir();
  original.AddCharRight(string_util::kPathSeparatorChar);
  original.AddRight(file_name);
  if (!File::IsFound(original.Get())) {
    return File::CopyFile(path, original.Get());
  }
  if (is_image_test) {
    return TestImageCompare(path, original.Get());
  }
  return TestFileCompare(path, original.Get());
}

void Tester::SetFailed(const Cstr* message) {
  if (tests_.IsFound(current_test_name_)) {
    tests_.Get(current_test_name_).state_ = TestState_Failed;
    DG_LOG_LINE(
        TXT("error: unittest.failed: testname:%s message:%s"),
        current_test_name_.GetConst(),
        message);
  }
}

void Tester::BeginTest(const Cstr* name) {
  Check(name);
  current_test_name_= name;
  DG_LOG_LINE(TXT("tester.begintest: name:%s"), name);
}

void Tester::SetStatePassedIfNotFailed(const Cstr* name) {
  if (tests_.Get(name).state_ != TestState_Failed) {
    tests_.Get(name).state_ = TestState_Passed;
  }
}

void Tester::EndTest() {
  SetStatePassedIfNotFailed(current_test_name_.GetConst());
  DG_LOG_LINE(TXT("tester.endtest: name:%s"), current_test_name_.GetConst());
  current_test_name_= NULL;
}

bool Tester::IsTestPassed(const Cstr* name) {
  if (!tests_.IsFound(name)) {
    return false;
  }
  switch (tests_.Get(name).state_) {
  case TestState_Passed:
    return true;
  }
  return false;
}

bool Tester::AddTest(const Cstr* name, TestFunc InTestFunc) {
  String test_name(name);
  Check(!tests_.IsFound(test_name));
  TestItem item;
  item.state_ = TestState_None;
  item.func_ = InTestFunc;
  item.name_ = name;
  tests_.Get(test_name) = item;
  tests_order_.PushBack(test_name);
  return true;
}

void Tester::RunTest(const Cstr* name) {
  String test_name(name);
  if (!tests_.IsFound(test_name)) {
    return;
  }
  const TestItem& item = tests_.Get(test_name);
  BeginTest(name);
  item.func_();
  EndTest();
}

void Tester::RunAllTests() {
  TestsOrderType::Iterator it = tests_order_.Begin();
  TestsOrderType::Iterator ie = tests_order_.End();
  for (; it != ie; ++it) {
    RunTest((*it).Get());
  }
}

void Tester::RunTestSet(const FixedArray<String> &test) {
  for (int idx = 0; idx < test.Count(); ++idx) {
    const String& test_name = test.GetConst(idx);
    DG_LOG_LINE(TXT("tester.runtestset: testname:%s"), test_name.GetConst()); 
    RunTest(test_name.GetConst());
  }
}
