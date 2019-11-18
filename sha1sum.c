/* SHA1 implementation */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "sha1.h"

void sha1_transform_amd64(uint32_t *sha1_state, uint32_t *data_block);
void sha1_transform_fast(uint32_t *sha1_state, uint32_t *data_block);

static int process_file(int filno, char *name)
{
	struct sha1_ctx ctx;
	uint8_t buffer[0x1000], *p;
	uint8_t md[20];
	size_t i;

	sha1_init(&ctx);

	while(i = read(filno, buffer, sizeof(buffer))) {
		if (-1 == i) {
			return -1;
		}

		p = buffer;
		while (i >= 64) {
			sha1_transform_amd64(ctx.state, (uint32_t *) p);
			ctx.data_counter += 64;
			p += 64, i -= 64;
		}
	}

	sha1_final(&ctx, md, buffer, i);

	for (i = 0; i < 20; ++i) {
		printf("%02x", md[i]);
	}
	printf("  %s\n", name);

	return 0;
}

int main(int argc, char **argv)
{
	int filno;
	ssize_t i;

	if (argc < 2) {
		if (-1 == process_file(STDIN_FILENO, "-")) {
			perror("-");
			goto err;
		}
	} else {
		for (i = 1; i < argc; ++i) {
			filno = open(argv[i], O_RDONLY);

			if (-1 == filno ||
				-1 == process_file(filno, argv[i]) ||
				-1 == close(filno)) {
				perror(argv[i]);
				goto err;
			}
		}

	}

	return EXIT_SUCCESS;
err:
	return EXIT_FAILURE;
}
