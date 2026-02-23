#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

#include "differentiator.h"
#include "font.h"
#include "macros.h"

#define CR              copy_node(node->right)
#define CL              copy_node(node->left)
#define DR              dif(node->right)
#define DL              dif(node->left)

#define RIGHT_IS_NUMBER     node->right->value->type == NUM
#define LEFT_IS_NUMBER      node->left->value->type == NUM
#define RIGHT_VALUE         node->right->value->data_t.number
#define LEFT_VALUE          node->left->value->data_t.number
#define OPERATION           node->value->data_t.op

node_t* dif(node_t* node)
{
    if (!node) return nullptr;

    switch(node->value->type)
    {
        case OP: return operators_array[(int)node->value->data_t.op].dif(node);
        case NUM: return dif_num(node);
        case VAR: return dif_var(node);
        default:
            return nullptr;
    }
}

node_t* dif_var(node_t* node)
{
    assert(node);
    return NUM_(1);
}

node_t* dif_num(node_t* node)
{
    assert(node);
    return NUM_(0);
}

node_t* dif_add(node_t* node)
{
    assert(node);
    return ADD_(DL, DR);
}

node_t* dif_sub(node_t* node)
{
    assert(node);
    return SUB_(DL, DR);
}

node_t* dif_mul(node_t* node)
{
    assert(node);
    return ADD_(MUL_(DL, CR), MUL_(CL, DR));
}

node_t* dif_div(node_t* node)
{
    assert(node);
    return DIV_(SUB_(MUL_(DL, CR), MUL_(CL, DR)), POW_(CR, NUM_(2)));
}

node_t* dif_ln(node_t* node)
{
    assert(node);
    return DIV_(DL, CL);
}

node_t* dif_cos(node_t* node)
{
    assert(node);
    return MUL_(MUL_(SIN_(CL), NUM_(-1)), DL);
}

node_t* dif_sin(node_t* node)
{
    assert(node);
    return MUL_(COS_(CL), DL);
}

node_t* dif_exp(node_t* node)
{
    assert(node);
    return MUL_(EXP_(CL), DL);
}

node_t* dif_pow(node_t* node)
{
    assert(node);

    if (node->left->value->type == NUM && node->right->value->type == NUM)
        return NUM_(0);

    else if (node->left->value->type != NUM && node->right->value->type == NUM)
        return MUL_(MUL_(POW_(CL, NUM_(node->right->value->data_t.number - 1)), CR), DL);

    else if (node->left->value->type == NUM && node->right->value->type != NUM)
        return MUL_(EXP_(MUL_(CR, LN_(CL))), MUL_(DR, LN_(CL)));

    else
        return MUL_(EXP_(MUL_(CR, LN_(CL))), ADD_(MUL_(CR, MUL_(DIV_(NUM_(1), CL), DL)), MUL_(LN_(CL), DR)));
}

node_t* copy_node(node_t* node)
{
    if (!node) return nullptr;

    node_t* new_node = (node_t*) calloc(1, sizeof(node_t));
    assert(new_node);

    new_node->value = (node_value*) calloc(1, sizeof(node_value));
    assert(new_node->value);

    new_node->value->type = node->value->type;

    switch(new_node->value->type)
    {
        case OP:
            new_node->value->data_t.op = node->value->data_t.op;
            break;

        case VAR:
            new_node->value->data_t.variable = strdup(node->value->data_t.variable);
            assert(new_node->value->data_t.variable);
            break;

        case NUM:
            new_node->value->data_t.number = node->value->data_t.number;
            break;

        default:
            break;
    }

    if (node->left)
        new_node->left = CL;
    else
        new_node->left = nullptr;

    if (node->right)
        new_node->right = CR;
    else
        new_node->right = nullptr;

    return new_node;
}

node_t* create_node(const type_data type, data_union data, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    assert(node);

    node->value = (node_value*) calloc(1, sizeof(node_value));
    assert(node->value);

    node->value->type = type;
    switch(type)
    {
        case OP:
            node->value->data_t.op = data.op;
            break;

        case VAR:
            node->value->data_t.variable = strdup(data.variable);
            assert(node->value->data_t.variable);
            break;

        case NUM:
            node->value->data_t.number = data.number;
            break;

        default:
            break;
    }

    node->left = left;
    node->right = right;

    return node;
}

void destroy_node(node_t* node)
{
    if (!node) return;

    destroy_node(node->left);
    destroy_node(node->right);

    if (node->value)
    {
        if (node->value->type == VAR)
            free((void*)node->value->data_t.variable);

        free(node->value);
    }

    free(node);
}
