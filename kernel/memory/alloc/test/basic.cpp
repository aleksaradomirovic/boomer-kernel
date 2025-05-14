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
extern paddr_t alloc_max_addr;

class MemoryClaimTest : public testing::Test {
  protected:
    std::bitset<1024> mmap;

    void SetUp() override {
        alloc_map = &mmap;
        alloc_max_addr = 1024 * ALLOC_GRANULARITY;
    }
};

TEST_F(MemoryClaimTest, ClaimUnclaimed) {
    mmap.reset(0);
    ASSERT_EQ(claim_memory(0), 0);
}

TEST_F(MemoryClaimTest, ClaimClaimed) {
    mmap.set(0);
    ASSERT_NE(claim_memory(0), 0);
}

TEST_F(MemoryClaimTest, UnclaimUnclaimed) {
    mmap.reset(0);
    ASSERT_NE(unclaim_memory(0), 0);
}

TEST_F(MemoryClaimTest, UnclaimClaimed) {
    mmap.set(0);
    ASSERT_EQ(unclaim_memory(0), 0);
}

TEST_F(MemoryClaimTest, ClaimThenUnclaim) {
    mmap.reset(0);

    ASSERT_EQ(claim_memory(0), 0);
    ASSERT_TRUE(mmap.test(0));
    ASSERT_EQ(unclaim_memory(0), 0);
    ASSERT_FALSE(mmap.test(0));

    ASSERT_NE(unclaim_memory(0), 0);
    ASSERT_EQ(claim_memory(0), 0);
}

class MemoryAcquireTest : public MemoryClaimTest {
  protected:
    paddr_t acquired[512];
};

TEST_F(MemoryAcquireTest, NoneTaken) {
    mmap.reset();

    ASSERT_EQ(acquire_memory(acquired, 512), 0);
}

TEST_F(MemoryAcquireTest, SomeTaken) {
    mmap.reset();
    mmap.set(0);

    ASSERT_EQ(acquire_memory(acquired, 512), 0);
}

TEST_F(MemoryAcquireTest, MostTaken) {
    mmap.set();
    mmap.reset(0);
    size_t count = mmap.count();

    ASSERT_NE(acquire_memory(acquired, 512), 0);
    ASSERT_EQ(mmap.count(), count);
}

TEST_F(MemoryAcquireTest, AllTaken) {
    mmap.set();

    ASSERT_NE(acquire_memory(acquired, 512), 0);
}

TEST_F(MemoryAcquireTest, WithObstacle) {
    mmap.reset();
    mmap.set(1);
    size_t count = mmap.count();

    ASSERT_EQ(acquire_memory(acquired, 2), 0);
    ASSERT_EQ(mmap.count(), count + 2);
    ASSERT_TRUE(mmap.test(1));
}

class MemoryReleaseTest : public MemoryAcquireTest {
  protected:
    void SetUp() override {
        MemoryAcquireTest::SetUp();

        for(size_t i = 0; i < 512; i++) {
            acquired[i] = i * ALLOC_GRANULARITY;
        }
    }
};

TEST_F(MemoryReleaseTest, NoneTaken) {
    mmap.reset();

    ASSERT_NE(release_memory(acquired, 512), 0);
}

TEST_F(MemoryReleaseTest, SomeTaken) {
    mmap.set();
    mmap.reset(0);

    ASSERT_NE(release_memory(acquired, 512), 0);
}

TEST_F(MemoryReleaseTest, AllTaken) {
    mmap.set();

    ASSERT_EQ(release_memory(acquired, 512), 0);
    
    for(size_t i = 512; i < mmap.size(); i++) {
        ASSERT_TRUE(mmap.test(i));
    }
}

TEST_F(MemoryReleaseTest, FaultyAddress) {
    mmap.set();
    size_t count = mmap.count();

    acquired[0] = mmap.size() * ALLOC_GRANULARITY;
    ASSERT_NE(release_memory(acquired, 512), 0);
    ASSERT_EQ(mmap.count(), count - 511);
}
