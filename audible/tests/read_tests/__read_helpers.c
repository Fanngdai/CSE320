#include "__read_helpers.h"

void open_input_file(char *filename) {
    int fd;
    if ((fd = open(filename, O_RDONLY, 0666)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
	abort();
    }
    dup2(fd, STDIN_FILENO);
}
