// Copyright 2015 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "snapshot/win/process_reader_win.h"

#include <windows.h>

#include "gtest/gtest.h"

namespace crashpad {
namespace test {
namespace {

TEST(ProcessReaderWin, SelfBasic) {
  ProcessReaderWin process_reader;
  ASSERT_TRUE(process_reader.Initialize(GetCurrentProcess()));

#if !defined(ARCH_CPU_64_BITS)
  EXPECT_FALSE(process_reader.Is64Bit());
#else
  EXPECT_TRUE(process_reader.Is64Bit());
#endif

  EXPECT_EQ(GetCurrentProcessId(), process_reader.ProcessID());

  const char kTestMemory[] = "Some test memory";
  char buffer[arraysize(kTestMemory)];
  ASSERT_TRUE(
      process_reader.ReadMemory(reinterpret_cast<uintptr_t>(kTestMemory),
                                sizeof(kTestMemory),
                                &buffer));
  EXPECT_STREQ(kTestMemory, buffer);
}

TEST(ProcessReaderWin, SelfOneThread) {
  ProcessReaderWin process_reader;
  ASSERT_TRUE(process_reader.Initialize(GetCurrentProcess()));

  const std::vector<ProcessReaderWin::Thread>& threads =
      process_reader.Threads();

  // If other tests ran in this process previously, threads may have been
  // created and may still be running. This check must look for at least one
  // thread, not exactly one thread.
  ASSERT_GE(threads.size(), 1u);

  EXPECT_EQ(GetThreadId(GetCurrentThread()), threads[0].id);
#if defined(ARCH_CPU_64_BITS)
  EXPECT_NE(0, threads[0].context.Rip);
#else
  EXPECT_NE(0, threads[0].context.Eip);
#endif

  EXPECT_EQ(0, threads[0].suspend_count);
}

}  // namespace
}  // namespace test
}  // namespace crashpad
