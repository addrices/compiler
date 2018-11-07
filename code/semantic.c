#include "m.h"

void solve_ExtDef(struct tree_node* root);

void analy_tree(struct tree_node* root_node){
    if(root_node == NULL)
        return;
    else if(strcmp(root_node->n_type, "ExtDef") == 0)
        solve_ExtDef(root_node);
    analy_tree(root_node->first_child);
    analy_tree(root_node->next_brother);
    return;
}

void solve_ExtDef(struct tree_node* root){
    if(strcmp(root->first_child->next_brother->n_type, "FunDec") == 0)
        printf("funcDef\n");
    else if(strcmp(root->first_child->next_brother->n_type, "SEMI") == 0)
        printf("struct def");
    return;
}