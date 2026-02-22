#include <stdlib.h>

#include "font.h"
#include "differentiator.h"
#include "input.h"
#include "parser.h"

int main()
{
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
                tree_dump(dif_node, "source/dump/differentiator_tree.png");

                destroy_node(dif_node);
                // destroy_node(node);
            }
                break;
            case FROM_FILE_TO_TREE:
            {
                FILE* tree_txt_file = fopen("differentiator_tree.txt", "r");
                if (!tree_txt_file)
                {
                    printf(MAKE_BOLD_RED("Failed to open file\n"));
                    break;
                }
                char* buffer = read_file_to_buffer(tree_txt_file);

                node = GetG(buffer);

                free(buffer);
                fclose(tree_txt_file);
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
                node = GetG(expression_str);

                free(expression_str);
                printf(MAKE_BOLD_GREEN("Successfully\n"));
            }
            default:
                break;
        }
    }

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}

