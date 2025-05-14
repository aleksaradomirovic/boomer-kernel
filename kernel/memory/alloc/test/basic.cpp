/*
 * Copyright (C) 2025  Aleksa Radomirovic
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

extern "C" {
#include "memory/alloc.h"
}

#include <gtest/gtest.h>

#include <bitset>

extern void *alloc_map;

class MemoryAllocTest : public testing::Test {
  protected:
    std::bitset<1024> mmap;

    void SetUp() override {
        alloc_map = &mmap;
    }
};

TEST_F(MemoryAllocTest, ClaimUnclaimed) {
    mmap.reset(0);
    ASSERT_EQ(claim_memory(0), 0);
}

TEST_F(MemoryAllocTest, ClaimClaimed) {
    mmap.set(0);
    ASSERT_NE(claim_memory(0), 0);
}

TEST_F(MemoryAllocTest, UnclaimUnclaimed) {
    mmap.reset(0);
    ASSERT_NE(unclaim_memory(0), 0);
}

TEST_F(MemoryAllocTest, UnclaimClaimed) {
    mmap.set(0);
    ASSERT_EQ(unclaim_memory(0), 0);
}

TEST_F(MemoryAllocTest, ClaimThenUnclaim) {
    mmap.reset(0);

    ASSERT_EQ(claim_memory(0), 0);
    ASSERT_TRUE(mmap.test(0));
    ASSERT_EQ(unclaim_memory(0), 0);
    ASSERT_FALSE(mmap.test(0));

    ASSERT_NE(unclaim_memory(0), 0);
    ASSERT_EQ(claim_memory(0), 0);
}
