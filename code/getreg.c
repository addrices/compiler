#include "m.h"

#ifdef  __LAB4_ENABLE

//将var_info初始化

void infolist_init(){
    var_head = NULL;
    for(int i = 0; i < __FUNC_HASH_NUM; i++){
        if(VAR_LIST[i] == NULL){
            continue;
        }
        var_node* current = VAR_LIST[i];
        while(current != NULL){
            if(current->kind == &INT_type){
                var_info* new_info = (var_info*)malloc(sizeof(var_info));
                new_info->if_new = true;
                new_info->if_reg = false;
                strcpy(new_info->name, current->name);
                var_head->reg_num = 0;
                if(var_head == NULL){
                    var_head = new_info;
                }
            }
            current = current->next;
        }
    }
}

var_info* search_varinfo(char* var_name){
    var_node* current = var_head;
    while(current != NULL){
        if(strcmp(current->name, var_name)==0)
            return current;
        current = current->next;
    }
    printf("there is no such var");
    assert(0);
}

//获得具体的reg，如果没有会添加load指令进去。type = 1是指会被赋值，type = 2是只是取一次数
int getreg(FILE* f, char* var_name,int type){
    var_info* var = search_varinfo(var_name);
    if(var->if_reg == true){
        if(type == 2){
            return var->reg_num;
        }
        else{
            var->if_new = false;
            return var->reg_num;
        }
    }
    else{
        if(type == 2){
        }
        else{

        }
    }
}

//当遇到一个新的block的时候将内存中不新的变量更新一哈,且将上一个块的内存变量放回内存中。
void update_newblock(FILE* f){

}


#endif