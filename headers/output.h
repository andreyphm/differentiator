#include "differentiator.h"

void tree_to_latex(node_t* const node, FILE* const output_file, const variable_t* const variables);
void latex_output(node_t* const node, FILE* const output_file, const variable_t* const variables, node_t* const parent = nullptr);
