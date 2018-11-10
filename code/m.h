#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#define __LAB1_ENABLE
#define __LAB2_ENABLE
#define __DEBUG


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
#define __MAX_NAME_LENGTH 32
#define __MAX_FUNC_PNUM 7
#define __FUNC_HASH_NUM 29
typedef struct type_ type;
typedef struct func_node_ func_node;
typedef struct struct_node_ struct_node;
typedef struct var_node var_node;
typedef struct field_list_ field_list;
typedef struct func_plist_ func_plist;

struct field_list_{
    char name[__MAX_NAME_LENGTH];
    type* kind;
    field_list* tail;
};
struct func_plist_{
    var_node* var;
    func_plist* next;
};
struct type_{
    enum {BASIC, ARRAY, STRUCTURE} kind;
    union{
        int basic;//1 int 2 float
        struct {type* elem; int size;} array;
        struct_node* structure;
    }u;
};
struct func_node_{
    char name[__MAX_NAME_LENGTH];
    struct func_node* next;
    type* return_type;
    func_plist* list;
};
struct struct_node_{
    char name[__MAX_NAME_LENGTH];
    struct struct_node *next;
    field_list* list;
};
struct var_node_{
    char name[__MAX_NAME_LENGTH];
    type kind;//int i float f struct s array a
    var_node* next;
};

//bool search_struct(struct struct_node* current, char *name);
void analy_Program(struct tree_node* root_node);
//struct func_node* func_list[__FUNC_HASH_NUM];
//void func_node_add(struct func_node *current);
//struct func_node* func_node_search(char *name);
#endif
struct tree_node *root;
void read_tree(struct tree_node* root_node,int i);
bool ProFlag;