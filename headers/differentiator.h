#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

typedef enum operator_codes_data
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    LN  = 4,
    COS = 5,
    SIN = 6,
    EXP = 7,
    POW = 8
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
    node_t* (*dif) (node_t* node);
    bool is_one_arg;
} operator_t;

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
node_t* dif_ln (node_t* node);
node_t* dif_cos(node_t* node);
node_t* dif_sin(node_t* node);
node_t* dif_exp(node_t* node);
node_t* dif_pow(node_t* node);

const operator_t operators_array[] =
{
    {ADD, "ADD", "+",   dif_add, false},
    {SUB, "SUB", "-",   dif_sub, false},
    {MUL, "MUL", "*",   dif_mul, false},
    {DIV, "DIV", "/",   dif_div, false},
    {LN,  "LN" , "ln",  dif_ln , true },
    {COS, "COS", "cos", dif_cos, true },
    {SIN, "SIN", "sin", dif_sin, true },
    {EXP, "EXP", "e",   dif_exp, true },
    {POW, "POW", "^",   dif_pow, false}
};

#endif //DIFFERENTIATOR_H
