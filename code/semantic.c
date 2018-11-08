#include "m.h"

void analy_ExtDefList(struct tree_node* root);
void analy_ExtDef(struct tree_node* root);
void analy_FunDec(struct tree_node* root, struct func_node* current);
void analy_VarList(struct tree_node* root, struct func_node* current);
void analy_ParamDec(struct tree_node* root, struct var_node* current);
void analy_Specifier(struct tree_node* root, struct var_node* current);
struct struct_node* analy_StructSpecifier(struct tree_node* root);
struct var_node* analy_Def(struct tree_node* root);
struct var_node* analy_Def(struct tree_node* root);

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

void analy_ExtDef(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(strcmp(root->first_child->next_brother->n_type, "FunDec") == 0){
        struct func_node *current = (struct func_node*)malloc(sizeof(struct func_node));
        current->next = NULL;
        //analy_Specifier
        analy_FunDec(root->first_child->next_brother,current);
        //analy_Compst
        //add node
    }
    else if(strcmp(root->first_child->next_brother->n_type, "SEMI") == 0)
        printf("struct def");
    return;
}

//函数的定义处理
void analy_FunDec(struct tree_node* root, struct func_node* current){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif    
    if(strlen(root->first_child->n_type) < __MAX_NAME_LENGTH)
        strcpy(current->name,root->first_child->n_type);
    else{
        printf("func name too long\n");
        assert(0);
    }
    if(strcmp(root->first_child->next_brother->next_brother->n_type,"VarList") == 0){
        current->parameter_num = 0;
        analy_VarList(root->first_child->next_brother->next_brother,current);
    }
    else{
        current->parameter_num = 0;
        current->parameters = NULL;
    }
    return;
}

//用于根据varList给func_node添加节点，这里写的不好，这个函数是给上游的func用的，不能复用
//，但因为也就func用他所以没关系，有空再重写把 ^.^
void analy_VarList(struct tree_node* root, struct func_node* current){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    int p_num = 1;
    struct tree_node* p = root;
    while(p->first_child->next_brother != NULL){
        p_num++;
        p = p->first_child->next_brother->next_brother;
    }
    printf("p_num %d\n",p_num);
    current->parameter_num = p_num;
    current->parameters = (struct var_node**)malloc(sizeof(struct var_node*) * p_num);
    p = root->first_child;
    for(int i = 0; i < p_num ;i++){
        current->parameters[i] = (struct var_node*)malloc(sizeof(struct var_node));
        analy_ParamDec(p,current->parameters[i]);
        if(i != p_num - 1)
            p = p->next_brother->next_brother->first_child;
    }
    return; 
}

//产生变量，加入变量表 >.<
void analy_ParamDec(struct tree_node* root, struct var_node* current){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    analy_Specifier(root->first_child, current);
    //analy_VarDec(root->first_child->next_brother);
    //add to varlist;
    return;
}

//分析变量的类型，如果是struct加入结构体表中 》—《
void analy_Specifier(struct tree_node* root, struct var_node* current){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
    printf("%s\n",root->first_child->n_type);
#endif
    if(strcmp(root->first_child->n_type,"TYPE") == 0){
        struct tree_node* TYPE = root->first_child;
        printf("TYPE %s\n",TYPE->n_value.a);
        current->type = TYPE->n_value.a[0];
        current->array = NULL;
        current->struct_type = NULL;
    }
    else{
        current->type = 's';
        current->array = NULL;
        current->struct_type = analy_StructSpecifier(root->first_child);
    }
    return;
}

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
        }*/
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
        if(def_num == 0)
            current->members = NULL;
        else
            current->members = (struct var_node**)malloc(sizeof(struct var_node*) * def_num);
        p = DefList;
        for(int i = 0; i < def_num;i++){
            current->members[i] = analy_Def(p->first_child);
            p = p->first_child->next_brother;
        }
    }
}

struct var_node* analy_Def(struct tree_node* root){
    return NULL;
}