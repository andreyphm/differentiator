#ifndef INPUT_H
#define INPUT_H

const int CORRECT_NUMBER_OF_FILES = 3;

enum program_status_data
{
    FROM_FILE_TO_TREE        = 1,
    FROM_CONSOLE_TO_TREE     = 2,
    DIF_TREE_TO_LATEX_FILE   = 3,
    PROGRAM_QUIT             = 4,
    PROGRAM_START_AGAIN      = 5
};

void check_files(FILE** const input_file, FILE** const output_file, int argc, const char* const argv[]);
void bad_argc_message(const char* const* argv);
program_status_data action_request();
program_status_data request_re_entry();
void clear_input_buffer();
char* read_file_to_buffer(FILE* const tree_txt_file);

#endif // INPUT_H
