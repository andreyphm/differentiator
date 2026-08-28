#ifndef DUMP_H
#define DUMP_H

#include "tokenization.h"

#define TREE_DUMP_TXT "source/tree_dump/tree_dump.txt"
#define TREE_DUMP_SVG "source/tree_dump/tree_dump.svg"
#define LIST_DUMP_TXT "source/list_dump/list_dump.txt"
#define LIST_DUMP_SVG "source/list_dump/list_dump.svg"

void tree_dump(node_t* const node, const variable_t* const variables);
void list_dump(list_t* const list, const variable_t* const variables);

#endif // DUMP_H
