#ifndef OPERATIONS_MACROS_H
#define OPERATIONS_MACROS_H

#define ADD_(left_node, right_node) create_node(OP,  (data_union){.op = ADD}, left_node, right_node)
#define SUB_(left_node, right_node) create_node(OP,  (data_union){.op = SUB}, left_node, right_node)
#define MUL_(left_node, right_node) create_node(OP,  (data_union){.op = MUL}, left_node, right_node)
#define DIV_(left_node, right_node) create_node(OP,  (data_union){.op = DIV}, left_node, right_node)
#define POW_(left_node, right_node) create_node(OP,  (data_union){.op = POW}, left_node, right_node)
#define EXP_(left_node)             create_node(OP,  (data_union){.op = EXP}, left_node, nullptr)
#define LN_(left_node)              create_node(OP,  (data_union){.op = LN},  left_node, nullptr)
#define SIN_(left_node)             create_node(OP,  (data_union){.op = SIN}, left_node, nullptr)
#define COS_(left_node)             create_node(OP,  (data_union){.op = COS}, left_node, nullptr)
#define NUM_(value)                 create_node(NUM, (data_union){.number = (value)}, nullptr, nullptr)
#define VAR_(value)                 create_node(VAR, (data_union){.variable = (value)}, nullptr, nullptr)

#endif // OPERATIONS_MACROS_H
