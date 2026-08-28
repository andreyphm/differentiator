#include "differentiator.h"
#include "input.h"
#include "output.h"

int main(int argc, const char* argv[])
{
    FILE* input_file = nullptr;
    if (!check_files(&input_file, argc, argv))
        return 1;

    program_status_data program_status = FROM_FILE_TO_TREE;
    node_t* node = nullptr;
    variable_t* variables_ptr = nullptr;
    const char* output_pdf_file = argc == MAX_NUMBER_OF_ARGUMENTS ? argv[2] : nullptr;

    while (program_status != PROGRAM_QUIT)
    {
        program_status = action_request();
        switch (program_status)
        {
            case PROGRAM_QUIT:
            case PROGRAM_START_AGAIN:
                break;
            case FROM_FILE_TO_TREE:
            case FROM_CONSOLE_TO_TREE:
                if (make_dif_tree(program_status, &variables_ptr, input_file, &node))
                {
                    if (output_pdf_file)
                        tree_to_pdf_file(node, variables_ptr, output_pdf_file);
                    else
                        tree_to_console(node, variables_ptr);
                }
                break;
            default:
                break;
        }
    }

    program_complete(&variables_ptr, &node, input_file);
}
