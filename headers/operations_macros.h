#ifndef OPERATIONS_MACROS_H
#define OPERATIONS_MACROS_H

#define ADD_(left_node, right_node) create_node(OP, ADD, left_node, right_node)
#define SUB_(left_node, right_node) create_node(OP, SUB, left_node, right_node)
#define MUL_(left_node, right_node) create_node(OP, MUL, left_node, right_node)
#define DIV_(left_node, right_node) create_node(OP, DIV, left_node, right_node)
#define POW_(left_node, right_node) create_node(OP, POW, left_node, right_node)
#define EXP_(left_node)             create_node(OP, EXP, left_node, nullptr)
#define LN_(left_node)              create_node(OP, LN,  left_node, nullptr)
#define SIN_(left_node)             create_node(OP, SIN, left_node, nullptr)
#define COS_(left_node)             create_node(OP, COS, left_node, nullptr)
#define NUM_(value)                 create_node(NUM, value, nullptr, nullptr)

#endif // OPERATIONS_MACROS_H
