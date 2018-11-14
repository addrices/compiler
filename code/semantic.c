#include "m.h"
#ifdef __LAB2_ENABLE

type INT_type = {BASIC, 1};
type FLOAT_type = {BASIC, 2};

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
        type* kind = analy_Specifier(root->first_child);
        analy_FunDec(root->first_child->next_brother, kind);
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
        strcpy(current->name,root->first_child->n_type);
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
        printf("func add error");
        assert(0);
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
    flist->next = NULL;
    return flist;
}

//分析产生的Specififer并将对应的type指针返回 
type* analy_Specifier(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->n_type, "TYPE") == 0){
        if(strcmp(root->first_child->n_value.a, "INT"))
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
            printf("struct add error\n");
            assert(0);
        }
        retype->u.structure = new_struct;
        return retype;
    }
    else{
        type* retype = (type*)malloc(sizeof(type));
        retype->kind = STRUCTURE;
        struct tree_node* tag = root->first_child->next_brother;
        struct_node* current = NULL;
        if(struct_node_search(current,tag->first_child->n_value.a) == false){
            printf("no this struct\n");
            assert(0);
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
        return new_fnode;
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
            printf("var add error\n");
            assert(0);
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

#endif