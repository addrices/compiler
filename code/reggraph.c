#include "m.h"

#ifdef __LAB4_ENABLE
bool graph_add(graph_node* graph_head,char* n1);
int get_graph_num(graph_node* graph_head, char* n1,int node_num);
graph_node* node_list2array(graph_node* graph_head,int num);
int count_graph_node(graph_node* graph_head,block_page* block);
int** get_neigh_array(graph_node* graph_head, block_page* block, int node_num);
void print_graph_neigh_array(int** array, int node_num);
void print_graph_node(graph_node* graph_node_array,int node_num);
void sort_graph_array(graph_node* graph_node_array, int node_num);
void dye_color(graph_node* graph_node_array,int** graph_neigh_array,int node_num);

graph_node* block_graph_color(block_page* block){
    graph_node* graph_head = (graph_node*)malloc(sizeof(graph_node));
    int node_num = count_graph_node(graph_head, block);                     //统计块中变量数，用链表储存。
    graph_node* graph_node_array = node_list2array(graph_head,node_num);      //将graph_node转移到数组中储存。
    int** graph_neigh_array = get_neigh_array(graph_node_array,block,node_num);      //将邻接关系变成一个二维数组。
    //print_graph_node(graph_node_array,node_num);
    sort_graph_array(graph_node_array,node_num);
    dye_color(graph_node_array,graph_neigh_array,node_num);
    //printf("after sort\n");
    print_graph_node(graph_node_array,node_num);
    printf("\n");
    //print_graph_neigh_array(graph_neigh_array,node_num);
    return graph_node_array;
}

int** get_neigh_array(graph_node* graph_head, block_page* block,int node_num){
    int** graph_array = (int**)malloc(node_num * sizeof(int*));     //邻接关系的二维数组
    for(int i = 0; i < node_num; i++){
        graph_array[i] = (int*)malloc(node_num * sizeof(int));
    }
    for(int i = 0; i < node_num; i++){
        for(int j = 0; j < node_num; j++){
            graph_array[i][j] = 0;
        }
    }
    inter_code* current = block->begin;
    for(int i = 0; i < block->num; i++){
        if(current->kind == LABEL_ic ||current->kind == FUNC_ic ||current->kind == GOTO_ic|| current->kind == DEC_ic)
            ;
        else if(current->kind == ASSIGN_ic || current->kind == GETADDR_ic || current->kind == GETVALUE_ic || current->kind == ADDRASS_ic || current->kind == CALL_ic ){
            int n1 = get_graph_num(graph_head,current->op2.left.name, node_num);
            int n2 = get_graph_num(graph_head,current->op2.right.name,  node_num);
            if(n1 != -1 && n2 != -1){
                graph_array[n1][n2] = 1;
                graph_array[n2][n1] = 1;
                graph_head[n1].neigh_num++;
                graph_head[n2].neigh_num++;
            }
        }
        else if(current->kind == READ_ic || current->kind == WRITE_ic || current->kind == PARAM_ic || current->kind == ARG_ic || current->kind == RETURN_ic)
            ;
        else if(current->kind == ADD_ic || current->kind == SUB_ic ||current->kind == MUL_ic ||current->kind == DIV_ic){
            int n1 = get_graph_num(graph_head,current->op3.left.name,node_num);
            int n2 = get_graph_num(graph_head,current->op3.right.name,node_num);
            int n3 = get_graph_num(graph_head,current->op3.result.name,node_num);
            if(n1 != -1 && n2 != -1 && n3 != -1){
                graph_array[n1][n2] = 1;
                graph_array[n2][n1] = 1;
                graph_array[n1][n3] = 1;
                graph_array[n3][n1] = 1;
                graph_array[n2][n3] = 1;
                graph_array[n3][n2] = 1;
                graph_head[n1].neigh_num += 2;
                graph_head[n2].neigh_num += 2;
                graph_head[n3].neigh_num += 2;
            }
            else if(n1 != -1 && n2 != -1){
                graph_array[n1][n2] = 1;
                graph_array[n2][n1] = 1;
                graph_head[n1].neigh_num++;
                graph_head[n2].neigh_num++;
            }
            else if(n1 != -1 && n3 != -1){
                graph_array[n1][n3] = 1;
                graph_array[n3][n1] = 1;  
                graph_head[n1].neigh_num++;
                graph_head[n3].neigh_num++;              
            }
            else if(n2 != -1&& n3 != -1){
                graph_array[n2][n3] = 1;
                graph_array[n3][n2] = 1;  
                graph_head[n2].neigh_num++;
                graph_head[n3].neigh_num++;              
            }
        }
        else if(current->kind == IF_ic){
            int n1 = get_graph_num(graph_head,current->branch.left.name, node_num);
            int n2 = get_graph_num(graph_head,current->branch.right.name,  node_num);
            if(n1 != -1 && n2 != -1){
                graph_array[n1][n2] = 1;
                graph_array[n2][n1] = 1;
                graph_head[n1].neigh_num++;
                graph_head[n2].neigh_num++;    
            }
        }
        else{
            printf("block_graph_init error\n");
            assert(0); 
        }
        current = current->next;
    }
    return graph_array;
}

