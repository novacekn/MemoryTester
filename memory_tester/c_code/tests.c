#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "sizes.h"

#define ONE 0x00000001L


int compare_regions(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	int r = 0;
	size_t i;
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;

	for(i = 0; i < count; i++, p1++, p2++) {
		if(*p1 != *p2) {
			fprintf(
					stderr,
					"FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n",
					(unsigned long) *p1,
					(unsigned long) *p2,
					(unsigned long) (i * sizeof(unsigned long))
			       );
			r = -1;
		}
	}
	return r;
}


int stuck_address(unsigned long volatile *bufa, size_t count) {
    unsigned long volatile *p1 = bufa;
    unsigned int j;
    size_t i;
    float percent;

    for (j = 0; j < 16; j++) {
        p1 = (unsigned long volatile *) bufa;
        for (i = 0; i < count; i++) {
            *p1 = ((j + i) % 2) == 0 ? (unsigned long) p1 : ~((unsigned long) p1);
            *p1++;
        }
        percent = ((float) j / 16) * 100;
        printf("%3.0f\n", percent);
        fflush(stdout);
        p1 = (unsigned long volatile *) bufa;
        for (i = 0; i < count; i++, p1++) {
            if (*p1 != (((j + i) % 2) == 0 ? (unsigned long) p1 : ~((unsigned long) p1))) {
                    fprintf(stderr,
                            "FAILURE: possible bad address line at offset "
                            "0x%08lx.\n",
                            (unsigned long) (i * sizeof(unsigned long)));
                printf("Skipping to next test...\n");
                fflush(stdout);
                return -1;
            }
        }
    }
    return 0;
}


int random_value(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;

	for(i = 0; i < count; i++) {
		*p1++ = *p2++ = rand_ul();
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int xor_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		*p1++ ^= q;
		*p2++ ^= q;
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int sub_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		*p1++ -= q;
		*p2++ -= q;
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int mul_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		*p1++ *= q;
		*p2++ *= q;
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int div_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		if(!q) {
			q++;
		}
		*p1++ /= q;
		*p2++ /= q;
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int or_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		*p1++ |= q;
		*p2++ |= q;
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int and_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		*p1++ &= q;
		*p2++ &= q;
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int sequential_increment_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	size_t i;
	unsigned long q = rand_ul();

	for(i = 0; i < count; i++) {
		*p1++ = *p2++ = (i + q);
	}
	return compare_regions(buffer_a, buffer_b, count);
}


int solid_bits_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_a;
	unsigned int j;
	float percent;
	unsigned long q;
	size_t i;

	for(j = 0; j < 64; j++) {
		q = (j % 2) == 0 ? UL_ONEBITS : 0;

		p1 = (unsigned long volatile *) buffer_a;
		p2 = (unsigned long volatile *) buffer_b;

		for(i = 0; i < count; i++) {
			*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
		}
		percent = ((float) j / 64) * 100;
		printf("%3.0f\n", percent);
		fflush(stdout);

		if(compare_regions(buffer_a, buffer_b, count)) {
			return -1;
		}
	}
	return 0;
}


int checkerboard_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	unsigned int j;
	float percent;
	unsigned long q;
	size_t i;

	for(j = 0; j < 64; j++) {
		q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
		p1 = (unsigned long volatile *) buffer_a;
		p2 = (unsigned long volatile *) buffer_b;

		for(i = 0; i < count; i++) {
			*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
		}
		percent = ((float) j / 64) * 100;
		printf("%3.0f\n", percent);
		fflush(stdout);

		if(compare_regions(buffer_a, buffer_b, count)) {
			return -1;
		}
	}
	return 0;
}


