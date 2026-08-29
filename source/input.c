#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "differentiator.h"
#include "font.h"
#include "input.h"

static void bad_arguments_message(const char* program_name);
static bool has_pdf_extension(const char* file_name);

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

static void bad_arguments_message(const char* program_name)
{
    assert(program_name);

    printf(MAKE_BOLD_RED("Incorrect command line arguments.\n")
           MAKE_BOLD("Usage: %s [-i input_file] [-o output.pdf]\n"), program_name);
}

bool parse_arguments(program_options_t* options, int argc, const char* const argv[])
{
    assert(options);
    assert(argv);
    assert(argc > 0);
    assert(argv[0]);

    *options = {};
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-i") && !options->input_file_name && i + 1 < argc)
            options->input_file_name = argv[++i];

        else if (!strcmp(argv[i], "-o") && !options->output_file_name && i + 1 < argc)
            options->output_file_name = argv[++i];

        else
        {
            bad_arguments_message(argv[0]);
            return false;
        }
    }

    if (options->output_file_name && !has_pdf_extension(options->output_file_name))
    {
        bad_arguments_message(argv[0]);
        return false;
    }

    return true;
}

bool open_input_file(FILE** input_file, const char* input_file_name)
{
    assert(input_file);

    *input_file = nullptr;
    if (!input_file_name)
        return true;

    *input_file = fopen(input_file_name, "r");
    if (!*input_file)
    {
        printf(MAKE_BOLD_RED("Can't open input file: %s\n"), input_file_name);
        return false;
    }

    return true;
}

static bool has_pdf_extension(const char* file_name)
{
    assert(file_name);

    size_t length = strlen(file_name);
    if (length < 4)
        return false;

    const char* extension = file_name + length - 4;
    return extension[0] == '.' &&
           tolower(extension[1]) == 'p' &&
           tolower(extension[2]) == 'd' &&
           tolower(extension[3]) == 'f';
}
