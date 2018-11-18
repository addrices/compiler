#include "m.h"
#ifdef __LAB2_ENABLE

type INT_type = {BASIC, 1};
type FLOAT_type = {BASIC, 2};
type* RE_type = NULL;

void analy_ExtDefList(struct tree_node* root);
void analy_ExtDef(struct tree_node* root);
func_node* analy_FunDec(struct tree_node* root,type* kind);
field_list* analy_VarList(struct tree_node* root);
field_list* analy_ParamDec(struct tree_node* root);
type* analy_Specifier(struct tree_node* root);
type* analy_StructSpecifier(struct tree_node* root);
var_node* analy_VarDec(struct tree_node* root,type* kind);
field_list* analy_DecList(struct tree_node* root,type* kind,bool flag);
field_list* analy_Dec(struct tree_node* root,type* kind,bool flag);
field_list* analy_DefList(struct tree_node* root,bool flag);
field_list* analy_Def(struct tree_node* root,bool flag);
void analy_ExtDecList(struct tree_node* root,type* kind);
bool type_equal(type* a_type, type* b_type);
bool analy_Args(struct tree_node* root, field_list* flist);
type* analy_Exp(struct tree_node* root);
void analy_Stmt(struct tree_node* root);
void analy_StmtList(struct tree_node* root);
void analy_Compst(struct tree_node* root);

//分析整个程序
void analy_Program(struct tree_node* root_node){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(root_node == NULL)
        return;
    else
        analy_ExtDefList(root_node->first_child);
    return;
}

//分析所有的大定义
void analy_ExtDefList(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(root->first_child == NULL)
        return;
    else{
        analy_ExtDef(root->first_child);
        analy_ExtDefList(root->first_child->next_brother);
    }
}

/*分析一个大定义
    Specifier ExtDecList SEMI
|   Specifier SEMI
|   Specifier FunDec CompSt
*/
void analy_ExtDef(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->next_brother->n_type, "FunDec") == 0){
        RE_type = NULL;
        type* kind = analy_Specifier(root->first_child);
        func_node* f = analy_FunDec(root->first_child->next_brother, kind);
        analy_Compst(root->first_child->next_brother->next_brother);
        if(f != NULL){
            if(type_equal(f->return_type, &RE_type)){
                error2_node* err = (error2_node*)malloc(sizeof(error2_node));
                err->length = root->n_length;
                err->num = 8;
                err->next = NULL;
                sprintf(err->info,"Type mismatched for return.");
                error2_node_add(err);
                return NULL;
            }
        }
    }
    else if(strcmp(root->first_child->next_brother->n_type, "SEMI") == 0){
        analy_Specifier(root->first_child);
    }
    else{
        type* kind = analy_Specifier(root->first_child);
        analy_ExtDecList(root->first_child->next_brother,kind);
    }
    return;
}

void analy_ExtDecList(struct tree_node* root,type* kind){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    analy_VarDec(root->first_child,kind);
    if(root->first_child->next_brother != NULL)
        analy_ExtDecList(root->first_child->next_brother->next_brother,kind);
}

/*函数的定义处理
    ID LP VarList RP
|   ID LP RP
*/
func_node* analy_FunDec(struct tree_node* root,type* kind){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    func_node* current = (func_node*)malloc(sizeof(func_node));
    current->return_type = kind;
    if(strlen(root->first_child->n_value.a) < __MAX_NAME_LENGTH)
        strcpy(current->name,root->first_child->n_value.a);
    else{
        printf("func name too long\n");
        assert(0);
    }
    if(strcmp(root->first_child->next_brother->next_brother->n_type,"VarList") == 0){
        current->list = analy_VarList(root->first_child->next_brother->next_brother);
    }
    else{
        current->list = NULL;
    }
    if(func_node_add(current) == false){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 4;
            err->next = NULL;
            sprintf(err->info,"Redefined function \"%s\"", current->name);
            error2_node_add(err);
            return NULL;
    }
    return current;
}

/*用于根据varList给func_node添加节点 ^.^ (整合field_list表给func_node)
    ParamDec COMMA VarList
|   ParamDec
*/
field_list* analy_VarList(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(root->first_child->next_brother == NULL){ //only ParamDec
        return analy_ParamDec(root->first_child);
    }
    else{
        field_list* current = analy_ParamDec(root->first_child);
        current->next = analy_VarList(root->first_child->next_brother->next_brother);
        return current;
    }
}

