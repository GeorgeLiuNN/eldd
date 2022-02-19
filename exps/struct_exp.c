#include <stdio.h>

#define BUFFER_SIZE 10

struct data_unit {
	unsigned int 		len;
	unsigned char		data[BUFFER_SIZE];
} __attribute__((packed));

struct acc_dtype {
	__uint32_t __pad;
	union {
		struct {
			__uint8_t chn;
			__uint8_t dat[3];
		};
		__uint32_t val;
	};
} __attribute__((__packed__));

int main() {
    int size = sizeof(struct data_unit);

    printf("data_unit size: %d \n", size);

    struct acc_dtype acc_data = {
        .__pad = 0x12345678u,
        .chn = 0x10u,
        .dat = {0x50u, 0x51, 0x52}
    };

    return 0;
}

// gcc struct_exp.c -g -o struct_exp