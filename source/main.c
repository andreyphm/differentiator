#include "differentiator.h"
#include "input.h"
#include "output.h"

int main(int argc, const char* argv[])
{
    program_options_t options = {};
    if (!parse_arguments(&options, argc, argv))
        return 1;

    FILE* input_file = nullptr;
    if (!open_input_file(&input_file, options.input_file_name))
        return 1;

    node_t* node = nullptr;
    variable_t* variables_ptr = nullptr;
    program_status_data input_source = input_file ? FROM_FILE_TO_TREE : FROM_CONSOLE_TO_TREE;

    bool succeeded = make_dif_tree(input_source, &variables_ptr, input_file, &node);
    if (succeeded)
    {
        if (options.output_file_name)
            succeeded = tree_to_pdf_file(node, variables_ptr, options.output_file_name);
        else
            tree_to_console(node, variables_ptr);
    }

    program_complete(&variables_ptr, &node, input_file);
    return succeeded ? 0 : 1;
}
