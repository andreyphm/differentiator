#ifndef OUTPUT_H
#define OUTPUT_H

#include "differentiator.h"
#include "input.h"

void tree_to_latex(node_t* const node, FILE* const output_file, const variable_t* const variables);
void latex_output(node_t* const node, FILE* const output_file, const variable_t* const variables, node_t* const parent = nullptr);
void dif_tree_to_latex_file(node_t* node, variable_t* variables_ptr, FILE* output_file);
void output_to_tree(program_status_data* program_status, variable_t** variables_ptr, 
                    FILE* input_file, node_t** node_ptr);
void from_file_to_tree(variable_t** variables, FILE* input_file, char** buffer_ptr, char** original_ptr);
void from_console_to_tree(variable_t** variables, char** buffer_ptr, char** original_ptr);
void program_complete(variable_t** variables_ptr, node_t** node_ptr, FILE* input_file);


#endif // OUTPUT_H
