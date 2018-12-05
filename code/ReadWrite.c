#include "m.h"

#ifdef __LAB3_ENABLE
void init_symbol(){     //初始化read，write函数
    func_node *read = (func_node*)malloc(sizeof(func_node));
    strcpy(read->name, "read");
    read->return_type = &INT_type;
    read->next = NULL;
    read->list = NULL;

    func_node *write = (func_node*)malloc(sizeof(func_node));    
    strcpy(write->name, "write");
    write->return_type = &INT_type;
    write->next = NULL;
    field_list* field = (field_list*)malloc(sizeof(field_list));
    write->list = field;
    field->next = NULL;
    var_node* var = (var_node*)malloc(sizeof(var_node));
    field->var = var;
    var->kind = &INT_type;
    strcpy(var->name, "write_num");
    var->next = NULL;
    
    if(!func_node_add(read)){
        assert(0);
    }
    if(!func_node_add(write)){
        assert(0);
    }
}
#endif