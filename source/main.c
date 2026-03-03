#include <stdlib.h>

#include "font.h"
#include "differentiator.h"
#include "input.h"
#include "parser.h"
#include "tokenization.h"

int main(int argc, const char* argv[])
{
    FILE* input_file = nullptr;
    FILE* output_file = nullptr;

    if (argc == CORRECT_NUMBER_OF_FILES)
    {
        input_file = fopen(argv[1], "r");
        output_file = fopen(argv[2], "w");

        if (!input_file)
        {
            printf(MAKE_BOLD_RED("Can't open input file. Default input file will be used: input.txt.\n"));
            input_file = fopen("input.txt", "r");
        }

        if (!output_file)
        {
            printf(MAKE_BOLD_RED("Can't open output file. Default output file will be used: output.txt.\n"));
            output_file = fopen("output.txt", "w");
        }
    }
    else
    {
        bad_argc_message(argv);
        input_file = fopen("input.txt", "r");
        output_file = fopen("output.txt", "w");
    }

    program_status_data program_status = FROM_FILE_TO_TREE;
    node_t* node = nullptr;

    while (program_status != PROGRAM_QUIT)
    {
        program_status = action_request();
        switch (program_status)
        {
            case PROGRAM_QUIT:
            case PROGRAM_START_AGAIN:
                break;
            case DIF_TREE_TO_PNG_FILE:
            {
                node_t* dif_node = dif(node);

                for (bool simplifications = true; simplifications;)
                {
                    simplifications = false;
                    dif_node = simplify_node(dif_node, &simplifications);
                }

                tree_dump(dif_node, "source/dump/differentiator_tree.png");

                destroy_node(dif_node);
            }
                break;
            case FROM_FILE_TO_TREE:
            {
                variable_t variables[MAX_NUMBER_OF_VARS] = {};
                char* buffer = read_file_to_buffer(input_file);
                char* original_ptr = buffer;

                // if (node) destroy_node(node);
                // node = GetG(&buffer);
                tokenization(buffer, variables);

                free(original_ptr);
                fclose(input_file);
                printf(MAKE_BOLD_GREEN("Successfully\n"));
                break;
            }
            case FROM_CONSOLE_TO_TREE:
            {
                printf(MAKE_BOLD("Please, write the expression in console\n"));
                char* expression_str = nullptr;
                size_t length = 0;

                ssize_t num_of_characters = getline(&expression_str, &length, stdin);

                if (num_of_characters == -1)
                {
                    printf(MAKE_BOLD_RED("Failed to read from console\n"));
                    break;
                }

                expression_str[num_of_characters - 1] = '$';
                char* original_ptr = expression_str;

                if (node) destroy_node(node);
                node = GetG(&expression_str);

                free(original_ptr);
                printf(MAKE_BOLD_GREEN("Successfully\n"));
            }
            default:
                break;
        }
    }

    destroy_node(node);
    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}

void bad_argc_message(const char* argv[])
{
    printf(MAKE_BOLD("You haven't entered the input and output files or you entered them incorrectly.\nDefault files will be used:"
                                "input.txt for input and output.txt for output.\nIf you want to select your files, please, "
                                "use: %s input_file output_file.\n\n"), argv[0]);
}