// 将类型交给 VarDec 获得VarDec产生的变量指针整合成field_list结点后return
// Specifier VarDec
field_list* analy_ParamDec(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    type* type_node = analy_Specifier(root->first_child);
    field_list* flist = (field_list*)malloc(sizeof(field_list));
    flist->var = analy_VarDec(root->first_child->next_brother, type_node);
    #ifdef __DEBUG
    print_varlist();
    #endif
    flist->next = NULL;
    return flist;
}

//分析产生的Specififer并将对应的type指针返回 
type* analy_Specifier(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->n_type, "TYPE") == 0){
        if(strcmp(root->first_child->n_value.a, "int") == 0)
            return &INT_type;
        else
            return &FLOAT_type;
    }
    else{
        return analy_StructSpecifier(root->first_child);
    }
}

/*分析声明的结构体的结构。加入结构体表。或者是寻找对应的结构体。
    STRUCT OptTag LC DefList RC
|   STRUCT Tag
*/
type* analy_StructSpecifier(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->next_brother->n_type, "OptTag") == 0){
        type* retype = (type*)malloc(sizeof(type));
        retype->kind = STRUCTURE;
        struct_node* new_struct = (struct_node*)malloc(sizeof(struct_node));
        struct tree_node* opttag = root->first_child->next_brother;
        if(opttag->first_child == NULL)
            new_struct->name[0] = '\0';
        else{
            if(strlen(opttag->first_child->n_value.a) < __MAX_NAME_LENGTH)
                strcpy(new_struct->name, opttag->first_child->n_value.a);
            else{
                printf("struct too long\n");
                assert(0);
            }
        }
        new_struct->next = NULL;
        new_struct->list = analy_DefList(root->first_child->next_brother->next_brother->next_brother,true);
        if(struct_node_add(new_struct) == false){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 16;
            err->next = NULL;
            sprintf(err->info,"Duplicated name \"%s\".",new_struct->name);
            error2_node_add(err);
            return NULL;
        }
        retype->u.structure = new_struct;
        return retype;
    }
    else{
        type* retype = (type*)malloc(sizeof(type));
        retype->kind = STRUCTURE;
        struct tree_node* tag = root->first_child->next_brother;
        struct_node* current = struct_node_search(tag->first_child->n_value.a) ;
        if(current == NULL){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 17;
            err->next = NULL;
            sprintf(err->info,"Undefined structure \"%s\".",tag->first_child->n_value.a);
            error2_node_add(err);
            return NULL;
        }
        retype->u.structure = current;
        return retype;
        //找到对应的结构体。
    }
}


/*当flag是true是说明是struct调用他，所以会返回一个field_list的指针，如果为false则是compst调用的返回NULL；
    Def DefList
|   Empty
*/
field_list* analy_DefList(struct tree_node* root,bool flag){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(root->first_child == NULL)
        return NULL;
    else if(flag == true){
        field_list* new_flist;
        new_flist = analy_Def(root->first_child,flag);
        new_flist->next = analy_DefList(root->first_child->next_brother,flag);
        return new_flist;
    }
    else{
        analy_Def(root->first_child,flag);
        analy_DefList(root->first_child->next_brother,flag);
        return NULL;
    }
}

/*返回一个field_list，其中保存着这个Def中声明的所有的变量。当flag为true时（struct调用）
不然不用管field_list;
    Specifier DecList SEMI
*/
field_list* analy_Def(struct tree_node* root,bool flag){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    type* kind = analy_Specifier(root->first_child);
    if(flag == true){
        field_list* new_flist = analy_DecList(root->first_child->next_brother,kind,flag);
        return new_flist;
    }
    else{
        analy_DecList(root->first_child->next_brother,kind,flag);
        return NULL;
    }
}

/*参考ParamDef，返回field_list的一个指针。
    Dec
|   Dec COMMA DecList
*/
field_list* analy_DecList(struct tree_node* root,type* kind,bool flag){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(flag){
        field_list* new_fnode;
        if(root->first_child->next_brother != NULL){
            new_fnode = analy_Dec(root->first_child,kind,flag);
            new_fnode->next = analy_DecList(root->first_child->next_brother->next_brother,kind,flag);
        }
        else{
            new_fnode = analy_Dec(root->first_child,kind,flag);
        }
        return new_fnode;
    }
    else{
        if(root->first_child->next_brother != NULL){
            analy_Dec(root->first_child,kind,flag);
            analy_DecList(root->first_child->next_brother->next_brother,kind,flag);
        }
        else{
            analy_Dec(root->first_child,kind,flag);
        }
        return NULL;
    }
}

