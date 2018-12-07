#include "m.h"

#ifdef __LAB3_ENABLE
int name_num = 0;
int label_num = 0;
inter_code_page* translate_exp(struct tree_node* root, operand* place);

/*翻译Exp，返回中间代码
    exp assignop exp
|   exp and exp | exp or exp
|   exp relop exp | exp plus exp
|   exp minus exp | exp star exp
|   exp div exp  | LP exp RP
|   minus exp   | NOT exp
|   id lp args rp | id lp rp //
|   exp lb exp rb | exp dot id
|   id! | int!
|   float!
*/
//在exp中可能会出现新添加的临时变量，保证这些临时变量的名字不同。
inter_code_page* translate_exp(struct tree_node* root, operand* place){
    int state = getexp_state(root);
    if(state == 15){
        struct tree_node* INT_node = root->first_child;
        int value = INT_node->n_value.n_value_i;
        place->kind = CONSTANT_op;
        sprintf(place->name,"#%d",value);
        return NULL;
    }
    else if(state == 14){  
        struct tree_node* ID_node = root->first_child;
        var_node* var = var_node_search(ID_node->n_value.a);
        if(var->kind != &INT_type)
            assert(0);
        place->kind = VARIABLE_op;
        sprintf(place->name,"%s",var->name);
        return NULL;
    }
    else if(state == 7){        //assign
        inter_code* code3 = (inter_code*)malloc(sizeof(inter_code)); code3->pre = NULL; code3->next = NULL;
        sprintf(code3->op2.left.name, "e%d",name_num);name_num++;
        sprintf(code3->op2.right.name, "e%d",name_num);name_num++;
        inter_code_page* code1 = translate_exp(root->first_child, &code3->op2.left);
        //如果这个变量可以用常量或者id直接表示会被替换成这个。
        inter_code_page* code2 = translate_exp(root->first_child->next_brother->next_brother, &code3->op2.right); //处理exp2 
        inter_code_page* codef = intercode_ppmerge(code1,code2);
        *place = code3->op2.left;
        code3->kind = ASSIGN_ic;
        return intercode_p1merge(codef,code3);
    }
    else if((state >= 0 && state <= 3) || (state >= 5 && state <= 6)){
        inter_code* code3 = (inter_code*)malloc(sizeof(inter_code)); code3->pre = NULL; code3->next = NULL;
        sprintf(code3->op3.left.name, "e%d",name_num);name_num++;
        sprintf(code3->op3.right.name, "e%d",name_num);name_num++;
        inter_code_page* code1 = translate_exp(root->first_child, &code3->op2.left);
        inter_code_page* code2 = translate_exp(root->first_child->next_brother->next_brother, &code3->op2.right); //处理exp2 
        inter_code_page* codef = intercode_ppmerge(code1,code2);
        switch(state){
            case 0: code3->kind = ADD_ic; break;
            case 1: code3->kind = SUB_ic; break;
            case 2: code3->kind = MUL_ic; break;
            case 3: code3->kind = DIV_ic; break;
            default: assert(0);
        }
        code3->op3.result = *place;
        return intercode_p1merge(codef,code3);
    }
    else if(state == 8){
        if(state == 9){
            assert(0);
        }
        inter_code* code2 = (inter_code*)malloc(sizeof(inter_code)); code2->pre = NULL; code2->next = NULL;
        sprintf(code2->op3.right.name, "e%d",name_num);name_num++;
        inter_code_page* code1 = translate_exp(root->first_child, &code2->op3.right);
        code2->op3.left.kind == CONSTANT_op;
        sprintf(code2->op3.left.name,"#0");
        code2->op3.result = *place;
        code2->kind = SUB_ic; 
        return intercode_p1merge(code1,code2);
    }
    else if(state == 9 || (state >=4 && state <= 6)){
        inter_code* label1 = (inter_code*)malloc(sizeof(inter_code)); label1->pre = NULL; label1->next = NULL;
        inter_code* label2 = (inter_code*)malloc(sizeof(inter_code)); label2->pre = NULL; label2->next = NULL;
        label1->kind = LABEL_ic;
        label2->kind = LABEL_ic;
        label1->op1.src.kind = LABEL_op;
        label2->op1.src.kind = LABEL_op;
        sprintf(label1->op1.src.name, "%d", label_num); label_num++;
        sprintf(label2->op1.src.name, "%d", label_num); label_num++;
        place->kind = CONSTANT_op;
        inter_code* code1 = (inter_code*)malloc(sizeof(inter_code)); code1->pre = NULL; code1->next = NULL;
        code1->kind = ASSIGN_ic;
        code1->op2.left = *place;
        code1->op2.right.kind = CONSTANT_op;
        sprintf(code1->op2.right.name, "#0");   //place := #0
        
        inter_code_page* code2 = translate_cond(root, label1, label2);
        
        inter_code* code3 = (inter_code*)malloc(sizeof(inter_code)); code3->pre = NULL; code3->next = NULL;
        code3->kind = ASSIGN_ic;
        code3->op2.left = *place;
        code3->op2.right.kind = CONSTANT_op;
        sprintf(code3->op2.right.name, "#1");   //place := #1

        intercode_11merge
    }
}


#endif