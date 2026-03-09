#include <stdlib.h>

#include "output.h"
#include "macros.h"
#include "font.h"

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