/*analyVarDec,那个field_list的处理要在这里做，应为varDec要处理数组。
    VarDec
|   VarDec ASSIGNOP EXP
*/
field_list* analy_Dec(struct tree_node* root,type* kind,bool flag){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(flag){
        field_list* new_fnode = (field_list*)malloc(sizeof(field_list));
        new_fnode->next = NULL;
        new_fnode->var = analy_VarDec(root->first_child, kind);
        if(root->first_child->next_brother == NULL)
            return new_fnode;
        else{
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 15;
            err->next = NULL;
            sprintf(err->info,"struct field can't initial");
            error2_node_add(err);
            return NULL;
        }
    }
    else{
        analy_VarDec(root->first_child,kind);
        return NULL;
    }
}


/*这个要返回变量，因为要处理数组。将声明的变量加入变量表。
    ID
|   VarDec LB INT RB
*/
var_node* analy_VarDec(struct tree_node* root,type* kind){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    var_node *new_node = (var_node*)malloc(sizeof(var_node));
    if(root->first_child->next_brother == NULL){
        if(strlen(root->first_child->n_value.a) < __MAX_NAME_LENGTH)
            strcpy(new_node->name,root->first_child->n_value.a);
        else{
            printf("var name too long\n");
            assert(0);
        }    
        new_node->kind = kind;
        new_node->next = NULL;
        if(var_node_add(new_node) == false){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 3;
            err->next = NULL;
            sprintf(err->info,"Redefined variable \"%s\"",new_node->name);
            error2_node_add(err);
            return NULL;
        }
        return new_node;
    }
    else{
        type* new_type_node = (type*)malloc(sizeof(type));
        new_type_node->kind = ARRAY;
        new_type_node->u.array.elem = kind;
        new_type_node->u.array.size = root->first_child->next_brother->next_brother->n_value.n_value_i;
        return analy_VarDec(root->first_child,new_type_node);
    }
}

/*分析函数体内部的内容了hhh &.&
    LC DefList StmtList RC
*/
void analy_Compst(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    analy_DefList(root->first_child->next_brother,false);
    analy_StmtList(root->first_child->next_brother->next_brother);
}

/*分析语句部分
    Stmt StmtList
|   empty
*/
void analy_StmtList(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(root->first_child == NULL)
        return;
    else{
        analy_Stmt(root->first_child);
        analy_StmtList(root->first_child->next_brother);
    }
}

/*分析一个语句。。。。
    Exp SEMI
|   CompSt
|   RETURN Exp SEMI
|   IF LP Exp RP Stmt
|   IF LP Exp RP Stmt ELSE Stmt
|   WHILE LP Exp RP Stmt
*/
void analy_Stmt(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->n_type, "Exp") == 0){
        analy_Exp(root->first_child);
    }  
    else if(strcmp(root->first_child->n_type, "CompSt") == 0){
        analy_Compst(root->first_child);
    }
    else if(strcmp(root->first_child->n_type, "RETURN") == 0){
        type* re = analy_Exp(root->first_child->next_brother);
        RE_type = re;
    }
    else if(strcmp(root->first_child->n_type, "IF") == 0){
        struct tree_node* Exp = root->first_child->next_brother->next_brother;
        if(analy_Exp(Exp) != &INT_type){
            printf("IF type error\n");
            assert(0);
        }
        struct tree_node* TStmt = Exp->next_brother->next_brother;
        analy_Stmt(TStmt);
        if(TStmt->next_brother != NULL){
            struct tree_node* FStmt = TStmt->next_brother->next_brother;
            analy_Stmt(FStmt);
        }
    }
    else{
        struct tree_node* Exp = root->first_child->next_brother->next_brother;
        if(analy_Exp(Exp) != &INT_type){
            printf("while type error\n");
            assert(0);
        }
        struct tree_node* TStmt = Exp->next_brother->next_brother;
        analy_Stmt(TStmt);
    }
}

