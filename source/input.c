#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "differentiator.h"
#include "font.h"
#include "input.h"

static void bad_argc_message(const char* const* argv);
static program_status_data request_re_entry(void);
static void clear_input_buffer(void);
static bool has_pdf_extension(const char* file_name);

program_status_data action_request(void)
{
    printf(MAKE_BOLD("Program should:\n1. Differentiate expression from the input file\n"
                                      "2. Differentiate expression from console\n"
                                      "3. Shut down\n"
                                      "Please, answer 1, 2 or 3\n"));
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
            case (int) FROM_CONSOLE_TO_TREE:
            case (int) PROGRAM_QUIT:
                return (program_status_data) user_input;
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

static program_status_data request_re_entry(void)
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

static void clear_input_buffer(void)
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

static void bad_argc_message(const char* const* argv)
{
    printf(MAKE_BOLD_RED("Incorrect command line arguments.\n")
           MAKE_BOLD("Usage: %s input_file [output.pdf]\n"), argv[0]);
}

bool check_files(FILE** const input_file, int argc, const char* const argv[])
{
    if (argc < MIN_NUMBER_OF_ARGUMENTS || argc > MAX_NUMBER_OF_ARGUMENTS)
    {
        bad_argc_message(argv);
        return false;
    }

    if (argc == MAX_NUMBER_OF_ARGUMENTS && !has_pdf_extension(argv[2]))
    {
        bad_argc_message(argv);
        return false;
    }

    *input_file = fopen(argv[1], "r");
    if (!*input_file)
    {
        printf(MAKE_BOLD_RED("Can't open input file: %s\n"), argv[1]);
        return false;
    }

    return true;
}

static bool has_pdf_extension(const char* file_name)
{
    size_t length = strlen(file_name);
    if (length < 4)
        return false;

    const char* extension = file_name + length - 4;
    return extension[0] == '.' &&
           tolower(extension[1]) == 'p' &&
           tolower(extension[2]) == 'd' &&
           tolower(extension[3]) == 'f';
}
