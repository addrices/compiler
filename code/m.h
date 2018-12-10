#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
//#define __LAB1_ENABLE
#define __LAB2_ENABLE
//#define __DEBUG
#define __LAB3_ENABLE


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
typedef struct sfield_list_ sfield_list;

struct error2_node_{
    int num;
    int length;
    char info[64];
    error2_node* next;
};
struct sfield_list_{
    char name[16];
    type* kind;
    sfield_list* next;
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
    sfield_list* list;
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
int getexp_state(struct tree_node *root);

#ifdef __DEBUG
void print_varlist();
void print_structlist();
void print_funclist();
#endif
#endif

#ifdef __LAB3_ENABLE
void init_symbol();
typedef struct operand_ operand;
typedef struct inter_code_ inter_code;
typedef struct inter_code_page_ inter_code_page;
//extern inter_codes* begin_intercode;
//extern inter_codes* end_intercode;
struct operand_{
    enum {LABEL_op, FUNC_op, VARIABLE_op, CONSTANT_op, ADDRESS_op} kind;
    char name[16];
};
struct inter_code_{
    enum {LABEL_ic, FUNC_ic, ASSIGN_ic, ADD_ic, SUB_ic, MUL_ic, DIV_ic, GETADDR_ic, GETVALUE_ic, ADDRASS_ic, GOTO_ic, IF_ic, RETURN_ic, DEC_ic, ARG_ic, CALL_ic, PARAM_ic, READ_ic, WRITE_ic} kind;
    union {
        struct {operand src;} op1;
        struct {operand right, left;} op2;
        struct {operand result, left, right;} op3;
        struct {enum {NE,EQ,LT,LE,GT,GE} relop; operand left, right, dest;} branch;
        struct {int size; operand dest;} dec;
    };
    inter_code* pre;
    inter_code* next;
};
struct inter_code_page_{
    int num;
    inter_code* begin;
    inter_code* end;
};
//void intercode_add(inter_codes* new_code);
void intercode_print(FILE* f);
inter_code_page* intercode_ppmerge(inter_code_page* forward, inter_code_page* behind);
inter_code_page* intercode_p1merge(inter_code_page* forward, inter_code* behind);
inter_code_page* intercode_1pmerge(inter_code* forward, inter_code_page* behind);
inter_code_page* intercode_11merge(inter_code* forward, inter_code* behind);
inter_code_page* intercode_1merge(inter_code* forward);
inter_code_page* translate_stmt(struct tree_node* root);
inter_code_page* translate_exp(struct tree_node* root, operand* place);
#endif
extern struct tree_node *root;
void read_tree(struct tree_node* root_node,int i);
extern bool ProFlag;