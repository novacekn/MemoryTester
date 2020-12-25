#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "sizes.h"
#include "tests.h"
#include "memory_tester.h"

#define EXIT_NONSTARTER 0x01
#define EXIT_ADDRESSLINES 0x02
#define EXIT_OTHERTEST 0x04
#define EXIT_FAILED_MLOCK 0x08

#ifndef MAP_LOCKED
    #define MAP_LOCKED 0
#endif


int _get_page_size(void) {
    int page_size = sysconf(_SC_PAGE_SIZE);
    if(page_size == -1) {
        page_size = 8192;
    }
    return page_size;
}


int main(int argc, char **argv) {
	unsigned long long size = strtoull(argv[1], NULL, 0);
	char *method = argv[2];

	memtester(size, method);

   	return 0;
}


int memtester(unsigned long long requested_bytes, char *method) {
    size_t page_size, buffer_size, half_length, count;
    ptrdiff_t page_size_mask;
    void volatile *buffer, *aligned;
    unsigned long volatile *buffer_a, *buffer_b;
    int do_mlock = 1, done_mem = 0;

    page_size = _get_page_size();
    page_size_mask = (ptrdiff_t) ~(page_size - 1);
    buffer = NULL;

    while(!done_mem) {
        while(!buffer && requested_bytes) {
            buffer = (void volatile *) malloc(requested_bytes);
            if(!buffer) {
                requested_bytes -= page_size;
            }
        }
        buffer_size = requested_bytes;

        if(do_mlock) {
            if((size_t) buffer % page_size) {
                aligned = (void volatile *) ((size_t) buffer & page_size_mask) + page_size;
                buffer_size -= ((size_t) aligned - (size_t) buffer);
            } else {
                aligned = buffer;
            }

            if(mlock((void *) aligned, buffer_size) < 0) {
                switch(errno) {
                    case EAGAIN:
                        printf("Over system limits, reducing...\n");
                        free((void *) buffer);
                        buffer = NULL;
                        requested_bytes -= page_size;
                        break;
                    case ENOMEM:
                        printf("Too many pages, reducing...\n");
                        free((void *) buffer);
                        buffer = NULL;
                        requested_bytes -= page_size;
                        break;
                    case EPERM:
                        printf("insufficient permission.\n");
                        do_mlock = 0;
                        free((void *) buffer);
                        buffer = NULL;
                        break;
                    default:
                        printf("Failed for unknown reason(s).\n");
                        do_mlock = 0;
                        done_mem = 1;
                }
            } else {
                done_mem = 1;
            }
        } else {
            done_mem = 1;
        }
    }

    if(!do_mlock) {
        fprintf(stderr, "Could not mlock memory. Exiting...\n");
        exit(EXIT_FAILED_MLOCK);
    }

    half_length = buffer_size / 2;
    count = half_length / sizeof(unsigned long);
    buffer_a = (unsigned long volatile *) aligned;
    buffer_b = (unsigned long volatile *) ((size_t) aligned + half_length);

    if(strcmp(method, "stuck_address") == 0) {
        // printf("Stuck Address:     ");
        if(!stuck_address(aligned, buffer_size / sizeof(unsigned long))) {
            printf("100");
        }
    }

    else if(strcmp(method, "random_value") == 0) {
	    if(!random_value(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "xor_comparison") == 0) {
	    if(!xor_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "sub_comparison") == 0) {
	    if(!sub_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "mul_comparison") == 0) {
	    if(!mul_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "div_comparison") == 0) {
	    if(!div_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "or_comparison") == 0) {
	    if(!or_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "and_comparison") == 0) {
	    if(!and_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "sequential_increment_comparison") == 0) {
	    if(!sequential_increment_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "solid_bits_comparison") == 0) {
	    if(!solid_bits_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "checkerboard_comparison") == 0) {
        //printf("Checkerboard:     ");
	    if(!checkerboard_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "block_sequential_comparison") == 0) {
	    if(!block_sequential_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "walking_zeroes_comparison") == 0) {
        //printf("Walking Zeroes:     ");
	    if(!walking_zeroes_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "walking_ones_comparison") == 0) {
	    if(!walking_ones_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "bit_spread_comparison") == 0) {
	    if(!bit_spread_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else if(strcmp(method, "bit_flip_comparison") == 0) {
        //printf("Bit Flip:     ");
	    if(!bit_flip_comparison(buffer_a, buffer_b, count)) {
		    printf("100");
	    }
    }

    else {
	    printf("%s is not a valid option.\n", method);
    }

    return 0;
}
