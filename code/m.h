#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define __LAB1_ENABLE
#define __LAB2_ENABLE


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
#define __MAX_NAME_LENGTH 16
#define __MAX_FUNC_PNUM 7
#define __FUNC_HASH_NUM 29

struct func_node{
    char name[__MAX_NAME_LENGTH];
    struct func_node* next;
    char return_type;//int i float f struct s
    struct struct_node* re_struct;//if struct point it, if not it's NULL
    int parameter_num;
    struct var_node **parameters;//point it's parameter's point
};
struct struct_node{
    char name[__MAX_NAME_LENGTH];
    struct struct_node *next;
    int member_num;//成员数量
    struct var_node **members;
};
struct var_node{
    char name[__MAX_NAME_LENGTH];
    char type;//int i float f struct s array a
    struct struct_node *struct_type;//如果是struct类型的话，指向对应类型的struct_node
    struct array_node* array;
};
struct array_node{
    char type;//int i float f struct s array a
    struct struct_node *struct_type;
    struct array_node* array;
};
void analy_tree(struct tree_node* root_node);
struct func_node* func_list[__FUNC_HASH_NUM];
void func_node_add(struct func_node *current);
struct func_node* func_node_search(char *name);
#endif
struct tree_node *root;
void read_tree(struct tree_node* root_node,int i);
bool ProFlag;