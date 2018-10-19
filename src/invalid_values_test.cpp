/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cerrno>
#include <sys/mman.h>

#include <gtest/gtest.h>

#include <ion/ion.h>

#include "ion_test_fixture.h"

class InvalidValues: public IonAllHeapsTest {
public:
	virtual void SetUp();
	virtual void TearDown();
	int m_validFd;
	int const m_badFd = -1;
};

void InvalidValues::SetUp()
{
	IonAllHeapsTest::SetUp();
	ASSERT_EQ(0, ion_alloc(m_ionFd, 4096, m_firstHeap, 0, &m_validFd))<< m_ionFd << " " << m_firstHeap;
	ASSERT_TRUE(m_validFd >= 0);
}

void InvalidValues::TearDown()
{
	ASSERT_EQ(0, close(m_validFd));
	m_validFd = -1;
	IonAllHeapsTest::TearDown();
}

TEST_F(InvalidValues, ion_close)
{
	EXPECT_EQ(-EBADF, ion_close(-1));
}

TEST_F(InvalidValues, ion_alloc_fd)
{
	int fd;
	/* invalid ion_fd */
	int ret = ion_alloc(0, 4096, m_firstHeap, 0, &fd);
	EXPECT_TRUE(ret == -EINVAL || ret == -ENOTTY);
	/* invalid ion_fd */
	EXPECT_EQ(-EBADF, ion_alloc(-1, 4096, m_firstHeap, 0, &fd));
	/* no heaps */
	EXPECT_EQ(-ENODEV, ion_alloc(m_ionFd, 4096, 0, 0, &fd));
	for (unsigned int heapMask : m_allHeaps) {
		SCOPED_TRACE(::testing::Message() << "heap " << heapMask);
		/* zero size */
		EXPECT_EQ(-EINVAL, ion_alloc(m_ionFd, 0, heapMask, 0, &fd));
		/* too large size */
		EXPECT_EQ(-EINVAL, ion_alloc(m_ionFd, -1, heapMask, 0, &fd));
		/* NULL handle */
		EXPECT_EQ(-EINVAL, ion_alloc(m_ionFd, 4096, heapMask, 0, NULL));
	}
}

TEST_F(InvalidValues, ion_map)
{
	/* bad fd */
	EXPECT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, 0, m_badFd, 0));
	/* zero length */
	EXPECT_EQ(MAP_FAILED, mmap(NULL, 0, PROT_READ, 0, m_validFd, 0));
	/* bad prot */
	EXPECT_EQ(MAP_FAILED, mmap(NULL, 4096, -1, 0, m_validFd, 0));
	/* bad offset */
	EXPECT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, 0, m_validFd, -1));
}
