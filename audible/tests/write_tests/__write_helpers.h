#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void open_input_file(char *filename);
void open_output_file(char *filename);
void reverse_bytes(unsigned int *a);