/*分析Exp，判断其中是否合法，如果不合法，报错，如果合法就返回这个exp的类型。
    exp assignop exp
|   exp and exp | exp or exp
|   exp relop exp | exp plus exp
|   exp minus exp | exp star exp
|   exp div exp  | LP exp RP
|   minus exp   | NOT exp
|   ld lp args rp | id lp rp
|   exp lb exp rb | exp dot id
|   id | int
|   float
*/
type* analy_Exp(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    struct tree_node* op = root->first_child->next_brother;
    int state = 0;
    if(op != NULL){
        if(strcmp(op->n_type,"ADD") == 0) state = 0;
        else if(strcmp(op->n_type, "MINUS") == 0) state = 1;
        else if(strcmp(op->n_type, "STAR") == 0) state = 2;
        else if(strcmp(op->n_type, "DIV") == 0) state = 3;
        else if(strcmp(op->n_type, "RELOP") == 0) state = 4; 
        else if(strcmp(op->n_type, "AND") == 0) state = 5;
        else if(strcmp(op->n_type, "OR") == 0) state = 6;
        else if(strcmp(op->n_type, "ASSIGNOP") == 0) state = 7;
        else if(strcmp(root->first_child->next_brother->n_type, "LP") == 0){
            if(strcmp(root->first_child->next_brother->next_brother->n_type, "RP") == 0)
                state = 10;
            else
                state = 11;
        }
        else if(strcmp(root->first_child->next_brother->n_type, "LB") == 0) state = 12;
        else if(strcmp(root->first_child->next_brother->n_type, "DOT") == 0) state = 13;
    }
    else if(strcmp(root->first_child->n_type, "MINUS") == 0) state = 8;
    else if(strcmp(root->first_child->n_type, "NOT") == 0) state = 9;
    else if(strcmp(root->first_child->n_type, "ID") == 0) state = 14;
    else if(strcmp(root->first_child->n_type, "INT") == 0) state = 15;
    else if(strcmp(root->first_child->n_type, "FLOAT") == 0) state = 16;
    if(state >= 0 && state <= 7){
        struct tree_node* a = root->first_child;
        struct tree_node* b = a->next_brother->next_brother;
        type* a_type = analy_Exp(a);
        type* b_type = analy_Exp(b);
        if(type_equal(a_type,b_type) == true){
            if(state >= 0 && state <= 3){
                if(a_type->kind != BASIC){
                    printf("Exp fourop type error\n");
                    assert(0);
                }
                else{
                    if(a_type == NULL)
                        return b_type;
                    else
                        return a_type;
                }
            } 
            else if(state == 4){
                if(a_type->kind != BASIC && a_type != NULL){
                    printf("Exp relop type error\n");
                    assert(0);
                }
                else
                    return &INT_type;
            }
            else if(state == 5 && state == 6){
                if((a_type->kind == BASIC && a_type->u.basic == 1) || a_type == NULL)
                    return &INT_type;
                else{
                    printf("Exp ORAND type error\n");
                    assert(0);
                }
            }
            else{
                if(strcmp(root->first_child->first_child->n_type,"INT") == 0 || strcmp(root->first_child->first_child->n_type,"FLOAT") == 0){
                    error2_node* err = (error2_node*)malloc(sizeof(error2_node));
                    err->length = root->n_length;
                    err->next = NULL;
                    err->num = 6;
                    sprintf(err->info,"The left-hand side of an assignment must be a variable");
                    error2_node_add(err);
                    return NULL;
                }
                if(a_type == NULL)
                    return b_type;
                else
                    return a_type;
            }
        }
        error2_node* err = (error2_node*)malloc(sizeof(error2_node));
        err->length = root->n_length;
        err->next = NULL;
        if(state == 7){
            err->num = 5;
            sprintf(err->info,"Type mismatched for assignment");
            error2_node_add(err);
        }
        else{
            err->num = 7;
            sprintf(err->info,"Type mismatched for operands");
            error2_node_add(err);
        }
        return NULL;
    }
    else if(state == 8){
        type* a_type = analy_Exp(root->first_child->next_brother);
        if(a_type->kind != BASIC && a_type->kind != NULL){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 7;
            err->next = NULL;
            sprintf(err->info,"Type mismatched for not");
            error2_node_add(err);
            return NULL;
        }
        else
            return a_type;
    }
    else if(state == 9){
        type* a_type = analy_Exp(root->first_child->next_brother);
        if((a_type->kind == BASIC && a_type->u.basic == 1) || a_type == NULL){
            return a_type;
        }
        else{
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 7;
            err->next = NULL;
            sprintf(err->info,"Type mismatched for not");
            error2_node_add(err);
            return NULL;
        }
    }
    else if(state == 10){
        func_node *func = func_node_search(root->first_child->n_value.a);
        if(func == NULL){           //没找到对应的func
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 2;
            err->next = NULL;
            sprintf(err->info,"Undefined function \"%s\"",root->first_child->n_value.a);
            error2_node_add(err);
            return NULL;
        }
        if(func->list != NULL){     //这个func应该是没有参数的。
            printf("parameter error\n");
            assert(0);
            return func->return_type;
        }
        return func->return_type;
    }
    else if(state == 11){
        func_node *func = func_node_search(root->first_child->n_value.a);
        var_node* varf = var_node_search(root->first_child->n_value.a);
        if(func == NULL){
            if(varf == NULL){
                error2_node* err = (error2_node*)malloc(sizeof(error2_node));
                err->length = root->n_length;
                err->num = 2;
                err->next = NULL;
                sprintf(err->info,"Undefined function \"%s\"",root->first_child->n_value.a);
                error2_node_add(err);
                return NULL;
            }
            else{
                error2_node* err = (error2_node*)malloc(sizeof(error2_node));
                err->length = root->n_length;
                err->num = 11;
                err->next = NULL;
                sprintf(err->info,"\"%s\" is not a function",root->first_child->n_value.a);
                error2_node_add(err);
                return NULL;
            }
        }
        if(analy_Args(root->first_child->next_brother->next_brother,func->list))
            return func->return_type;
        else{
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 9;
            err->next = NULL;
            sprintf(err->info,"Function \"%s\" is not applicable for arguments ",func->name);
            error2_node_add(err);
            return NULL;
        }
    }
    else if(state == 12){
        type* a_type = analy_Exp(root->first_child);
        type* b_type = analy_Exp(root->first_child->next_brother->next_brother);
        if(a_type->kind != ARRAY && a_type != NULL){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 10;
            err->next = NULL;
            sprintf(err->info,"it is not an array");
            error2_node_add(err);
            return NULL;
        }
        if((b_type->kind != BASIC || b_type->u.basic != 1) && b_type->kind != NULL){//[]中的数必须是int类型的。
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 12;
            err->next = NULL;
            sprintf(err->info,"it is not an integer.");
            error2_node_add(err);
            return NULL;
        }
        return a_type->u.array.elem;
    }
    else if(state == 13){
        type* a_type = analy_Exp(root->first_child);
        if(a_type->kind != STRUCTURE){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 13;
            err->next = NULL;
            sprintf(err->info,"Illegal use of \".\".");
            error2_node_add(err);
            return NULL;
        }
        field_list* fp = a_type->u.structure->list;
        while(fp != NULL){
            if(strcmp(fp->var->name,root->first_child->next_brother->next_brother->n_value.a) == 0)
                return fp->var->kind;
            fp = fp->next;
        }
        error2_node* err = (error2_node*)malloc(sizeof(error2_node));
        err->length = root->n_length;
        err->num = 14;
        err->next = NULL;
        sprintf(err->info,"Non-existent field");
        error2_node_add(err);
        return NULL;
    }
    else if(state == 14){
        var_node *var = var_node_search(root->first_child->n_value.a);
        if(var == NULL){
            error2_node* err = (error2_node*)malloc(sizeof(error2_node));
            err->length = root->n_length;
            err->num = 1;
            err->next = NULL;
            sprintf(err->info,"Undefined variable \"%s\"",root->first_child->n_value.a);
            error2_node_add(err);
            return NULL;
        }   
        return var->kind;
    }
    else if(state == 15){
        return &INT_type;
    }
    else{
        return &FLOAT_type;
    }
}

