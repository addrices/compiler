#include "m.h"

#ifdef  __LAB4_ENABLE
struct var_info_{
    bool if_new;        //当前内存中是否已经是最新的。
    bool if_reg;        //是否已经在寄存器中了。
    char name[__MAX_NAME_LENGTH];
    int reg_num;        //在哪个寄存器中。
    struct var_info_* next;    //链表组织起来
};
typedef struct var_info_ var_info;

var_info* var_head;

//将var_info初始化
void infolist_init(){
    var_head = NULL;
    for(int i = 0; i < __FUNC_HASH_NUM; i++){
        if(VAR_LIST[i] == NULL){
            continue;
        }
        var_node* current = VAR_LIST[i];
        while(current != NULL){
            var_info* new_info = (var_info*)malloc(sizeof(var_info));
            new_info->if_new = true;
            new_info->if_reg = false;
            strcpy(new_info->name, current->name);
            var_head->reg_num = 0;
            if(var_head == NULL){
                var_head = new_info;
            }

        }
    }
}

//获得具体的reg，如果没有会添加load指令进去。
int getreg(FILE* f, char* var_name){

}

//当遇到一个新的block的时候将内存中不新的变量更新一哈
void update_newblock(FILE* f){

}


#endif