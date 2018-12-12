#include "m.h"
#ifdef __LAB2_ENABLE
error2_node* error2_line;
void error2_node_add(error2_node* current){
    if(error2_line == NULL)
        error2_line = current;
    else{
        error2_node *point = error2_line;
        while(point->next != NULL)
            point = point->next;
        point->next = current;
    }
}

bool print_error2s(){
    error2_node* point = error2_line;
    if(point == NULL)
        return true;
    while(point != NULL){
        printf("Error type %d at line %d: %s.\n",point->num, point->length, point->info);
        point = point->next;
    }
    return false;
}

#endif