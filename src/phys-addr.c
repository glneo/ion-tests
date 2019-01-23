/*
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 *      Andrew F. Davis <afd@ti.com>
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

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <ion/ion.h>
#include <linux/ion.h>
#include <linux/dma_buf_phys.h>

#define HEAP_ID 2

size_t len = 32 * 1024;

int get_phys(int phys_fd, int fd, u_int64_t *phys)
{
	if (phys == NULL)
		return -EINVAL;

	struct dma_buf_phys_data data = {
		.fd = fd,
	};

	int ret = ioctl(phys_fd, DMA_BUF_PHYS_IOC_CONVERT, &data);
	if (ret < 0)
		return -errno;

	*phys = data.phys;

	return 0;
}

int main(int argc, char* argv[])
{
	int ion_fd = ion_open();
	if (ion_fd < 0) {
		printf("Failed to open ION device: %s\n", strerror(ion_fd));
		return EXIT_FAILURE;
	}

	int dma_buf_fd;
	int ret = ion_alloc(ion_fd, len, atoi(argv[1]), 0, &dma_buf_fd);
	if (ret) {
		printf("ion allocation failed: %s\n", __func__, strerror(ret));
		return EXIT_FAILURE;
	}

	ret = ion_close(ion_fd);
	if (ret < 0) {
		printf("Failed to close ION device: %s\n", strerror(ret));
		return EXIT_FAILURE;
	}

	int phys_fd = open("/dev/dma-buf-phys", O_RDONLY | O_CLOEXEC);
	if (phys_fd < 0) {
		printf("Failed to open DMA-BUF to Phys device: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	u_int64_t phys;
	ret = get_phys(phys_fd, dma_buf_fd, &phys);
	if (ret < 0) {
		printf("Failed to get physical address: %s\n", strerror(ret));
		return EXIT_FAILURE;
	}

	printf("Physical address: 0x%08x\n", (u_int32_t)phys);

	/* this should un-pin the buffer */
	ret = close(phys_fd);
	if (ret < 0) {
		printf("Failed to detach the buffer: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* this should free the buffer */
	ret = close(dma_buf_fd);
	if (ret < 0) {
		printf("Failed to close the buffer: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	return 0;
}
