#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define __LAB1_ENABLE
#define __LAB2_ENABLE
#define __MAX_NAME_LENGTH 16

/*lab1 syntax.y*/
union n_un{
    int n_value_i;
    float n_value_f;
    char* a;
};
struct tree_node{
    struct tree_node* first_child;
    struct tree_node* next_brother;
    char n_type[32];
    int n_length;
    union n_un n_value;
    bool if_lex;
};
int error_line;
struct tree_node* create_node(char* a, int length, union n_un value,bool flag);
void add_child(struct tree_node* a, struct tree_node* b);
void add_sibling(struct tree_node* a, struct tree_node* b);
void read_tree(struct tree_node* root_node,int i);

/*lab2*/
#ifdef __LAB2_ENABLE
struct func_node{
    char name[__MAX_NAME_LENGTH];  //name
    int parameter_num;//参数数量
    char *parameters;//int a float b struct c
};
struct struct_member_node{
    char name[__MAX_NAME_LENGTH];
    char type; //int a float b struct c
};
struct struct_node{
    char name[__MAX_NAME_LENGTH];
    int member_num;//成员数量
    struct struct_member_node *members;//成员列表
};
struct var_node{
    char name[__MAX_NAME_LENGTH];
    char type;//int a float b struct c
    struct struct_node *struct_type;//如果是struct类型的话，指向对应类型的struct_node
};
#endif
struct func_node *func_begin;
struct tree_node *root;
void read_tree(struct tree_node* root_node,int i);
bool ProFlag;