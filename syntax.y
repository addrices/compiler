%{
    #include "m.h"

    union n_un{
        int n_value_i;
        float n_value_f;
        char* a;
    };

    struct tree_node{
        struct tree_node* first_child;
        struct tree_node* next_brother;
        char n_type[32];
        int n_length;
        union n_un n_value;
        bool if_lex;
    };


    struct tree_node* create_node(char* a, int length, union n_un value,bool flag);
    void add_child(struct tree_node* a, struct tree_node* b);
    void add_sibling(struct tree_node* a, struct tree_node* b);
    void read_tree(struct tree_node* root_node,int i);
%}
%union {
    int type_int;
    float type_float;
    int type_relop;
    char* type_id;
    char* type_type;
    struct tree_node* type_node;
}
%token <type_int> INT 
%token <type_float> FLOAT
%token <type_relop> RELOP
%token <type_id> ID
%token <type_type> TYPE
%token SEMI ASSIGNOP COMMA PLUS MINUS STAR DIV AND NOT
%token OR LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE DOT

%type <type_node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def
%type <type_node> DecList Dec Exp Args Int Float Relop Id Type Semi Assignop Comma Plus Minus Star Div
%type <type_node> And Not Or Lp Rp Lb Rb Lc Rc Struct Return If Else While Dot Negate

%right Assignop
%left Or
%left And
%left Relop
%left Add Minus
%left Mul div
%right Not Negate
%left Lp Rp Lb Rb

%%
Program : ExtDefList {
        $$ = create_node("Program", @$.first_line, (union n_un)0, false);
        root = $$;
        add_child($$, $1);
    }

