#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WAV_HEADER 11
#define DEFAULT_VOLUME 4
#define DEFAULT_DELAY 8000

int _init_variables(int *d, int *v, int argc, char **argv) { // parsing arguments from the command
    int arg;
    while ((arg = getopt(argc, argv, "d:v:")) != -1) {
        switch (arg) {
            case 'd':
                *d = strtol(optarg, NULL, 10);
                if (*d <= 0 || *d == 2147483647) { // // 0, negative, failure, or too large delay
                    fprintf(stderr, "Invalid value for -d: '%s'\n", optarg);
                    return 1;
                }
                break;
            case 'v':
                *v = strtol(optarg, NULL, 10);
                if (*v <= 0 || *v == 2147483647) { // // 0, negative or failure
                    fprintf(stderr, "Invalid value for -v: '%s'\n", optarg);
                    return 1;
                }
                break;
            default:
                return 1;
                break;
        }
    }
    if (*v > 32767) return 2; // There won't be any echo
    return 0;
}

int _error_in_writing_output() { // helper
    fprintf(stderr, "Failed to write the output\n");
    return 1;
}

int _error_in_opening_file() {
    fprintf(stderr, "Unable to open input, output, or both\n");
    return 1;
}

int _terminate(FILE *I, FILE *O) { // helper for closing files
    if (fclose(O) != 0) {
        fprintf(stderr, "Failed to close the output\n");
        return 1;
    }
    if (fclose(I) != 0) {
        fprintf(stderr, "Failed to close the input\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc % 2 == 0 || argc < 2) return _error_in_opening_file(); // unintelligible arguments
    
    int volume_multiplier_int = DEFAULT_VOLUME, delay = DEFAULT_DELAY, getopt_return = _init_variables(&delay, &volume_multiplier_int, argc, argv); // Processing for user input
    if (getopt_return == 1) return 1; // 0: Normal; 1: Error 2: No echo;
    if (optind != argc - 2) {
        fprintf(stderr, "Unspecified input, output, or both\n");
        return 1;
    }
    if (strcmp(argv[optind], argv[optind + 1]) == 0) {
        fprintf(stderr, "The output should differ from the input\n");
        return 1;
    }

    FILE *input, *output; // Open and check if input and output files are valid
    if((input = fopen(argv[optind++], "rb")) == NULL) return _error_in_opening_file();
    if((output = fopen(argv[optind++], "wb")) == NULL) return _error_in_opening_file();

    int header[WAV_HEADER]; // Read & Write the header
    if (fread(header, sizeof(int) * WAV_HEADER, 1, input) != 1) { // Read the header
        fprintf(stderr, "Error in reading the header\n");
        return 1;
    }

    short buff, num_read;
    if (getopt_return == 2) { // volume_multiplier_int is too large, just copy the original to output
        if (fwrite(header, sizeof(int) * WAV_HEADER, 1, output) != 1) return _error_in_writing_output();
        while (fread(&buff, sizeof(short), 1, input) == 1) if (fwrite(&buff, sizeof(short), 1, output) != 1) return _error_in_writing_output();
        return _terminate(input, output);
    }
    if ((num_read = fread(&buff, 1, 2, input)) == 2) { // At least one sample appear in the input
        header[1] += delay * 2; // header manipulations
        header[10] += delay * 2;
        if (fseek(input, -2, SEEK_CUR) != 0) return _error_in_opening_file();
    }
    else { // No sample in the input
        if (fwrite(header, sizeof(int) * WAV_HEADER, 1, output) != 1) return _error_in_writing_output();
        if (fwrite(&buff, 1, num_read, output) != num_read) return _error_in_writing_output();
        return _terminate(input, output);
    }
    
    if (fwrite(header, sizeof(int) * WAV_HEADER, 1, output) != 1) return _error_in_writing_output();

    short volume_multiplier = volume_multiplier_int, mix_buff;; // Cast volume_multiplier_int to short
    short *buff_array = (short*) malloc(delay * sizeof(short));
    int sample_index_tracker;
    
    // Now loop through the input to obtain the first buff block / until first delay is reached
    for (sample_index_tracker = 0; sample_index_tracker < delay; sample_index_tracker++) {
        // Check that it is reading at least one element
        if (fread(&buff, sizeof(short), 1, input) == 1) buff_array[sample_index_tracker] = buff;
        else {
            for (int i = sample_index_tracker; i < delay; i++) buff_array[i] = 0;
            break;
        }
    }
    
    if (fwrite(buff_array, sizeof(short), delay, output) != delay) return _error_in_writing_output();

    short long_input = (sample_index_tracker == delay) ? 1:0; // 1: input is longer than delay; 2: Instead
    short a = 0;
    // We have reached delay, now **mix** in the samples from buff
    while (fread(&buff, sizeof(short), 1, input) == 1) {
        // Check if we've reached end of delay content and restart from sample_index_tracker = 0
        if (sample_index_tracker == delay) sample_index_tracker = 0;
        // You basically want to conintue reading and mix with content of buff_array and then rewrite buff_array
        mix_buff = buff + buff_array[sample_index_tracker] / volume_multiplier;
        buff_array[sample_index_tracker++] = buff;
        // Now we write the mixed buff
        if (fwrite(&mix_buff, sizeof(short), 1, output) != 1) return _error_in_writing_output();
    }

    for (int i = 0; i < sample_index_tracker; i++) buff_array[i] /= volume_multiplier;

    // Now if we have a remaining none-full value for buff to write, write it out
    if (long_input) {
        for (int i = sample_index_tracker; i < delay; i++) buff_array[i] /= volume_multiplier;
        int remain = delay - sample_index_tracker; // Number of samples remaining
        if (fwrite(buff_array + sample_index_tracker, sizeof(short), remain, output) != remain) return _error_in_writing_output();
    }
    
    // Now write from 0, to sample_index_tracker - 1
    if (fwrite(buff_array, sizeof(short), sample_index_tracker, output) != sample_index_tracker) return _error_in_writing_output();
    free(buff_array);

    return _terminate(input, output);
}