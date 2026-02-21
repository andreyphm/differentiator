#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "differentiator.h"
#include "font.h"

#define CR      copy_node(node->right)
#define CL      copy_node(node->left)
#define DR      dif(node->right)
#define DL      dif(node->left)
#define DCR     dif(copy_node(node->right))
#define DCL     dif(copy_node(node->left))

#define ADD_(left_node, right_node) create_node(OP, ADD, left_node, right_node)
#define SUB_(left_node, right_node) create_node(OP, SUB, left_node, right_node)
#define MUL_(left_node, right_node) create_node(OP, MUL, left_node, right_node)
#define DIV_(left_node, right_node) create_node(OP, DIV, left_node, right_node)

node_t* dif(node_t* node)
{
    if (!node) return nullptr;

    switch(node->value->type)
    {
        case OP:
            switch(node->value->data_t.op)
            {
                case ADD: return dif_add(node);
                case SUB: return dif_sub(node);
                case MUL: return dif_mul(node);
                case DIV: return dif_div(node);
                default:
                    return nullptr;
            }
        case NUM: return dif_num(node);
        case VAR: return dif_var(node);
        default:
            return nullptr;
    }
}

node_t* dif_var(node_t* node)
{
    assert(node);
    destroy_node(node);
    return create_node(NUM, 1, nullptr, nullptr);
}

node_t* dif_num(node_t* node)
{
    assert(node);
    destroy_node(node);
    return create_node(NUM, 0, nullptr, nullptr);
}

node_t* dif_add(node_t* node)
{
    assert(node);
    return ADD_(DCL, DCR);
}

node_t* dif_sub(node_t* node)
{
    assert(node);
    return SUB_(DCL, DCR);
}

node_t* dif_mul(node_t* node)
{
    assert(node);
    return ADD_(MUL_(DL, CR), MUL_(CL, DR));
}

node_t* dif_div(node_t* node)
{
    assert(node);
    return DIV_(SUB_(MUL_(DL, CR), MUL_(CL, DR)), MUL_(CR, CR));
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
            new_node->value->data_t.variable = node->value->data_t.variable;
            break;

        case NUM:
            new_node->value->data_t.number = node->value->data_t.number;
            break;

        default:
            break;
    }

    new_node->left = CL;
    new_node->right = CR;

    return new_node;
}

node_t* create_node(const type_data type, double data, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    assert(node);

    node->value = (node_value*) calloc(1, sizeof(node_value));
    assert(node->value);

    node->value->type = type;
    switch(type)
    {
        case OP:
            node->value->data_t.op = (operator_code) data;
            break;

        case VAR:
            node->value->data_t.variable = (char) data;
            break;

        case NUM:
            node->value->data_t.number = data;
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

    free(node->value);
    free(node);
}

