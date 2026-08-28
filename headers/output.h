#ifndef OUTPUT_H
#define OUTPUT_H

#include "differentiator.h"
#include "input.h"

bool tree_to_pdf_file(const node_t* node, const variable_t* variables, const char* output_file_name);
void tree_to_console(const node_t* node, const variable_t* variables);
bool make_dif_tree(program_status_data program_status, variable_t** variables_ptr,
                   FILE* input_file, node_t** node_ptr);
void program_complete(variable_t** variables_ptr, node_t** node_ptr, FILE* input_file);


#endif // OUTPUT_H
