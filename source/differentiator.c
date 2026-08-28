#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

#include "differentiator.h"
#include "font.h"
#include "macros.h"

static node_t* dif_num(node_t* node);
static node_t* dif_var(node_t* node);
static node_t* dif_add(node_t* node);
static node_t* dif_sub(node_t* node);
static node_t* dif_mul(node_t* node);
static node_t* dif_div(node_t* node);
static node_t* dif_ln(node_t* node);
static node_t* dif_cos(node_t* node);
static node_t* dif_sin(node_t* node);
static node_t* dif_exp(node_t* node);
static node_t* dif_pow(node_t* node);

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

node_t* differentiate(node_t* node)
{
    if (!node) return nullptr;

    switch(NODE_TYPE)
    {
        case OP: return operators_array[(int)NODE_OPERATION].differentiate(node);
        case NUM: return dif_num(node);
        case VAR: return dif_var(node);
        case SPEC:
        default:
            return nullptr;
    }
}

static node_t* dif_var(node_t* node)
{
    assert(node);
    return NUM_(1);
}

static node_t* dif_num(node_t* node)
{
    assert(node);
    return NUM_(0);
}

static node_t* dif_add(node_t* node)
{
    assert(node);
    return ADD_(DL, DR);
}

static node_t* dif_sub(node_t* node)
{
    assert(node);
    if (node->left)
        return SUB_(DL, DR);
    else
        return MUL_(NUM_(-1), DR);
}

static node_t* dif_mul(node_t* node)
{
    assert(node);
    return ADD_(MUL_(DL, CR), MUL_(CL, DR));
}

static node_t* dif_div(node_t* node)
{
    assert(node);
    return DIV_(SUB_(MUL_(DL, CR), MUL_(CL, DR)), POW_(CR, NUM_(2)));
}

static node_t* dif_ln(node_t* node)
{
    assert(node);
    return DIV_(DL, CL);
}

static node_t* dif_cos(node_t* node)
{
    assert(node);
    return MUL_(MUL_(NUM_(-1), SIN_(CL)), DL);
}

static node_t* dif_sin(node_t* node)
{
    assert(node);
    return MUL_(COS_(CL), DL);
}

static node_t* dif_exp(node_t* node)
{
    assert(node);
    return MUL_(EXP_(CL), DL);
}

static node_t* dif_pow(node_t* node)
{
    assert(node);

    if (LEFT_IS_NUMBER && RIGHT_IS_NUMBER)
        return NUM_(0);

    else if (!LEFT_IS_NUMBER && RIGHT_IS_NUMBER)
        return MUL_(MUL_(POW_(CL, NUM_(RIGHT_VALUE - 1)), CR), DL);

    else if (LEFT_IS_NUMBER && !RIGHT_IS_NUMBER)
        return MUL_((POW_(CL, CR)), MUL_(DR, LN_(CL)));

    else
        return MUL_(POW_(CL, CR), ADD_(MUL_(CR, MUL_(DIV_(NUM_(1), CL), DL)), MUL_(LN_(CL), DR)));
}

node_t* copy_node(node_t* node)
{
    if (!node) return nullptr;

    node_t* new_node = (node_t*) calloc(1, sizeof(node_t));
    assert(new_node);

    new_node->value = (node_value*) calloc(1, sizeof(node_value));
    assert(new_node->value);

    new_node->value->type = NODE_TYPE;

    switch(new_node->value->type)
    {
        case OP:
            new_node->value->data_t.op = node->value->data_t.op;
            break;

        case VAR:
            new_node->value->data_t.var_number = node->value->data_t.var_number;
            break;

        case NUM:
            new_node->value->data_t.number = node->value->data_t.number;
            break;

        case SPEC:
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

    NODE_TYPE = type;
    switch(type)
    {
        case OP:
            node->value->data_t.op = data.op;
            break;

        case VAR:
            node->value->data_t.var_number = data.var_number;
            break;

        case NUM:
            node->value->data_t.number = data.number;
            break;

        case SPEC:
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
        free(node->value);

    free(node);
}

bool is_close_to_zero (double number_being_checked)
{
    return (fabs(number_being_checked) < NUMBER_CLOSE_TO_ZERO);
}
