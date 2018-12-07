#include "__write_helpers.h"

void open_output_file(char *filename) {
    int fd;
    if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
	abort();
    }
    dup2(fd, STDOUT_FILENO);
}

void reverse_bytes(unsigned int *a) {
    char *ptr = (char *)a;
    char tmp;

    for (int i = 0; i < (sizeof(int) / 2); i++) {
        tmp = *(ptr + i);
        *(ptr + i) = *(ptr + sizeof(int) - i - 1);
        *(ptr + sizeof(int) - i - 1) = tmp;
    }
}
