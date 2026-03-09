#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

const double NUMBER_CLOSE_TO_ZERO  = 10e-12;
const size_t LAST_CHAR_OP_NUM = 4;
const size_t FIRST_FUNC_NUM   = 5;
const size_t LAST_FUNC_NUM    = 8;

typedef enum operator_codes_data
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
} operator_code;

typedef enum type_data
{
    OP   = 1,
    VAR  = 2,
    NUM  = 3,
    SPEC = 4
} type_data;

typedef enum error_code
{
    NO_ERROR = 0,
    SYNTAX_ERROR = 1
} error_code;

typedef enum priority_t
{
    ZERO_PRIORITY   = 0,
    FIRST_PRIORITY  = 1,
    SECOND_PRIORITY = 2,
    THIRD_PRIORITY  = 3
} priority_t;

typedef union
{
    double number;
    int var_number;
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
    size_t strlen;
    node_t* (*dif) (node_t* node);
    bool is_one_arg;
    priority_t priority;

} operator_t;

typedef struct variable_t
{
    int number;
    char* name;
    size_t length;
} variable_t;

node_t* create_node(const type_data type, data_union data, node_t* left, node_t* right);
void destroy_node(node_t* node);

const char* enum_to_string(type_data type);
void variables_destroy(variable_t** variables);

node_t* copy_node(node_t* node);
node_t* simplify_node(node_t* node, bool* simplifications_ptr);
bool is_close_to_zero (double number_being_checked);
double remove_minus_before_zero (double number_being_checked);

node_t* dif(node_t* node);
node_t* dif_num(node_t* node);
node_t* dif_var(node_t* node);
node_t* dif_add(node_t* node);
node_t* dif_sub(node_t* node);
node_t* dif_mul(node_t* node);
node_t* dif_div(node_t* node);
node_t* dif_ln (node_t* node);
node_t* dif_cos(node_t* node);
node_t* dif_sin(node_t* node);
node_t* dif_exp(node_t* node);
node_t* dif_pow(node_t* node);

const operator_t operators_array[] =
{
    {ADD, "ADD", "+",   1, dif_add, false, SECOND_PRIORITY},
    {SUB, "SUB", "-",   1, dif_sub, false, SECOND_PRIORITY},
    {MUL, "MUL", "*",   1, dif_mul, false, FIRST_PRIORITY},
    {DIV, "DIV", "/",   1, dif_div, false, FIRST_PRIORITY},
    {POW, "POW", "^",   1, dif_pow, false, ZERO_PRIORITY},
    {LN,  "LN" , "ln",  2, dif_ln , true, THIRD_PRIORITY},
    {COS, "COS", "cos", 3, dif_cos, true, THIRD_PRIORITY},
    {SIN, "SIN", "sin", 3, dif_sin, true, THIRD_PRIORITY},
    {EXP, "EXP", "exp", 3, dif_exp, true, THIRD_PRIORITY}
};

#endif //DIFFERENTIATOR_H