graph_node* node_list2array(graph_node* graph_head,int num){
    graph_node* graph_node_array = (graph_node*)malloc(num * sizeof(graph_node));
    graph_node* current = graph_head;
    for(int i = 0; i < num;i++){
        graph_node_array[i] = *current;
        graph_node* free_node = current;
        current = current->next;
        free(free_node);
    }
    return graph_node_array;
}

int count_graph_node(graph_node* graph_head,block_page* block){
    graph_head->graph_num = -1;
    inter_code* current = block->begin;
    int node_num = 0;                   //这是表示这个基本块中有多少变量。
    for(int i = 0; i < block->num; i++){
        if(current->kind == LABEL_ic || current->kind == FUNC_ic || current->kind == GOTO_ic || current->kind == DEC_ic)
            ;
        else if(current->kind == ASSIGN_ic || current->kind == GETADDR_ic || current->kind == GETVALUE_ic || current->kind == ADDRASS_ic || current->kind == CALL_ic ){
            if(graph_add(graph_head,current->op2.left.name))
                node_num++;
            if(graph_add(graph_head,current->op2.right.name))
                node_num++;
        }
        else if(current->kind == READ_ic || current->kind == WRITE_ic || current->kind == PARAM_ic || current->kind == ARG_ic || current->kind == RETURN_ic){
            if(graph_add(graph_head,current->op1.src.name))
                node_num++;
        }
        else if(current->kind == ADD_ic || current->kind == SUB_ic ||current->kind == MUL_ic ||current->kind == DIV_ic){
            if(graph_add(graph_head, current->op3.result.name))
                node_num++;
            if(graph_add(graph_head, current->op3.left.name))
                node_num++;
            if(graph_add(graph_head, current->op3.right.name))
                node_num++;
        }
        else if(current->kind == IF_ic){
            if(graph_add(graph_head, current->branch.left.name))
                node_num++;
            if(graph_add(graph_head, current->branch.right.name))
                node_num++;
        }
        else{
            printf("block_graph_init error\n");
            assert(0); 
        }
        current = current->next;
    }           //第一遍扫描，获得graph_node的数量以便生成二维数组来描述图；
    return node_num;
}

int get_graph_num(graph_node* graph_head, char* n1,int node_num){
    char* n;
    if(n1[0] == '*' || n1[0] == '&')
        n = n1 + 1;
    else if(n1[0] == '#')
        return -1;
    else    
        n = n1;
    for(int i = 0; i < node_num; i++){
        if(strcmp(graph_head[i].name, n) == 0){
            return i;
        }
    }
    assert(0);
}

