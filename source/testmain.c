#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "tokenization.h"
#include "font.h"
#include "input.h"

int main()
{
    FILE* input_file = fopen("input.txt", "r");

    char* buffer = read_file_to_buffer(input_file);
    char* original_ptr = buffer;

    tokenization(buffer);

    free(original_ptr);
    fclose(input_file);
    printf(MAKE_BOLD_GREEN("Successfully\n"));

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}

char* read_file_to_buffer(FILE* const tree_txt_file)
{
    assert(tree_txt_file);

    struct stat file_struct = {};
    fstat(fileno(tree_txt_file), &file_struct);
    size_t file_size = (size_t)file_struct.st_size;

    char* buffer = (char*) calloc(file_size + 1, sizeof(*buffer));
    file_size = fread(buffer, sizeof(*buffer), file_size, tree_txt_file);
    *(buffer + file_size) = '$';

    return buffer;
}
