#include <math.h>

#include "differentiator.h"
#include "macros.h"

node_t* simplify_node(node_t* node, bool* simplifications_ptr)
{
    if (!node) return nullptr;
    if (!node->left || !node->right) return node;

    switch(node->value->type)
    {
        case OP:
            if (LEFT_IS_NUMBER && RIGHT_IS_NUMBER)
            {
                *simplifications_ptr = true;
                double right_number = RIGHT_VALUE;
                double left_number  = LEFT_VALUE;
                operator_code op_code = NODE_OPERATION;
                destroy_node(node);

                switch(op_code)
                {
                    case ADD: return NUM_(left_number + right_number);
                    case SUB: return NUM_(left_number - right_number);
                    case MUL: return NUM_(remove_minus_before_zero(left_number * right_number));
                    case DIV: return NUM_(remove_minus_before_zero(left_number / right_number));
                    case POW: return NUM_(pow(left_number, right_number));
                    case LN:
                    case COS:
                    case SIN:
                    case EXP:
                    default:  break;
                }
            }
            switch(NODE_OPERATION)
            {
                case MUL:
                    if ((RIGHT_IS_NUMBER && is_close_to_zero(RIGHT_VALUE)) || (LEFT_IS_NUMBER && is_close_to_zero(LEFT_VALUE)))
                    {
                        *simplifications_ptr = true;
                        destroy_node(node);
                        return NUM_(0);
                    }

                    else if (RIGHT_IS_NUMBER && is_close_to_zero(RIGHT_VALUE - 1))
                    {
                        *simplifications_ptr = true;
                        node_t* left_node = CL;
                        destroy_node(node);
                        return left_node;
                    }

                    else if (LEFT_IS_NUMBER && is_close_to_zero(LEFT_VALUE - 1))
                    {
                        *simplifications_ptr = true;
                        node_t* right_node = CR;
                        destroy_node(node);
                        return right_node;
                    }
                    break;

                case POW:
                    if (RIGHT_IS_NUMBER && is_close_to_zero(RIGHT_VALUE - 1))
                    {
                        *simplifications_ptr = true;
                        node_t* left_node = CL;
                        destroy_node(node);
                        return left_node;
                    }
                    break;

                case ADD:
                    if ((RIGHT_IS_NUMBER && is_close_to_zero(RIGHT_VALUE)))
                    {
                        *simplifications_ptr = true;
                        node_t* left_node = CL;
                        destroy_node(node);
                        return left_node;
                    }

                    else if ((LEFT_IS_NUMBER && is_close_to_zero(LEFT_VALUE)))
                    {
                        *simplifications_ptr = true;
                        node_t* right_node = CR;
                        destroy_node(node);
                        return right_node;
                    }
                    break;

                case DIV:
                    if (LEFT_IS_NUMBER && is_close_to_zero(LEFT_VALUE))
                    {
                        *simplifications_ptr = true;
                        destroy_node(node);
                        return NUM_(0);
                    }
                    break;
                case SUB:
                    if ((RIGHT_IS_NUMBER && is_close_to_zero(RIGHT_VALUE)))
                    {
                        *simplifications_ptr = true;
                        node_t* left_node = CL;
                        destroy_node(node);
                        return left_node;
                    }

                    else if ((LEFT_IS_NUMBER && is_close_to_zero(LEFT_VALUE)))
                    {
                        *simplifications_ptr = true;
                        node_t* right_node = CR;
                        destroy_node(node);
                        return MUL_(NUM_(-1), right_node);
                    }
                case LN:
                case COS:
                case SIN:
                case EXP:
                default:
                    break;
            }
        case VAR:
        case NUM:
        case SPEC:
        default:
            break;
    }
    node->left  = simplify_node(node->left, simplifications_ptr);
    node->right = simplify_node(node->right, simplifications_ptr);

    return node;
}

double remove_minus_before_zero (double number_being_checked)
{
    if (is_close_to_zero(number_being_checked))
        number_being_checked = fabs(number_being_checked);

    return number_being_checked;
}
