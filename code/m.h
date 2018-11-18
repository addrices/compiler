#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
//#define __LAB1_ENABLE
#define __LAB2_ENABLE
//#define __DEBUG


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
extern int error_line;
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
typedef struct var_node_ var_node;
typedef struct field_list_ field_list;
typedef struct func_plist_ func_plist;
typedef struct error2_node_ error2_node;

struct error2_node_{
    int num;
    int length;
    char info[64];
    error2_node* next;
};
struct field_list_{
    var_node* var;
    field_list* next;
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
    func_node* next;
    type* return_type;
    field_list* list;
};
struct struct_node_{
    char name[__MAX_NAME_LENGTH];
    struct_node *next;
    field_list* list;
};
struct var_node_{
    char name[__MAX_NAME_LENGTH];
    type* kind;//int i float f struct s array a
    var_node* next;
};
extern type INT_type;
extern type FLOAT_type;
extern error2_node* error2_line;
void analy_Program(struct tree_node* root_node);
bool func_node_add(func_node *current);
func_node* func_node_search(char *name);
bool var_node_add(var_node *current);
var_node* var_node_search(char *name);
bool struct_node_add(struct_node *current);
struct_node* struct_node_search(char *name);
void print_error2s();
void error2_node_add(error2_node* current);

#ifdef __DEBUG
void print_varlist();
void print_structlist();
void print_funclist();
#endif
#endif
extern struct tree_node *root;
void read_tree(struct tree_node* root_node,int i);
extern bool ProFlag;