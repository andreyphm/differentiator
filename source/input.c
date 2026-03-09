#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "differentiator.h"
#include "font.h"
#include "input.h"

program_status_data action_request()
{
    printf(MAKE_BOLD("Program should:\n1. Read expression from file\n2. Read expression from console\n"
                                      "3. Output the differentiated tree to a PNG file\n4. Shut down\nPlease, answer 1, 2, 3 or 4\n"));
    int user_input = 0;
    int result_of_scanf = scanf("%d", &user_input);
    int extra_symbol = 0;
    program_status_data program_status = FROM_FILE_TO_TREE;

    if (result_of_scanf == 1)
    {
        extra_symbol = getchar();
        if (!(extra_symbol == '\n' || extra_symbol == EOF))
        {
            clear_input_buffer();
            program_status = request_re_entry();
        }

        else
        {
            switch (user_input)
            {
            case (int) FROM_FILE_TO_TREE:
                return FROM_FILE_TO_TREE;
            case (int) FROM_CONSOLE_TO_TREE:
                return FROM_CONSOLE_TO_TREE;
            case (int) DIF_TREE_TO_PNG_FILE:
                return DIF_TREE_TO_PNG_FILE;
            case (int) PROGRAM_QUIT:
                return PROGRAM_QUIT;
            default:
                program_status = request_re_entry();
            }
        }
    }

    else
    {
        clear_input_buffer();
        program_status = request_re_entry();
    }

    return program_status;
}

program_status_data request_re_entry()
{
    program_status_data program_status = FROM_FILE_TO_TREE;
    int user_answer = 0;

    printf(MAKE_BOLD_RED("Input is incorrect.\n"
                         "Do you want to continue the program?\n"
                         "Answer 1 to start again and write something else to exit:\n"));

    scanf("%d", &user_answer);

    if (user_answer != 1)
        program_status = PROGRAM_QUIT;

    else
    {
        int extra_symbol = 0;
        program_status = PROGRAM_START_AGAIN;

        do {
            extra_symbol = getchar();

            if(!isspace(extra_symbol))
            {
                program_status = PROGRAM_QUIT;
                clear_input_buffer();
                break;
            }

        } while(extra_symbol != '\n' && extra_symbol != EOF);
    }
    return program_status;
}

void clear_input_buffer()
{
    int entered_character = 0;

    do {
        entered_character = getchar();
    } while (entered_character != '\n' && entered_character != EOF);
}

char* read_file_to_buffer(FILE* const tree_txt_file)
{
    assert(tree_txt_file);

    struct stat file_struct = {};
    fstat(fileno(tree_txt_file), &file_struct);
    size_t file_size = (size_t)file_struct.st_size;

    char* buffer = (char*) calloc(file_size + 1, sizeof(*buffer));
    file_size = fread(buffer, sizeof(*buffer), file_size, tree_txt_file);
    buffer[file_size] = '\0';

    return buffer;
}

void bad_argc_message(const char* argv[])
{
    printf(MAKE_BOLD("You haven't entered the input and output files or you entered them incorrectly.\nDefault files will be used:"
                                "input.txt for input and output.txt for output.\nIf you want to select your files, please, "
                                "use: %s input_file output_file.\n\n"), argv[0]);
}
