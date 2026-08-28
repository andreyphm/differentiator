#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

const double NUMBER_CLOSE_TO_ZERO  = 10e-12;
const size_t LAST_CHAR_OP_NUM = 4;
const size_t FIRST_FUNC_NUM   = 5;
const size_t LAST_FUNC_NUM    = 8;

enum operator_code
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    POW = 4,
    LN  = 5,
    COS = 6,
    SIN = 7,
    EXP = 8
};

enum type_data
{
    OP   = 1,
    VAR  = 2,
    NUM  = 3,
    SPEC = 4
};

enum error_code
{
    NO_ERROR            = 0,
    SYNTAX_ERROR        = 1
};

enum priority_t
{
    ZERO_PRIORITY   = 0,
    FIRST_PRIORITY  = 1,
    SECOND_PRIORITY = 2,
    THIRD_PRIORITY  = 3
};

union data_union
{
    double number;
    int var_number;
    operator_code op;
};

struct node_value
{
    type_data type;
    data_union data_t;
};

struct node_t
{
    node_value* value;
    node_t* right;
    node_t* left;
};

struct operator_t
{
    operator_code code;
    const char* name;
    const char* design;
    size_t strlen;
    node_t* (*differentiate)(node_t* node);
    bool is_one_arg;
    priority_t priority;

};

struct variable_t
{
    int number;
    char* name;
    size_t length;
};

node_t* create_node(const type_data type, data_union data, node_t* left, node_t* right);
void destroy_node(node_t* node);

void variables_destroy(variable_t** variables);

node_t* copy_node(node_t* node);
node_t* simplify_tree(node_t* node, bool* simplifications_ptr);
bool is_close_to_zero (double number_being_checked);

node_t* differentiate(node_t* node);

extern const operator_t operators_array[];

#endif //DIFFERENTIATOR_H
