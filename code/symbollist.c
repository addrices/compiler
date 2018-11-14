#include "m.h"

func_node* FUNC_LIST[__FUNC_HASH_NUM];
var_node* VAR_LIST[__FUNC_HASH_NUM];
struct_node* STRUCT_LIST[__FUNC_HASH_NUM];
int hash(char *a);
bool func_node_add(func_node *current);
bool func_node_search(func_node* current,char *name);
bool var_node_add(var_node *current);
bool var_node_search(var_node* current,char *name);
bool struct_node_add(struct_node *current);
bool struct_node_search(struct_node* current,char *name);

int hash(char *a){
    int b = 0;
    for(int i = 0; i < strlen(a); i++){
        b = b * 33 + a[i];
    }  
    return b % __FUNC_HASH_NUM;
}

bool func_node_add(func_node *current){
    func_node* empty = NULL;
    if(func_node_search(empty,current->name) == true)
        return false;
    int num = hash(current->name);
    func_node *begin = FUNC_LIST[num];
    if(begin == NULL){
        FUNC_LIST[num] = current;
        current->next = NULL;
    }
    else{
        func_node *point = begin;
        while(point->next != NULL)
            point = point->next;
        point->next = current;
        current->next = NULL;
    }
    return true;
}

bool func_node_search(func_node* current,char *name){
    int b = hash(name);
    func_node* point = FUNC_LIST[b];
    while(point != NULL){
        if(strcmp(point->name,name) == 0){
            current = point;
            return true;
        }
        point = point->next;
    }
    current = NULL;
    return false;
}

bool var_node_add(var_node *current){
    var_node* empty = NULL;
    if(var_node_search(empty,current->name) == true)    //当其中有相同名字的则加入失败。
        return false;
    int num = hash(current->name);
    var_node *begin = VAR_LIST[num];
    if(begin == NULL){
        VAR_LIST[num] = current;
        current->next = NULL;
    }
    else{
        var_node *point = begin;
        while(point->next != NULL)
            point = point->next;
        point->next = current;
        current->next = NULL;
    }
    return true;
}

bool var_node_search(var_node* current,char *name){
    int b = hash(name);
    var_node* point = VAR_LIST[b];
    while(point != NULL){
        if(strcmp(point->name,name) == 0){
            current = point;
            return true;
        }
        point = point->next;
    }
    current = NULL;
    return false;
}

bool struct_node_add(struct_node *current){
    struct_node* empty = NULL;
    if(struct_node_search(empty,current->name) == true)
        return false;
    int num = hash(current->name);
    struct_node *begin = STRUCT_LIST[num];
    if(begin == NULL){
        STRUCT_LIST[num] = current;
        current->next = NULL;
    }
    else{
        struct_node *point = begin;
        while(point->next != NULL)
            point = point->next;
        point->next = current;
        current->next = NULL;
    }
    return true;
}

bool struct_node_search(struct_node* current,char *name){
    int b = hash(name);
    struct_node* point = STRUCT_LIST[b];
    while(point != NULL){
        if(strcmp(point->name,name) == 0){
            current = point;
            return true;
        }
        point = point->next;
    }
    current = NULL;
    return false;
}
#ifdef __DEBUG
void print_func(func_node* func);
void print_type(type* kind);
void print_flist(field_list* flist);
void print_var(var_node* var);
void print_struct(struct_node* point);
void print_funclist(){
    func_node* point;
    for(int i = 0; i < __FUNC_HASH_NUM; i++){
        point = FUNC_LIST[i];
        while(point != NULL){
            print_func(point);
            point = point->next;
        }
    }
    return;
}

void print_func(func_node* func){
    printf("funcname %s  return ",func->name);
    print_type(func->return_type);
    printf("\n");
    print_flist(func->list);
    return;
}

void print_type(type* kind){
    if(kind->kind == BASIC){
        if(kind->u.basic = 1)
            printf("Int    ");
        else
            printf("Float    ");
    }
    else if(kind->kind = ARRAY){
        printf("ARRAY   ");
    }
    else{
        printf("STRUCTURE");
    }
    return;
}

void print_flist(field_list* flist){
    field_list* point = flist;
    while(flist != NULL){
        printf("flist\n");
        print_var(point->var);
        point = point->next;
    }
    return;
}

void print_var(var_node* var){
    printf("varname  ");
    print_type(var->kind);
    printf(" %s\n",var->name);
}

void print_structlist(){
    struct_node* point = NULL;
    for(int i = 0; i < __FUNC_HASH_NUM; i++){
        point = STRUCT_LIST[i];
        while(point != NULL){
            print_struct(point);
            point = point->next;
        }
    }
}

void print_struct(struct_node* point){
    printf("struct %s\n",point->name);
    print_flist(point->list);
}

void print_varlist(){
    var_node* point = NULL;
    for(int i = 0; i < __FUNC_HASH_NUM; i++){
        point = VAR_LIST[i];
        while(point != NULL){
            print_var(point);
            point = point->next;
        }
    }
}
#endif