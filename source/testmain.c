#include "font.h"
#include "differentiator.h"

int main()
{
//     node_t* node_1 = create_node(OP, DIV, nullptr, nullptr);
//
//     node_1->left = create_node(OP, ADD, nullptr, nullptr);
//     node_1->left->left = create_node(VAR, 'x', nullptr, nullptr);
//     node_1->left->right = create_node(NUM, 5, nullptr, nullptr);
//
//     node_1->right = create_node(OP, ADD, nullptr, nullptr);
//     node_1->right->left = create_node(VAR, 'y', nullptr, nullptr);
//     node_1->right->right = create_node(NUM, 4, nullptr, nullptr);
//
//     node_t* dif_node_1 = dif(node_1);
//
//     tree_dump(dif_node_1, "source/dump/differentiator_tree.png");
//
//     destroy_node(dif_node_1);
//     destroy_node(node_1);

    node_t* node_1 = create_node(OP, SIN, nullptr, nullptr);

    node_1->left = create_node(OP, DIV, nullptr, nullptr);
    node_1->left->left = create_node(NUM, 1, nullptr, nullptr);
    node_1->left->right = create_node(VAR, 'x', nullptr, nullptr);

    node_t* dif_node_1 = dif(node_1);

    tree_dump(dif_node_1, "source/dump/differentiator_tree.png");

    destroy_node(dif_node_1);
    destroy_node(node_1);

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}
