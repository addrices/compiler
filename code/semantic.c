#include "m.h"

void analy_ExtDefList(struct tree_node* root);
void analy_ExtDef(struct tree_node* root);
func_node* analy_FunDec(struct tree_node* root);
func_plist* analy_VarList(struct tree_node* root);
func_plist* analy_ParamDec(struct tree_node* root);
type* analy_Specifier(struct tree_node* root);
var_node* analy_VarDec(struct tree_node* root);

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
        func_node* current = analy_FunDec(root->first_child->next_brother);
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
func_node* analy_FunDec(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    func_node* current = (func_node*)malloc(sizeof(func_node));
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

/*用于根据varList给func_node添加节点 ^.^ (整合func_plist表给func_node)
    ParamDec COMMA VarList
|   ParamDec
*/
func_plist* analy_VarList(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    if(root->first_child->next_brother == NULL){ //only ParamDec
        return analy_ParamDec(root->first_child);
    }
    else{
        func_plist* current = analy_ParamDec(root->first_child);
        current->next = analy_VarList(root->first_child->next_brother->next_brother);
        return current;
    }
}

// 将类型交给 VarDec 获得VarDec产生的变量指针整合成func_plist结点后return
// 并将产生的参数变量加入var_list中
// Specifier VarDec
func_plist* analy_ParamDec(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    type* type_node = analy_Specifier(root->first_child);
    var_node* var = analy_VarDec(root->first_child->next_brother);
    func_plist* current = (func_plist*)malloc(sizeof(struct func_plist_));
    current->next = NULL;
    return current;
}

//分析产生的Specififer并将对应的type指针返回 
type* analy_Specifier(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    type* current = NULL;
    return current;
}

var_node* analy_VarDec(struct tree_node* root){
#ifdef __DEBUG
    printf("%s\n",root->n_type);
#endif
    var_node* current = NULL;
    return current;
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