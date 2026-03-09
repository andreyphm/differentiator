#include <stdlib.h>
#include <string.h>

#include "font.h"
#include "differentiator.h"
#include "input.h"
#include "parser.h"
#include "tokenization.h"
#include "dump.h"
#include "output.h"

#define TREE_TXT_FILE

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
            printf(MAKE_BOLD_RED("Can't open output file. Default output file will be used: output.tex.\n"));
            output_file = fopen("output.tex", "w");
        }
    }
    else
    {
        bad_argc_message(argv);
        input_file = fopen("input.txt", "r");
        output_file = fopen("output.tex", "w");
    }

    program_status_data program_status = FROM_FILE_TO_TREE;
    node_t* node = nullptr;
    variable_t* variables_ptr = nullptr;
    variable_t* variables = nullptr;
    char* buffer = nullptr;
    char* original_ptr = nullptr;

    while (program_status != PROGRAM_QUIT)
    {
        program_status = action_request();
        switch (program_status)
        {
            case PROGRAM_QUIT:
            case PROGRAM_START_AGAIN:
                break;
            case DIF_TREE_TO_LATEX_FILE:
            {
                node_t* dif_node = dif(node);

                for (bool simplifications = true; simplifications;)
                {
                    simplifications = false;
                    dif_node = simplify_node(dif_node, &simplifications);
                }

                tree_dump(dif_node, TREE_DUMP_PNG, variables_ptr);

                tree_to_latex(dif_node, output_file, variables_ptr);

                destroy_node(dif_node);
            }
                break;
            case FROM_FILE_TO_TREE:
            case FROM_CONSOLE_TO_TREE:
            {
                if (program_status == FROM_FILE_TO_TREE)
                {
                    if (variables_ptr) variables_destroy(&variables_ptr);
                    variables = (variable_t*) calloc(MAX_NUMBER_OF_VARS, sizeof(variable_t));

                    rewind(input_file);
                    buffer = read_file_to_buffer(input_file);
                    size_t buffer_len = strlen(buffer);
                    buffer[buffer_len] = '$';
                    original_ptr = buffer;
                }

                if (program_status == FROM_CONSOLE_TO_TREE)
                {
                    printf(MAKE_BOLD("Please, write the expression in console\n"));

                    if (variables_ptr) variables_destroy(&variables_ptr);
                    variables = (variable_t*) calloc(MAX_NUMBER_OF_VARS, sizeof(variable_t));

                    buffer = nullptr;
                    size_t length = 0;

                    ssize_t num_of_characters = getline(&buffer, &length, stdin);
                    if (num_of_characters == -1)
                    {
                        printf(MAKE_BOLD_RED("Failed to read from console\n"));
                        break;
                    }
                    buffer[num_of_characters - 1] = '$';
                    original_ptr = buffer;
                }

                list_t list = {nullptr, nullptr, nullptr};
                list.head = create_token(SPEC, (token_union){.spec_symbol = '!'}, &list);
                list.tail = list.head;

                error_code error = tokenization(buffer, variables, &list);
                if (error)
                {
                    free(original_ptr);
                    list_destroy(&list);
                    variables_ptr = variables;
                    program_status = request_re_entry();
                    break;
                }

                if (node) destroy_node(node);
                token_t* current = list.head;
                node = GetG(&current);

                free(original_ptr);
                list_destroy(&list);
                variables_ptr = variables;
                if (!node)
                {
                    program_status = request_re_entry();
                    break;
                }
                printf(MAKE_BOLD_GREEN("Successfully\n"));
            }
            default:
                break;
        }
    }

    if (variables_ptr) variables_destroy(&variables_ptr);
    destroy_node(node);
    fclose(input_file);
    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}