ExtDefList : ExtDef ExtDefList {
        $$ = create_node("ExtDefList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | /*empty*/{$$ = create_node("ExtDefList", @$.first_line, (union n_un)0, false);}

ExtDef : Specifier ExtDecList Semi{
        $$ = create_node("ExtDef", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Specifier Semi{
        $$ = create_node("ExtDef", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | Specifier FunDec CompSt{
        $$ = create_node("ExtDef", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }

ExtDecList : VarDec{
        $$ = create_node("ExtDecList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | VarDec Comma ExtDecList{
        $$ = create_node("ExtDecList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }

Specifier : Type {
        $$ = create_node("Specifier", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | StructSpecifier{
        $$ = create_node("Specifier", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }

StructSpecifier : Struct OptTag Lc DefList Rc{
        $$ = create_node("StructSpecifier", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
        add_sibling($4, $5);
    }
    | Struct Tag {
        $$ = create_node("StructSpecifier", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }

OptTag : Id{
        $$ = create_node("OptTag", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | /*empty*/{$$ = create_node("OptTag", @$.first_line, (union n_un)0, false);}

Tag : Id{
        $$ = create_node("Tag", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }

VarDec : Id
    | VarDec Lb Int Rb{
        $$ = create_node("VarDec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
    }

FunDec : Id Lp VarList Rp{
        $$ = create_node("FunDec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);

    }
    | Id Lp Rp{
        $$ = create_node("FunDec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }

VarList : ParamDec Comma VarList{
        $$ = create_node("VarList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | ParamDec{
        $$ = create_node("VarList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }


ParamDec : Specifier VarDec{
        $$ = create_node("ParamDec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }

CompSt : Lc DefList StmtList Rc{
        $$ = create_node("CompSt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
    }

StmtList : Stmt StmtList{
        $$ = create_node("StmtList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | /*empty*/{$$ = create_node("StmtList", @$.first_line, (union n_un)0, false);}

Stmt : Exp Semi{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | CompSt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | Return Exp Semi{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | If Lp Exp Rp Stmt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
        add_sibling($4, $5);
    }
    | If Lp Exp Rp Stmt Else Stmt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
        add_sibling($4, $5);
        add_sibling($5, $6);
        add_sibling($6, $7);
    }
    | While Lp Exp Rp Stmt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
        add_sibling($4, $5);
    }

DefList : Def DefList{
        $$ = create_node("DefList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | /*empty*/{$$ = create_node("DefList", @$.first_line, (union n_un)0, false);}

Def : Specifier DecList Semi{
        $$ = create_node("Def", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }

DecList : Dec{
        $$ = create_node("DecList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | Dec Comma DecList{
        $$ = create_node("DecList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }

Dec : VarDec{
        $$ = create_node("Dec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    |VarDec Assignop Exp{
        $$ = create_node("Dec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }

Exp : Exp Assignop Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp And Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Or Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Relop Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Plus Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Minus Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Star Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Div Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Lp Exp Rp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Negate Exp {
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | Not Exp {
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | Id Lp Args Rp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);

    }
    | Id Lp Rp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp Lb Exp Rb{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
        add_sibling($3, $4);
    }
    | Exp Dot Id{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Id{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | Int{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | Float{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }

Args : Exp Comma Args{
        $$ = create_node("Args", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, $3);
    }
    | Exp {
        $$ = create_node("Args", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }

Int : INT{ $$ = create_node("Int", @$.first_line, (union n_un)$1, true);}
Float : FLOAT { $$ = create_node("Float", @$.first_line, (union n_un)$1, true);}
Relop : RELOP { $$ = create_node("Relop", @$.first_line, (union n_un)$1, true);}
Id : ID{ $$ = create_node("Id", @$.first_line, (union n_un)$1, true);}
Type : TYPE { $$ = create_node("Type", @$.first_line, (union n_un)$1, true);}
Semi : SEMI { $$ = create_node("Semi", @$.first_line, (union n_un)0, true);}
Assignop : ASSIGNOP { $$ = create_node("Assignop", @$.first_line, (union n_un)0, true);}
Comma : COMMA { $$ = create_node("Comma", @$.first_line, (union n_un)0, true);}
Plus : PLUS { $$ = create_node("Plus", @$.first_line, (union n_un)0, true);}
Minus : MINUS { $$ = create_node("Minus", @$.first_line, (union n_un)0, true);}
Not : NOT { $$ = create_node("Not", @$.first_line, (union n_un)0, true);}
Star : STAR { $$ = create_node("Star", @$.first_line, (union n_un)0, true);}
Div : DIV { $$ = create_node("Div", @$.first_line, (union n_un)0, true);}
And : AND { $$ = create_node("And", @$.first_line, (union n_un)0, true);}
Or : OR { $$ = create_node("Or", @$.first_line, (union n_un)0, true);}
Lp : LP { $$ = create_node("Lp", @$.first_line, (union n_un)0, true);}
Rp : RP { $$ = create_node("Rp", @$.first_line, (union n_un)0, true);}
Lb : LB { $$ = create_node("LB", @$.first_line, (union n_un)0, true);}
Rb : RB { $$ = create_node("Rb", @$.first_line, (union n_un)0, true);}
Lc : LC { $$ = create_node("Lc", @$.first_line, (union n_un)0, true);}
Rc : RC { $$ = create_node("Rc", @$.first_line, (union n_un)0, true);}
Struct : STRUCT { $$ = create_node("Struct", @$.first_line, (union n_un)0, true);}
Return : RETURN { $$ = create_node("Return", @$.first_line, (union n_un)0, true);}
If : IF { $$ = create_node("If", @$.first_line, (union n_un)0, true);}
Else : ELSE { $$ = create_node("Else", @$.first_line, (union n_un)0, true);}
While : WHILE { $$ = create_node("While", @$.first_line, (union n_un)0, true);}
Dot : DOT {$$ = create_node("Dot", @$.first_line, (union n_un)0, true);}
Negate : MINUS { $$ = create_node("Negate", @$.first_line, (union n_un)0, true);}

%%
#include "lex.yy.c"
int yyerror(char* msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}

struct tree_node* create_node(char* a, int length, union n_un value, bool flag){
    struct tree_node* n_node = (struct tree_node*)malloc(sizeof(struct tree_node));
    strcpy(n_node->n_type, a);
    n_node->n_length = length;
    n_node->n_value = value;
    n_node->next_brother = NULL;
    n_node->first_child = NULL;
    n_node->if_lex = flag;
    return n_node;
}

void add_child(struct tree_node* a, struct tree_node* b){
    a->first_child = b;
    return;
}

void add_sibling(struct tree_node* a, struct tree_node* b){
    a->next_brother = b;
    return;
}

void read_tree(struct tree_node* root_node,int i){
    if(root_node == NULL)
        return;
    if(root_node->if_lex == false && root_node->first_child == NULL)
        goto A;
    for(int k = 0; k < i; k++)
        printf("  ");
    if(strcmp(root_node->n_type,"Int") == 0)
        printf("Int: %d \n", root_node->n_value.n_value_i);
    else if(strcmp(root_node->n_type,"Float") == 0)
        printf("Float: %f \n", root_node->n_value.n_value_f);
    else if(strcmp(root_node->n_type,"Relop") == 0){
        if(root_node->n_value.n_value_i == 1)
            printf("Relop: <\n");
        if(root_node->n_value.n_value_i == 2)
            printf("Relop: >\n");
        if(root_node->n_value.n_value_i == 3)
            printf("Relop: ==\n");
        if(root_node->n_value.n_value_i == 4)
            printf("Relop: <=\n");
        if(root_node->n_value.n_value_i == 5)
            printf("Relop: >=\n");
        if(root_node->n_value.n_value_i == 6)
            printf("Relop: !=\n");
    }
    else if(strcmp(root_node->n_type, "Id") == 0){
        printf("Id: %s\n", root_node->n_value.a);
    }
    else if(strcmp(root_node->n_type, "Type") == 0){
        printf("Type: %s\n", root_node->n_value.a);
    }
    else if(root_node->if_lex == true)
        printf("%s \n", root_node->n_type);
    else
        printf("%s (%d)\n", root_node->n_type, root_node->n_length);
    read_tree(root_node->first_child,i+1);
    A:
    read_tree(root_node->next_brother,i);
    return;
}