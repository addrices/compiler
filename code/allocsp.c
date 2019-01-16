#include "m.h"

#ifdef __LAB4_ENABLE
void get_func_var(func_info* func);
void alloc_sp(func_info* func);
bool funcvar_add(func_var* head,char* n1, int size);
block_page* divide_block(func_info* icode);
void print_func(func_info* func);

func_info* divide_func(inter_code_page* icode){
    inter_code* current = icode->begin;
    int num = 0;
    func_info* func_head = (func_info*)malloc(sizeof(func_info));
    func_info* current_func = func_head;
    func_head->next_func = NULL;
    func_head->var_list = NULL;
    func_head->block_list = NULL;
    for(int i = 0; i < icode->num; i++){
        if(current->kind == FUNC_ic){
            strcpy(current_func->name, current->op1.src.name);
            current_func->begin = current;
            current_func->code_num = i;
        }
        else if(i == icode->num - 1){
            current_func->code_num = i - current_func->code_num + 1;
            current_func->end = current; 
            get_func_var(current_func);
            current_func->block_list = divide_block(current_func);
            current_func->next_func = NULL;
            current_func = current_func->next_func;
            break;
        }
        else if(current->next->kind == FUNC_ic){
            current_func->code_num = i - current_func->code_num + 1;
            current_func->end = current; 
            get_func_var(current_func);
            current_func->block_list = divide_block(current_func);
            func_info* new_func = (func_info*)malloc(sizeof(func_info));
            new_func->next_func = NULL;
            new_func->var_list = NULL;
            new_func->block_list = NULL;
            current_func->next_func = new_func;
            current_func = current_func->next_func;
        }
        current = current->next;
    }

    //print_func(func_head);
    return func_head;

}

void get_func_var(func_info* func){
    func_var* var_head = (func_var*)malloc(sizeof(func_var));
    inter_code* current = func->begin;
    int totalsize = 0;
    func->var_num = -1;
    func->var_list = var_head;
    func->var_list->size = -1;
    func->var_list->next = NULL;
    int node_num = 0;                   //这是表示这个基本块中有多少变量。
    for(int i = 0; i < func->code_num; i++){
        if(current->kind == LABEL_ic || current->kind == FUNC_ic || current->kind == GOTO_ic)
            ;
        else if(current->kind == DEC_ic){
            int size = atoi(current->op2.right.name);
            if(funcvar_add(func->var_list,current->op2.left.name,size)){
                totalsize += size;
                node_num++;
            }
        }
        else if(current->kind == CALL_ic){
            if(funcvar_add(func->var_list,current->op2.left.name,4)){
                totalsize += 4;
                node_num++;
            }
        }
        else if(current->kind == ASSIGN_ic || current->kind == GETADDR_ic || current->kind == GETVALUE_ic || current->kind == ADDRASS_ic){
            if(funcvar_add(func->var_list,current->op2.left.name,4)){
                totalsize += 4;
                node_num++;
            }
            if(funcvar_add(func->var_list,current->op2.right.name,4)){
                totalsize += 4;
                node_num++;
            }
        }
        else if(current->kind == READ_ic || current->kind == WRITE_ic || current->kind == PARAM_ic || current->kind == ARG_ic || current->kind == RETURN_ic){
            if(funcvar_add(func->var_list,current->op1.src.name,4)){
                totalsize += 4;
                node_num++;
            }
        }
        else if(current->kind == ADD_ic || current->kind == SUB_ic ||current->kind == MUL_ic ||current->kind == DIV_ic){
            if(funcvar_add(func->var_list,current->op3.result.name,4)){
                totalsize += 4;
                node_num++;
            }
            if(funcvar_add(func->var_list,current->op3.left.name,4)){
                totalsize += 4;
                node_num++;
            }
            if(funcvar_add(func->var_list,current->op3.right.name,4)){
                totalsize += 4;
                node_num++;
            }
        }
        else if(current->kind == IF_ic){
            if(funcvar_add(func->var_list,current->branch.left.name,4)){
                totalsize += 4;
                node_num++;
            }
            if(funcvar_add(func->var_list,current->branch.right.name,4)){
                totalsize += 4;
                node_num++;
            }
        }
        else{
            printf("block_graph_init error\n");
            assert(0); 
        }
        current = current->next;
    }           //第一遍扫描，获得graph_node的数量以便生成二维数组来描述图；
    func->var_num = node_num;
    func->size = totalsize;
    alloc_sp(func);
    return;
}

void alloc_sp(func_info* func){
    int shift = 0;
    func_var* current_var = func->var_list;
    for(int i = 0; i < func->var_num;i++){
        current_var->sp_shift = func->size - shift;
        shift = shift + current_var->size;
        current_var = current_var->next;
    }
}

bool funcvar_add(func_var* head,char* n1, int size){
    char* n;
    if(n1[0] == '#')
        return false;
    else if(n1[0] == '&'||n1[0] == '*')
        n = n1+1;
    else
        n = n1;
    func_var* current = head;
    if(current->size == -1){             //该图为空
        strcpy(current->name, n);
        current->next = NULL;
        current->size = size;
        return true;
    }
    while(current->next != NULL){
        if(strcmp(current->name, n) == 0){
            return false;   
        }
        current = current->next;
    }
    if(strcmp(current->name, n) == 0){
        return false;
    }
    func_var* new_node = (func_var*)malloc(sizeof(graph_node));
    strcpy(new_node->name, n);
    new_node->size = size;
    new_node->next = NULL;
    current->next = new_node;
    return true;
}

block_page* divide_block(func_info* icode){
    inter_code* current = icode->begin;
    int num = 0;
    block_page* new_block = (block_page*)malloc(sizeof(block_page));
    block_page* block_head = new_block;
    block_page* current_block = new_block;
    new_block->begin = icode->begin;
    new_block->num = 0;
    for(int i = 0;i < icode->code_num; i++){                                 //指令从0开始计数
        if(current->kind == FUNC_ic || current->kind == LABEL_ic){      //func开始一个新的基本块
            if(current_block->begin == current){
                current = current->next;
                continue;
            }
            block_page* new_block = (block_page*)malloc(sizeof(block_page));
            new_block->begin = current;
            new_block->num = i;
            current_block->end = current->pre;
            current_block->num = i - current_block->num;
            current_block->next = new_block;
            current_block = new_block;
        }
        else if(current->kind == GOTO_ic || current->kind == IF_ic || current->kind == RETURN_ic || current->kind == CALL_ic){ //这些作为基本块的结尾在上一个基本块中
            if(i == icode->code_num-1){                                  //这些是结尾的基本块
                current_block->num = i - current_block->num + 1;    
                current_block->end = current;
                current_block->next = NULL;
                current = current->next;
                continue;
            }
            block_page* new_block = (block_page*)malloc(sizeof(block_page));    //每次分割时将下一个基本块的开头准备好。
            new_block->begin = current->next;
            new_block->num = i+1;
            current_block->num = i+1 - current_block->num;
            current_block->end = current;
            current_block->next = new_block;
            current_block = new_block;
        }
        current = current->next;
    }
    return new_block;
}

void print_func(func_info* func){
    func_info* current = func;
    while(current != NULL){
        printf("func_name %s size %d\n", current->name, current->size);
        printf("code_num %d var_num %d", current->code_num, current->var_num);
        printf("var_list\n");
        func_var* current_var = current->var_list;
        while(current_var != NULL){
            printf("%s shift %d size %d\n",current_var->name, current_var->sp_shift,current_var->size);
            current_var = current_var->next;
        }
        current = current->next_func;
    }
}

#endif