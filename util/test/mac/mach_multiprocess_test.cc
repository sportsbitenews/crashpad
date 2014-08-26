// Copyright 2014 The Crashpad Authors. All rights reserved.
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

#include "util/test/mac/mach_multiprocess.h"

#include <unistd.h>

#include "base/basictypes.h"
#include "gtest/gtest.h"
#include "util/file/fd_io.h"
#include "util/test/errors.h"

namespace {

using namespace crashpad;
using namespace crashpad::test;

class TestMachMultiprocess final : public MachMultiprocess {
 public:
  TestMachMultiprocess() : MachMultiprocess() {}

  ~TestMachMultiprocess() {}

 protected:
  // The base class will have already exercised the Mach ports for IPC and the
  // child task port. Just make sure that the pipe is set up correctly and that
  // ChildPID() works as expected.
  virtual void Parent() override {
    int read_fd = ReadPipeFD();
    char c;
    ssize_t rv = ReadFD(read_fd, &c, 1);
    ASSERT_EQ(1, rv) << ErrnoMessage("read");
    EXPECT_EQ('M', c);

    pid_t pid;
    rv = ReadFD(read_fd, &pid, sizeof(pid));
    ASSERT_EQ(static_cast<ssize_t>(sizeof(pid)), rv) << ErrnoMessage("read");
    EXPECT_EQ(pid, ChildPID());

    int write_fd = WritePipeFD();
    c = 'm';
    rv = WriteFD(write_fd, &c, 1);
    ASSERT_EQ(1, rv) << ErrnoMessage("write");

    // The child will close its end of the pipe and exit. Make sure that the
    // parent sees EOF.
    rv = ReadFD(read_fd, &c, 1);
    ASSERT_EQ(0, rv) << ErrnoMessage("read");
  }

  virtual void Child() override {
    int write_fd = WritePipeFD();

    char c = 'M';
    ssize_t rv = WriteFD(write_fd, &c, 1);
    ASSERT_EQ(1, rv) << ErrnoMessage("write");

    pid_t pid = getpid();
    rv = WriteFD(write_fd, &pid, sizeof(pid));
    ASSERT_EQ(static_cast<ssize_t>(sizeof(pid)), rv) << ErrnoMessage("write");

    int read_fd = ReadPipeFD();
    rv = ReadFD(read_fd, &c, 1);
    ASSERT_EQ(1, rv) << ErrnoMessage("read");
    EXPECT_EQ('m', c);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(TestMachMultiprocess);
};

TEST(MachMultiprocess, MachMultiprocess) {
  TestMachMultiprocess mach_multiprocess;
  mach_multiprocess.Run();
}

}  // namespace