bool graph_add(graph_node* graph_head,char* n1){
    char* n;
    if(n1[0] == '#')
        return false;
    else if(n1[0] == '&'||n1[0] == '*')
        n = n1+1;
    else
        n = n1;
    graph_node* current = graph_head;
    if(current->graph_num == -1){             //该图为空
        strcpy(current->name, n);
        current->next = NULL;
        current->graph_num = 0;
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
    graph_node* new_node = (graph_node*)malloc(sizeof(graph_node));
    strcpy(new_node->name, n);
    new_node->graph_num = current->graph_num + 1;
    new_node->next = NULL;
    new_node->neigh_num = 0;
    new_node->reg_num = 0;
    current->next = new_node;
    return true;
}

void print_graph_node(graph_node* graph_node_array,int node_num){
    for(int i = 0; i < node_num; i++){
        printf("%s %d %d %d\n",graph_node_array[i].name, graph_node_array[i].graph_num, graph_node_array[i].neigh_num, graph_node_array[i].reg_num);
    }
}

void print_graph_neigh_array(int** array, int node_num){
    printf("neigh_array:\n");
    for(int i = 0; i < node_num; i++){
        for(int j = 0; j < node_num; j++){
            printf("%d ",array[i][j]);
        }
        printf("\n");
    }
}

void sort_graph_array(graph_node* graph_node_array, int node_num){
    for(int i = 0; i < node_num - 1; i++){
        for(int j = 0; j < node_num - i - 1; j++){
            if(graph_node_array[j].neigh_num < graph_node_array[j+1].neigh_num){
                graph_node temp = graph_node_array[j];
                graph_node_array[j] = graph_node_array[j+1];
                graph_node_array[j+1] = temp;
            }
        }
    }
}

void dye_color(graph_node* graph_node_array,int** graph_neigh_array,int node_num){
    for(int i = 0; i < node_num; i++){
        bool regs[17];              //$8-$24分给你们当寄存器用
        int graph_num = graph_node_array[i].graph_num;
        for(int j = 0; j < 17;j++)
            regs[j] = false;
        for(int j = 0; j < node_num; j++){
            if(graph_neigh_array[graph_num][j] == 1){       //j号是我的邻居
                for(int k = 0; k < node_num; k++){
                    if(graph_node_array[k].graph_num == j && graph_node_array[k].reg_num != 0)      //搜索j号node，如果其reg_num ！=0，说明已经初始化过了。
                       regs[graph_node_array[k].reg_num - 8] = true; 
                }
            }
        }
        for(int j = 0; j < 17; j++){
            if(regs[j] == false){
                graph_node_array[i].reg_num = j + 8;
                break;
            }
        }
    }
    for(int i = 0; i < node_num; i++)
        if(graph_node_array[i].reg_num == 0){
            printf("图染色失败\n");
            assert(0);
        }
}


void divide_block(inter_code_page* icode){
    inter_code* current = icode->begin;
    int num = 0;
    block_page* new_block = (block_page*)malloc(sizeof(block_page));
    block_head = new_block;
    block_page* current_block = new_block;
    new_block->begin = icode->begin;
    new_block->num = 0;
    for(int i = 0;i < icode->num; i++){
        if(current->kind == FUNC_ic || current->kind == LABEL_ic){
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
        else if(current->kind == GOTO_ic || current->kind == IF_ic || current->kind == RETURN_ic || current->kind == CALL_ic){
            if(i == icode->num-1){
                current_block->num = i - current_block->num + 1;
                current_block->end = current;
                current_block->next = NULL;
                current = current->next;
                continue;
            }
            block_page* new_block = (block_page*)malloc(sizeof(block_page));
            new_block->begin = current->next;
            new_block->num = i+1;
            current_block->num = i+1 - current_block->num;
            current_block->end = current;
            current_block->next = new_block;
            current_block = new_block;
        }
        current = current->next;
    }
    /*current_block = block_head;
    while(current_block!= NULL){
        block_graph_color(current_block);
        current_block = current_block->next;
    }*/
}
#endif