/*只要判断flist是否和func的flist相匹配即可。
    Exp COMMA Args
|   Exp
*/
bool analy_Args(struct tree_node* root, field_list* flist){
    struct tree_node* exp =  root->first_child;
    type* exp_type = analy_Exp(exp);
    if(exp->next_brother == NULL){        //arg -> exp
        if(flist->next != NULL)
            return false;
        else{
            if(type_equal(exp_type,flist->var->kind) == true){
                return true;
            }
            else
                return false;
        }
    }
    else{
        if(type_equal(exp_type,flist->var->kind) == true){
            return analy_Args(exp->next_brother->next_brother,flist->next);
        }
        else
            return false;
    }
}

//判断2个类型是否等价。
bool type_equal(type* a_type, type* b_type){
    if(a_type == NULL || b_type == NULL)
        return true;
    if(a_type->kind == b_type->kind){
        if(a_type->kind == BASIC){
            if(a_type->u.basic == b_type->u.basic)
                return true; 
        }
        else if(a_type->kind == ARRAY){
            int a_array_num = 0;
            type *a = a_type;
            while(a->kind == ARRAY){
                a_array_num++;
                a = a->u.array.elem;
            }
            int b_array_num = 0;
            type *b = b_type;
            while(b->kind == ARRAY){
                b_array_num++;
                b = b->u.array.elem;
            }
            if(a_array_num == b_array_num && type_equal(a,b) == true)
                return true;
        }
        else if(a_type->u.structure == b_type->u.structure){
            return true;
        }
    }
    return false;
}

#endif