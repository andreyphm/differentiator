#ifndef INPUT_H
#define INPUT_H

const int MIN_NUMBER_OF_ARGUMENTS = 2;
const int MAX_NUMBER_OF_ARGUMENTS = 3;

enum program_status_data
{
    FROM_FILE_TO_TREE        = 1,
    FROM_CONSOLE_TO_TREE     = 2,
    PROGRAM_QUIT             = 3,
    PROGRAM_START_AGAIN      = 4
};

bool check_files(FILE** const input_file, int argc, const char* const argv[]);
program_status_data action_request(void);
char* read_file_to_buffer(FILE* const tree_txt_file);

#endif // INPUT_H
