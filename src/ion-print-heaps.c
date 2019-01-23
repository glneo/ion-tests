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

static const char *heap_types[] = {
	"ION_HEAP_TYPE_SYSTEM",
	"ION_HEAP_TYPE_SYSTEM_CONTIG",
	"ION_HEAP_TYPE_CARVEOUT",
	"ION_HEAP_TYPE_CHUNK",
	"ION_HEAP_TYPE_DMA",
	"ION_HEAP_TYPE_CUSTOM",
};

int main(int argc, char* argv[])
{
	int ion_fd = ion_open();
	if (ion_fd < 0) {
		printf("Failed to open ION device: %s\n", strerror(ion_fd));
		return EXIT_FAILURE;
	}

	int heap_count;
	int ret = ion_query_heap_cnt(ion_fd, &heap_count);
	if (ret < 0) {
		printf("Failed to get heap count: %d\n", ret);
		return EXIT_FAILURE;
	}

	struct ion_heap_data *heap_data;
	heap_data = malloc(sizeof(*heap_data) * heap_count);
	ret = ion_query_get_heaps(ion_fd, heap_count, heap_data);
	if (ret < 0) {
		printf("Failed to get heap count: %d\n", ret);
		return EXIT_FAILURE;
	}

	for (int i = 0; i < heap_count; i++){
		printf("Heap:\n");
		printf("\tName: %.*s\n", MAX_HEAP_NAME, heap_data[i].name);
		printf("\tType: %s\n", heap_types[heap_data[i].type]);
		printf("\tID: %d\n", heap_data[i].heap_id);
	}

	ion_close(ion_fd);

	return 0;
}
