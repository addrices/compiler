#include "m.h"
/*
int hash(char *a){
    int b = 0;
    for(int i = 0; i < strlen(a); i++){
        b = b * 33 + a[i];
    }  
    return b % __FUNC_HASH_NUM;
}

bool search_struct(struct struct_node* current, char *name){
    return true;
}

void func_node_add(struct func_node *current){
    int num = hash(current->name);
    struct func_node *begin = func_list[num];
    if(begin == NULL){
        begin = current;
    }
    else{
        struct func_node *point = begin;
        while(point->next != NULL)
            point = point->next;
        point->next = current;
    }
}

struct func_node* func_node_search(char *name){
    int num = hash(name);
    struct func_node *current = func_list[num];
    while(current != NULL){
        if(strcmp(current->name,name) == 0)
            return current;
        else
            current = current->next;
    }
    return current;
}
*/
