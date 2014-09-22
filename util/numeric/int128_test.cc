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

#include "util/numeric/int128.h"

#include "gtest/gtest.h"

namespace {

using namespace crashpad;

TEST(Int128, UInt128) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  const uint8_t kBytes[] =
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
#else
  const uint8_t kBytes[] =
      {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
#endif

  uint128_struct uint128;
  COMPILE_ASSERT(sizeof(uint128) == sizeof(kBytes), sizes_must_be_equal);

  uint128 = bit_cast<uint128_struct>(kBytes);

  EXPECT_EQ(0x0706050403020100u, uint128.lo);
  EXPECT_EQ(0x0f0e0d0c0b0a0908u, uint128.hi);
}

}  // namespace