#include <stdlib.h>
#include <string.h>

#include "output.h"
#include "macros.h"
#include "font.h"
#include "dump.h"
#include "parser.h"

#define OP_DESIGN           operators_array[NODE_OPERATION].design
#define OP_CODE             operators_array[NODE_OPERATION].code
#define OP_PRIORITY         operators_array[NODE_OPERATION].priority
#define PARENT_PRIORITY     operators_array[PARENT_OPERATION].priority

void tree_to_latex(node_t* const node, FILE* const output_file, const variable_t* const variables)
{
    fprintf(output_file, "\\documentclass{article}\n");
    fprintf(output_file, "\\usepackage{amsmath}\n");
    fprintf(output_file, "\\begin{document}\n");
    fprintf(output_file, "\\[\n");

    latex_output(node, output_file, variables);

    fprintf(output_file, "\n\\]\n");
    fprintf(output_file, "\\end{document}\n");

    fclose(output_file);

    system("pdflatex -interaction=nonstopmode output.tex > nul 2>&1");
    // system("del /F /Q *.aux *.log *.out *.fdb_latexmk *.fls *.synctex.gz *.pdf 2>nul");

    printf(MAKE_BOLD_GREEN("The differentiated expession is saved to output.pdf\n"));
}

void latex_output(node_t* const node, FILE* const output_file, const variable_t* const variables, node_t* const parent)
{
    switch (NODE_TYPE)
    {
        case NUM:
            if (NODE_NUMBER > 0 || is_close_to_zero(NODE_NUMBER))
                fprintf(output_file, "%lg", NODE_NUMBER);
            else
                fprintf(output_file, "\\left(%lg\\right)", NODE_NUMBER);
            break;

        case VAR:
            fprintf(output_file, "%s", variables[NODE_VAR_NUMBER].name);
            break;

        case OP:
            switch(OP_CODE)
            {
                case ADD:
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\left(");
                    latex_output(node->left, output_file, variables, node);
                    fprintf(output_file, " + ");
                    latex_output(node->right, output_file, variables, node);
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\right)");
                    break;

                case SUB:
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\left(");
                    latex_output(node->left, output_file, variables, node);
                    fprintf(output_file, " - ");
                    latex_output(node->right, output_file, variables, node);
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\right)");
                    break;

                case MUL:
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\left(");
                    latex_output(node->left, output_file, variables, node);
                    fprintf(output_file, " \\cdot ");
                    latex_output(node->right, output_file, variables, node);
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\right)");
                    break;

                case DIV:
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\left(");
                    fprintf(output_file, "\\frac{");
                    latex_output(node->left, output_file, variables, node);
                    fprintf(output_file, "}{");
                    latex_output(node->right, output_file, variables, node);
                    fprintf(output_file, "}");
                    if (parent && (OP_PRIORITY > PARENT_PRIORITY)) fprintf(output_file, "\\right)");
                    break;

                case POW:
                    latex_output(node->left, output_file, variables, node);
                    fprintf(output_file, "^{");
                    latex_output(node->right, output_file, variables, node);
                    fprintf(output_file, "}");
                    break;

                case LN:
                case COS:
                case SIN:
                case EXP:
                    fprintf(output_file, "\\%s\\left(", OP_DESIGN);
                    latex_output(node->left, output_file, variables, node);
                    fprintf(output_file, "\\right)");
                    break;

                default:
                    break;
            }
        case SPEC:
        default:
            break;
    }
}

void dif_tree_to_latex_file(node_t* node, variable_t* variables_ptr, FILE* output_file)
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

void output_to_tree(program_status_data* program_status, variable_t** variables_ptr, 
                    FILE* input_file, node_t** node_ptr)
{
    variable_t* variables = nullptr;
    char* buffer = nullptr;
    char* original_ptr = nullptr;
    if (*variables_ptr) variables_destroy(variables_ptr);

    if (*program_status == FROM_FILE_TO_TREE)
        from_file_to_tree(&variables, input_file, &buffer, &original_ptr);
    else
        from_console_to_tree(&variables, &buffer, &original_ptr);
                    

    list_t list = {nullptr, nullptr, nullptr};
    list.head = create_token(SPEC, (token_union){.spec_symbol = '!'}, &list);
    list.tail = list.head;

    error_code error = tokenization(buffer, variables, &list);
    if (error)
    {
        free(original_ptr);
        list_destroy(&list);
        *variables_ptr = variables;
        *program_status = request_re_entry();
        return;
    }

    if (*node_ptr) destroy_node(*node_ptr);
    token_t* current = list.head;
    *node_ptr = GetG(&current);

    free(original_ptr);
    list_destroy(&list);
    *variables_ptr = variables;
    if (!*node_ptr)
    {
        *program_status = request_re_entry();
        return;
    }

    printf(MAKE_BOLD_GREEN("Successfully\n"));
}

void from_file_to_tree(variable_t** variables, FILE* input_file, char** buffer_ptr, char** original_ptr)
{
    *variables = (variable_t*) calloc(MAX_NUMBER_OF_VARS, sizeof(variable_t));
    
    rewind(input_file);
    *buffer_ptr = read_file_to_buffer(input_file);
    size_t buffer_len = strlen(*buffer_ptr);
    (*buffer_ptr)[buffer_len] = '$';
    *original_ptr = *buffer_ptr;
}

void from_console_to_tree(variable_t** variables, char** buffer_ptr, char** original_ptr)
{
    printf(MAKE_BOLD("Please, write the expression in console\n"));

    *variables = (variable_t*) calloc(MAX_NUMBER_OF_VARS, sizeof(variable_t));

    *buffer_ptr = nullptr;
    size_t length = 0;

    ssize_t num_of_characters = getline(buffer_ptr, &length, stdin);
    if (num_of_characters == -1)
    {
        free(*variables);
        printf(MAKE_BOLD_RED("Failed to read from console\n"));
        return;
    }
    (*buffer_ptr)[num_of_characters - 1] = '$';
    *original_ptr = *buffer_ptr;
}

void program_complete(variable_t** variables_ptr, node_t** node_ptr, FILE* input_file)
{
    if (*variables_ptr) variables_destroy(variables_ptr);
    destroy_node(*node_ptr);
    fclose(input_file);
    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}
