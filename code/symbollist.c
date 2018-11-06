#include "m.h"

void func_node_add(struct func_node *current){
    if(func_begin == NULL){
        current->next = current;
        current->pre = current;
        func_begin = current;
    }
    else{
        current->pre = func_begin->pre
        current->next = func_begin;
        func_begin->pre->next = current;
        func_begin->pre = current;
    }
}