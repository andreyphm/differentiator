#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <stdio.h>

enum program_status_data
{
    FROM_FILE_TO_TREE        = 1,
    FROM_CONSOLE_TO_TREE     = 2
};

struct program_options_t
{
    const char* input_file_name;
    const char* output_file_name;
};

bool parse_arguments(program_options_t* options, int argc, const char* const argv[]);
bool open_input_file(FILE** input_file, const char* input_file_name);
char* read_file_to_buffer(FILE* const tree_txt_file);

#endif // INPUT_H