int block_sequential_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	unsigned int j;
	float percent;
	size_t i;

	for(j = 0; j < 256; j++) {
		p1 = (unsigned long volatile *) buffer_a;
		p2 = (unsigned long volatile *) buffer_b;

		for(i = 0; i < count; i++) {
			*p1++ = *p2++ = (unsigned long) UL_BYTE(j);
		}
		percent = ((float) j / 256) * 100;
		printf("%3.0f\n", percent);
		fflush(stdout);

		if(compare_regions(buffer_a, buffer_b, count)) {
			return -1;
		}
	}
	return 0;
}


int walking_zeroes_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	unsigned int j;
	float percent;
	size_t i;

	for(j = 0; j < UL_LEN * 2; j++) {
		p1 = (unsigned long volatile *) buffer_a;
		p2 = (unsigned long volatile *) buffer_b;

		for(i = 0; i < count; i++) {
			if(j < UL_LEN) {
				*p1++ = *p2++ = ONE << j;
			} else {
				*p1++ = *p2++ = ONE << (UL_LEN * 2 - j - 1);
			}
		}
		percent = ((float) j / (UL_LEN * 2)) * 100;
		printf("%3.0f\n", percent);
		fflush(stdout);

		if(compare_regions(buffer_a, buffer_b, count)) {
			return -1;
		}
	}
	return 0;
}


int walking_ones_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	unsigned int j;
	float percent;
	size_t i;

	for(j = 0; j < UL_LEN * 2; j++) {
		p1 = (unsigned long volatile *) buffer_a;
		p2 = (unsigned long volatile *) buffer_b;

		for(i = 0; i < count; i++) {
			if(j < UL_LEN) {
				*p1++ = *p2++ = UL_ONEBITS ^ (ONE << j);
			} else {
				*p1++ = *p2++ = UL_ONEBITS ^ (ONE << (UL_LEN * 2 - j - 1));
			}
		}
		percent = ((float) j / (UL_LEN * 2)) * 100;
		printf("%3.0f\n", percent);
		fflush(stdout);

		if(compare_regions(buffer_a, buffer_b, count)) {
			return -1;
		}
	}
	return 0;
}


int bit_spread_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	unsigned int j;
	float percent;
	size_t i;

	for(j = 0; j < UL_LEN * 2; j++) {
		p1 = (unsigned long volatile *) buffer_a;
		p2 = (unsigned long volatile *) buffer_b;

		for(i = 0; i < count; i++) {
			if(j < UL_LEN) {
				*p1++ = *p2++ = (i % 2 == 0) ? (ONE << j) | (ONE << (j + 2)) : UL_ONEBITS ^ ((ONE << j) | (ONE << (j + 2)));
			} else {
				*p1++ = *p2++ = (i % 2 == 0) ? (ONE << (UL_LEN * 2 - 1 - j)) | (ONE << (UL_LEN * 2 + 1 - j)) : UL_ONEBITS ^ (ONE << (UL_LEN * 2 - 1 - j) | (ONE << (UL_LEN * 2 + 1 - j)));
			}
		}
		percent = ((float) j / (UL_LEN * 2)) * 100;
		printf("%3.0f\n", percent);
		fflush(stdout);

		if(compare_regions(buffer_a, buffer_b, count)) {
			return -1;
		}
	}
	return 0;
}


int bit_flip_comparison(unsigned long volatile *buffer_a, unsigned long volatile *buffer_b, size_t count) {
	unsigned long volatile *p1 = buffer_a;
	unsigned long volatile *p2 = buffer_b;
	unsigned int j, k;
	unsigned long q;
	float percent;
	size_t i;

	for(k = 0; k < UL_LEN; k++) {
		q = ONE << k;
		for(j = 0; j < 8; j++) {
			q = ~q;
			p1 = (unsigned long volatile *) buffer_a;
			p2 = (unsigned long volatile *) buffer_b;

			for(i = 0; i < count; i++) {
				*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
			}
			percent = ((float) k / UL_LEN) * 100;
			printf("%3.0f\n", percent);
			fflush(stdout);

			if(compare_regions(buffer_a, buffer_b, count)) {
				return -1;
			}
		}
	}
	return 0;
}
