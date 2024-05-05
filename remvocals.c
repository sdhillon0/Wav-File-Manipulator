#include <stdio.h>

int main(int argc, char **argv) {
    FILE *input, *output;
    if ((input = fopen(argv[1], "rb")) == NULL) {
        fprintf(stderr, "Error in opening the input");
        return 1;
    }
    if ((output = fopen(argv[2], "wb")) == NULL) {
        fprintf(stderr, "Error in opening the output");
        return 1;
    }
    int header_size = 44;
    char header[header_size];
    if (fread(header, header_size, 1, input) != 1) {
        fprintf(stderr, "Error in reading the header");
        return 1;
    }
    if (fwrite(header, header_size, 1, output) != 1) {
        fprintf(stderr, "Error in writing the header");
        return 1;
    }
    short combine[2];
    while (fread(combine, sizeof(short), 1, input) == 1) {
        if (fread(&combine[1], sizeof(short), 1, input) != 1) {
            fprintf(stderr, "Error in reading input");
            return 1;
        }
        combine[0] = (combine[0] - combine[1]) / 2;
        combine[1] = combine[0];
        if (fwrite(combine, sizeof(short), 2, output) != 2) {
            fprintf(stderr, "Error in writing the content");
            return 1;
        }
    }
    if (fclose(output) != 0) {
        fprintf(stderr, "Error in closing the output");
        return 1;
    }
    if (fclose(input) != 0) {
        fprintf(stderr, "Error in closing the input");
        return 1;
    }
    return 0;
}