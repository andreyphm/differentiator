#include <stdlib.h>

#include "output.h"
#include "macros.h"
#include "font.h"

#define OP_DESIGN operators_array[NODE_OPERATION].design

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

void latex_output(node_t* const node, FILE* const output_file, const variable_t* const variables)
{
    switch (NODE_TYPE)
    {
        case NUM:
            fprintf(output_file, "%lg", NODE_NUMBER);
            break;
        case VAR:
            fprintf(output_file, "%s", variables[NODE_VAR_NUMBER].name);
            break;
        case OP:
            switch(operators_array[NODE_OPERATION].code)
            {
                case ADD:
                    fprintf(output_file, "\\left(");
                    latex_output(node->left, output_file, variables);
                    fprintf(output_file, " + ");
                    latex_output(node->right, output_file, variables);
                    fprintf(output_file, "\\right)");
                    break;

                case SUB:
                    fprintf(output_file, "\\left(");
                    latex_output(node->left, output_file, variables);
                    fprintf(output_file, " - ");
                    latex_output(node->right, output_file, variables);
                    fprintf(output_file, "\\right)");
                    break;

                case MUL:
                    fprintf(output_file, "\\left(");
                    latex_output(node->left, output_file, variables);
                    fprintf(output_file, " \\cdot ");
                    latex_output(node->right, output_file, variables);
                    fprintf(output_file, "\\right)");
                    break;

                case DIV:
                    fprintf(output_file, "\\left(");
                    fprintf(output_file, "\\frac{");
                    latex_output(node->left, output_file, variables);
                    fprintf(output_file, "}{");
                    latex_output(node->right, output_file, variables);
                    fprintf(output_file, "}");
                    fprintf(output_file, "\\right)");
                    break;

                case POW:
                    latex_output(node->left, output_file, variables);
                    fprintf(output_file, "^{");
                    latex_output(node->right, output_file, variables);
                    fprintf(output_file, "}");
                    break;

                case LN:
                case COS:
                case SIN:
                case EXP:
                    fprintf(output_file, "\\%s\\left(", OP_DESIGN);
                    latex_output(node->left, output_file, variables);
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
