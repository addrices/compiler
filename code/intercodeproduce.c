#include "m.h"

#ifdef __LAB3_ENABLE
int name_num = 0;
int label_num = 0;
inter_code_page* translate_exp(struct tree_node* root, operand* place);
inter_code_page* translate_stmt(struct tree_node* root);
inter_code_page* translaste_cond(struct tree_node* root, inter_code* label_true, inter_code* label_false);

inter_code* new_label(){            //新建一个label的code结点
    inter_code* new_label = (inter_code*)malloc(sizeof(inter_code));
    new_label->next = NULL;
    new_label->pre = NULL;
    new_label->kind = LABEL_ic;
    new_label->op1.src.kind = LABEL_op;
    sprintf(new_label->op1.src.name, "%d", label_num); label_num++;
}

/*翻译Exp，返回中间代码
    exp assignop exp
|   exp plus exp
|   exp minus exp | exp star exp
|   exp div exp  | LP exp RP
|   minus exp   |
|   id lp args rp | id lp rp //
|   exp lb exp rb | exp dot id
|   id! | int!
|   float!
*/
//在exp中可能会出现新添加的临时变量，保证这些临时变量的名字不同。

inter_code_page* translate_exp(struct tree_node* root, operand* place){
    int state = getexp_state(root);
    if(state == 15){            //INT
        if(place == NULL)
            assert(0);
        struct tree_node* INT_node = root->first_child;
        int value = INT_node->n_value.n_value_i;
        place->kind = CONSTANT_op;
        sprintf(place->name,"#%d",value);
        return NULL;
    }
    else if(state == 14){       //ID
        if(place == NULL)
            assert(0);
        struct tree_node* ID_node = root->first_child;
        var_node* var = var_node_search(ID_node->n_value.a);
        if(var->kind != &INT_type)
            assert(0);
        place->kind = VARIABLE_op;
        sprintf(place->name,"%s",var->name);
        return NULL;
    }
    else if(state == 7){        //assign    a = b
        if(place == NULL)
            assert(0);
        inter_code* code3 = (inter_code*)malloc(sizeof(inter_code)); code3->pre = NULL; code3->next = NULL;
        sprintf(code3->op2.left.name, "e%d",name_num);name_num++;
        sprintf(code3->op2.right.name, "e%d",name_num);name_num++;
        inter_code_page* code1 = translate_exp(root->first_child, &code3->op2.left);
        //如果这个变量可以用常量或者id直接表示会被替换成这个。
        inter_code_page* code2 = translate_exp(root->first_child->next_brother->next_brother, &code3->op2.right); //处理exp2 
        inter_code_page* codef = intercode_ppmerge(code1,code2);
        if(place != NULL){
            *place = code3->op2.left;
        }
        code3->kind = ASSIGN_ic;
        return intercode_p1merge(codef,code3);
    }
    else if((state >= 0 && state <= 3) || (state >= 5 && state <= 6)){      //+-*/   &|非法
        if(place == NULL)
            assert(0);
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
        if(place == NULL)
            assert(0);
        inter_code* code2 = (inter_code*)malloc(sizeof(inter_code)); code2->pre = NULL; code2->next = NULL;
        sprintf(code2->op3.right.name, "e%d",name_num);name_num++;
        inter_code_page* code1 = translate_exp(root->first_child, &code2->op3.right);
        code2->op3.left.kind == CONSTANT_op;
        sprintf(code2->op3.left.name,"#0");
        code2->op3.result = *place;
        code2->kind = SUB_ic; 
        return intercode_p1merge(code1,code2);
    }
    else if(state == 9 || (state >=4 && state <= 6)){ //exp中出现比较语句
        if(place == NULL)
            assert(0);
        inter_code* label1 = new_label();
        inter_code* label2 = new_label();
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
        
        code2 = intercode_1pmerge(code1, code2);
        code2 = intercode_p1merge(code2, label1);
        code2 = intercode_p1merge(code2, code3);
        return intercode_p1merge(code2, label2);        //将代码打包
    }
    else if(state == 10){
        struct tree_node* ID = root->first_child;
        func_node* func = func_node_search(ID->n_value.a);
        if(func->name == "read"){
            inter_code* read_code = (inter_code*)malloc(sizeof(inter_code)); read_code->pre = NULL; read_code->next = NULL;
            read_code->kind = READ_ic;
            read_code->op1.src = *place;
            return intercode_1merge(read_code);
        }
        else{
            inter_code* func_code = (inter_code*)malloc(sizeof(inter_code)); func_code->pre = NULL; func_code->next = NULL;
            func_code->kind = CALL_ic;
            func_code->op1.src.kind = FUNC_op;
            strcpy(func_code->op2.right.name, func->name);
            func_code->op2.left = *place;
            return intercode_1merge(func_code);
        }
    }
    else if(state == 11){
        struct tree_node* ID = root->first_child;
        func_node* func = func_node_search(ID->n_value.a);
    }
    assert(0);      //TODO:
}
/*翻译stmt语句
    Exp SEMI
|   CompSt
|   RETURN Exp SEMI
|   IF LP Exp RP Stmt
|   IF LP Exp RP Stmt ELSE Stmt
|   WHILE LP Exp RP Stmt
*/
inter_code_page* translate_stmt(struct tree_node* root){
    if(strcmp(root->first_child->n_type, "Exp") == 0){
        return translate_exp(root->first_child,NULL);
    }
    else if(strcmp(root->first_child->n_type, "Compst") == 0){
        return translaste_compst(root->first_child);
    }
    else if(strcmp(root->first_child->n_type, "RETURN") == 0){
        inter_code* code2 = (inter_code*)malloc(sizeof(inter_code)); code2->pre = NULL; code2->next = NULL;
        sprintf(code2->op1.src.name, "e%d",name_num);name_num++;
        inter_code_page* code1 = translate_exp(root->first_child->next_brother, &code2->op1.src);
        code2->kind = RETURN_ic;
        return intercode_p1merge(code1,code2);
    }
    else if(strcmp(root->first_child->n_type, "WHILE") == 0){
            //=============================
            //      LABEL1                =
            //      if(exp) goto LABEL2   =
            //      goto LABEL3           =
            //      LABEL2                =
            //      stmt                  =
            //      goto LABEL1           =
            //      LABEL3                =
            //=============================
        inter_code* label1 = new_label();
        inter_code* label2 = new_label();
        inter_code* label3 = new_label();
        inter_code_page* code1 = translate_cond(root->first_child->next_brother->next_brother, label2, label3);
        inter_code_page* code2 = translate_stmt(root->first_child->next_brother->next_brother->next_brother->next_brother);
        inter_code* goto_code = (inter_code*)malloc(sizeof(inter_code));goto_code->pre = NULL; goto_code->next = NULL;
        goto_code->kind = GOTO_ic;
        goto_code->op1.src.kind = LABEL_op;
        strcpy(goto_code->op1.src.name, label1->op1.src.name);

        inter_code_page* codef = intercode_1pmerge(label1, code1);
        codef = intercode_p1merge(codef,label2);
        codef = intercode_ppmerge(codef,code2);
        codef = intercode_p1merge(codef,goto_code);
        return intercode_p1merge(codef,label3);
    }
    else{           //if语句
        struct tree_node* exp = root->first_child->next_brother->next_brother;
        struct tree_node* stmt1 = root->first_child->next_brother->next_brother->next_brother->next_brother;
        if(stmt1->next_brother == NULL){        //no else
            //=============================
            //      if(exp) goto LABEL1   =
            //      goto LABEL2           =
            //      LABEL1                =
            //      stmt1                 =
            //      LABEL2                =
            //=============================
            inter_code* label1 = new_label();
            inter_code* label2 = new_label();

            inter_code_page* code1 = translate_cond(exp, label1, label2);
            inter_code_page* code2 = translate_stmt(stmt1);
            
            inter_code_page* codef = intercode_p1merge(code1, label1);
            codef = intercode_p1merge(code1,label1);
            codef = intercode_ppmerge(codef,code2);
            return intercode_p1merge(codef,label2);
        }
        else{                                   //having else
            //=============================
            //      if(exp) goto LABEL1   =
            //      goto LABEL2           =
            //      LABEL1                =
            //      stmt1                 =
            //      goto LABEL3           =
            //      LABEL2                =
            //      stmt2                 =
            //      LABEL3                =
            //=============================
            struct tree_node* stmt2 = stmt1->next_brother->next_brother;
            inter_code* label1 = new_label();
            inter_code* label2 = new_label();
            inter_code* label3 = new_label();

            inter_code_page* code1 = translate_cond(exp,label1,label2);
            inter_code_page* code2 = translate_stmt(stmt1);
            inter_code_page* code3 = translate_stmt(stmt2);
            inter_code* goto_code = (inter_code*)malloc(sizeof(inter_code)); goto_code->pre = NULL; goto_code->next = NULL;
            goto_code->kind = GOTO_ic;
            goto_code->op1.src.kind = LABEL_op;
            strcpy(goto_code->op1.src.name, label3->op1.src.name);
            inter_code_page* codef = intercode_p1merge(code1, label1);
            codef = intercode_ppmerge(codef,code2);
            codef = intercode_p1merge(codef,goto_code);
            codef = intercode_p1merge(codef,label2);
            codef = intercode_ppmerge(codef,code3);
            return intercode_p1merge(codef,label3);
        }
    }
}
/*
|   exp AND exp | exp OR exp
|   NOT exp |   exp RELOP exp
*/
inter_code_page* translaste_cond(struct tree_node* root, inter_code* label_true, inter_code* label_false){
    struct tree_node* op = root->first_child->next_brother;
    if(strcmp(root->first_child->n_type, "NOT") == 0){
        return translate_cond(root->first_child, label_false, label_true);
    }
    else if(strcmp(op->n_type, "OR") == 0){
        //=============================
        //  if(exp1) goto LABELtrue   =
        //  goto LABEL1               =
        //  LABEL1                    =
        //  if(exp2) goto LABELtrue   =
        //  goto LABELfalse           =
        //  stmt2                     =
        //  LABELtrue                 =
        //  ..........                =
        //  LABELfalse                =
        //  ..........                =
        //=============================
        inter_code* label1 = new_label();
        inter_code_page* code1 = translate_cond(root->first_child, label_true, label1);
        inter_code_page* code2 = translate_cond(root->first_child->next_brother->next_brother, label_true, label_false);
        inter_code_page* codef = intercode_p1merge(code1, label1);
        return intercode_ppmerge(codef,code2);
    }
    else if(strcmp(op->n_type, "AND") == 0){
        //=============================
        //  if(exp1) goto LABEL1      =
        //  goto LABELfalse           =
        //  LABEL1                    =
        //  if(exp2) goto LABELtrue   =
        //  goto LABELfalse           =
        //  stmt2                     =
        //  LABELtrue                 =
        //  ..........                =
        //  LABELfalse                =
        //  ..........                =
        //=============================
        inter_code* label1 = new_label();
        inter_code_page* code1 = translate_cond(root->first_child, label1, label_false);
        inter_code_page* code2 = translate_cond(root->first_child, label_true, label_false);
        inter_code_page* codef = intercode_p1merge(code1, label1);
        return intercode_ppmerge(codef,code2);
    }
    else if(strcmp(op->n_type, "RELOP") == 0){
        //=============================
        //  if(true) goto LABELtrue   =
        //  goto false                =
        //=============================
        inter_code* code3 = (inter_code*)malloc(sizeof(inter_code)); code3->pre = NULL; code3->next = NULL;
        code3->kind = IF_ic;
        code3->branch.left.kind = VARIABLE_op;
        code3->branch.right.kind = VARIABLE_op;
        sprintf(code3->branch.left.name, "e%d",name_num); name_num++;
        sprintf(code3->branch.right.name, "e%d",name_num); name_num++;
        inter_code_page* code1 = translate_exp(root->first_child, &code3->branch.left);
        inter_code_page* code2 = translate_exp(root->first_child->next_brother->next_brother, &code3->branch.right);
        switch(op->n_value.n_value_i){
            case 1: code3->branch.relop = LT;break;
            case 2: code3->branch.relop = GT;break;
            case 3: code3->branch.relop = EQ;break;
            case 4: code3->branch.relop = LE;break;
            case 5: code3->branch.relop = GE;break;
            case 6: code3->branch.relop = NE;break;
            default: break;
        }
        code3->branch.dest = label_true->op1.src;
        inter_code* goto_code = (inter_code*)malloc(sizeof(inter_code)); goto_code->pre = NULL; goto_code->next = NULL;
        goto_code->kind = GOTO_ic;
        goto_code->op1.src = label_false->op1.src;
        
        inter_code_page* codef = intercode_ppmerge(code1, code2);
        inter_code_page* codef = intercode_ppmerge(codef, code3);
        return intercode_p1merge(codef, goto_code);
    }
    else{
        assert(0);
    }
}

#endif