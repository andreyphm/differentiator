#include "differentiator.h"
#include "input.h"
#include "output.h"

int main(int argc, const char* argv[])
{
    FILE* input_file = nullptr;
    FILE* output_file = nullptr;
    check_files(&input_file, &output_file, argc, argv);

    program_status_data program_status = FROM_FILE_TO_TREE;
    node_t* node = nullptr;
    variable_t* variables_ptr = nullptr;

    while (program_status != PROGRAM_QUIT)
    {
        program_status = action_request();
        switch (program_status)
        {
            case PROGRAM_QUIT:
            case PROGRAM_START_AGAIN:
                break;
            case DIF_TREE_TO_LATEX_FILE:
                dif_tree_to_latex_file(node, variables_ptr, output_file);
                break;
            case FROM_FILE_TO_TREE:
            case FROM_CONSOLE_TO_TREE:
                output_to_tree(&program_status, &variables_ptr, input_file, &node);
            default:
                break;
        }
    }

    program_complete(&variables_ptr, &node, input_file);
}
