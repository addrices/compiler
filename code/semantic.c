#include "m.h"
#ifdef __LAB2_ENABLE

type INT_type = {BASIC, 1};
type FLOAT_type = {BASIC, 2};

void analy_ExtDefList(struct tree_node* root);
void analy_ExtDef(struct tree_node* root);
func_node* analy_FunDec(struct tree_node* root,type *kind);
field_list* analy_VarList(struct tree_node* root);
field_list* analy_ParamDec(struct tree_node* root);
type* analy_Specifier(struct tree_node* root);
var_node* analy_VarDec(struct tree_node* root,type* kind);
field_list* analy_DefList(struct tree_node* root,bool flag);

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

//分析一个大定义
void analy_ExtDef(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->next_brother->n_type, "FunDec") == 0){
        type* return_kind = analy_Specifier(root->first_child);
        func_node* current = analy_FunDec(root->first_child->next_brother, return_kind);
        current->return_type = return_kind;
        //add_node analy_FunDec
    }
    else if(strcmp(root->first_child->next_brother->n_type, "SEMI") == 0){
        //add
    }
        printf("struct def");
    return;
}

/*函数的定义处理
    ID LP VarList RP
|   ID LP RP
*/
func_node* analy_FunDec(struct tree_node* root,type *kind){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    func_node* current = (func_node*)malloc(sizeof(func_node));
    current->return_type = kind;
    if(strlen(root->first_child->n_type) < __MAX_NAME_LENGTH)
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
    field_list* flist = analy_VarDec(root->first_child->next_brother, type_node,true);
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

//分析声明的结构体的结构。加入结构体表。
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
        else
            strcpy(new_struct->name, opttag->first_child->n_value.a);
        new_struct->next = NULL;
        new_struct->list = analy_DefList(root->first_child->next_brother->next_brother->next_brother);
    }
    else{
        type* retype = (type*)malloc(sizeof(type));
        retype->kind = STRUCTURE;
        struct tree_node* tag = root->first_child->next_brother;
        //retype->u.structure = seek_struct(tag->first_child->n_value.a);
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
        field_list* current;
        while(current->next != NULL)
            current = current->next;
        current->next = analy_DefList(root->first_child->next_brother,flag);
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


//这个要返回变量，因为要处理数组。
var_node* analy_VarDec(struct tree_node* root,type* kind){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    return NULL;
}

/*
//structspecifier中有定义和引用
//struct tag是引用 STRUCT OptTag LC DefList RC中有定义的行为
//用返回值的形式好像好一点 ？.？ 返回定义在结构体表中的指针
struct struct_node* analy_StructSpecifier(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->next_brother->n_type,"Tag") == 0){
        //search struct list;
        struct struct_node* current;
        char name[16];
        strcpy(name,root->first_child->next_brother->first_child->n_value.a);
        /*if(search_struct(current,name) == true)
            return current;
        else{
            //error!!!;
            assert(0);
        }
    }
    else{
        //create a struct node;
        struct struct_node* current = (struct struct_node*)malloc(sizeof(struct struct_node));        
        char name[16];
        if(root->first_child->next_brother->first_child == NULL)//optTag = empty;
            name[0] = '\0';
        else
            strcpy(name, root->first_child->next_brother->first_child->n_value.a);
        struct tree_node* DefList = root->first_child->next_brother->next_brother->next_brother;
        int def_num = 0;//这里不直接分析DefList是因为结构体表中的member需要他们的指针～～～而且DefList有要复用。。
        struct tree_node* p = DefList;
        while(p->first_child != NULL){
            p = p->first_child->next_brother;
            def_num++;
        }
        if(def_num == 0){
            current->members = NULL;
            return;
        }
        else
            current->members = (struct var_node**)malloc(sizeof(struct var_node*) * def_num);
        p = DefList;
        for(int i = 0; i < def_num;i++){
            //current->members[i] = analy_Def(p->first_child);
            p = p->first_child->next_brother;
        }
    }
}

struct var_node* analy_Def(struct tree_node* root){
    
}*/

#endif