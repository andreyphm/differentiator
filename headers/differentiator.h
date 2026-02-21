#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <assert.h>

enum program_status_data
{
    FILE_TO_TREE        = 1,
    TREE_TO_TXT_FILE    = 2,
    TREE_TO_PNG_FILE    = 3,
    PROGRAM_QUIT        = 4,
    PROGRAM_START_AGAIN = 5
};

typedef enum operator_codes_data
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3
} operator_code;

typedef enum type_data
{
    OP  = 1,
    VAR = 2,
    NUM = 3
} type_data;

typedef union
{
    double number;
    char variable;
    operator_code op;
} data_union;

typedef struct node_value
{
    type_data type;
    data_union data_t;
} node_value;

typedef struct node_t
{
    node_value* value;
    node_t* right;
    node_t* left;
} node_t;

typedef struct operator_t
{
    operator_code code;
    const char* name;
    const char* design;
} operator_t;

const operator_t operators_array[] =
{
    {ADD, "ADD", "+"},
    {SUB, "SUB", "-"},
    {MUL, "MUL", "*"},
    {DIV, "DIV", "/"}
};

node_t* create_node(const type_data type, double data, node_t* left, node_t* right);
void destroy_node(node_t* node);

const char* enum_to_string(type_data type);
void tree_dump(node_t* const node, const char* const png_file_name);

node_t* copy_node(node_t* node);

node_t* dif(node_t* node);
node_t* dif_num(node_t* node);
node_t* dif_var(node_t* node);
node_t* dif_add(node_t* node);
node_t* dif_sub(node_t* node);
node_t* dif_mul(node_t* node);
node_t* dif_div(node_t* node);

#endif //DIFFERENTIATOR_H
