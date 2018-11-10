%{
    #include "m.h"
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
%type <type_node> DecList Dec Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

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

ExtDef : Specifier ExtDecList SEMI{
        $$ = create_node("ExtDef", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("SEMI", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, NewNode);
    }
    | Specifier SEMI{
        $$ = create_node("ExtDef", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("SEMI", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
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
    | VarDec COMMA ExtDecList{
        $$ = create_node("ExtDecList", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("COMMA", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }

Specifier : TYPE {
        $$ = create_node("Specifier", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("TYPE", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }
    | StructSpecifier{
        $$ = create_node("Specifier", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }

StructSpecifier : STRUCT OptTag LC DefList RC{
        $$ = create_node("StructSpecifier", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("STRUCT", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("LC", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RC", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
        add_sibling($2, NewNode2);
        add_sibling(NewNode2, $4);
        add_sibling($4, NewNode3);
    }
    | STRUCT Tag {
        $$ = create_node("StructSpecifier", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("STRUCT", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
    }
    | STRUCT error {
        $$ = create_node("CompSt", @$.first_line, (union n_un)0, false);
        error_line =  @$.last_line;
        yyerror("error StructSpecifier");
    }

OptTag : ID{
        $$ = create_node("OptTag", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }
    | /*empty*/{$$ = create_node("OptTag", @$.first_line, (union n_un)0, false);}

Tag : ID{
        $$ = create_node("Tag", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }

VarDec : ID {
        $$ = create_node("VarDec", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }
    | VarDec LB INT RB{
        $$ = create_node("VarDec", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("LB", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("INT", @$.first_line, (union n_un)$3, true);
        struct tree_node* NewNode3 = create_node("RB", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, NewNode3);
    }

FunDec : ID LP VarList RP{
        $$ = create_node("FunDec", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, $3);
        add_sibling($3, NewNode3);

    }
    | ID LP RP{
        $$ = create_node("FunDec", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, NewNode3);
    }
    | ID LP error {
        $$ = create_node("FunDec", @$.first_line, (union n_un)0, false);
        error_line =  @$.last_line;
        yyerror("error FunDec");
    }

VarList : ParamDec COMMA VarList{
        $$ = create_node("VarList", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("COMMA", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
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

CompSt : LC DefList StmtList RC{
        $$ = create_node("CompSt", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("LC", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("RC", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
        add_sibling($2, $3);
        add_sibling($3, NewNode2);
    }
    | error RC{
        $$ = create_node("CompSt", @$.first_line, (union n_un)0, false);
        error_line =  @$.first_line;
        yyerror("error CompSt");
    }

StmtList : Stmt StmtList{
        $$ = create_node("StmtList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | /*empty*/{$$ = create_node("StmtList", @$.first_line, (union n_un)0, false);}

Stmt : Exp SEMI{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("SEMI", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
    }
    | CompSt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | RETURN Exp SEMI{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("RETURN", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("SEMI", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
        add_sibling($2, NewNode2);
    }
    | IF LP Exp RP Stmt {
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("IF", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, $3);
        add_sibling($3, NewNode3);
        add_sibling(NewNode3, $5);
    } %prec LOWER_THAN_ELSE
    | IF LP Exp RP Stmt ELSE Stmt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("IF", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode4 = create_node("ELSE", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, $3);
        add_sibling($3, NewNode3);
        add_sibling(NewNode3, $5);
        add_sibling($5, NewNode4);
        add_sibling(NewNode4, $7);
    }
    | WHILE LP Exp RP Stmt{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("WHILE", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, $3);
        add_sibling($3, NewNode3);
        add_sibling(NewNode3, $5);
    }
    | error SEMI{
        $$ = create_node("Stmt", @$.first_line, (union n_un)0, false);
        error_line =  @$.first_line;
        yyerror("error SEMI");
    } 


DefList : Def DefList{
        $$ = create_node("DefList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        add_sibling($1, $2);
    }
    | /*empty*/{$$ = create_node("DefList", @$.first_line, (union n_un)0, false);}

Def : Specifier DecList SEMI{
        $$ = create_node("Def", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("SEMI", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, $2);
        add_sibling($2, NewNode);
    }
    | Specifier error SEMI{
        $$ = create_node("Def", @$.first_line, (union n_un)0, false);
        error_line =  @$.first_line;
        yyerror("Def error");
    } 

DecList : Dec{
        $$ = create_node("DecList", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | Dec COMMA DecList{
        $$ = create_node("DecList", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("COMMA", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }

Dec : VarDec{
        $$ = create_node("Dec", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | VarDec ASSIGNOP Exp{
        $$ = create_node("Dec", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ASSIGNOP", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }

Exp : Exp ASSIGNOP Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ASSIGNOP", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp AND Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false); 
        struct tree_node* NewNode = create_node("AND", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp OR Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("OR", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp RELOP Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("RELOP", @$.first_line, (union n_un)$2, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp PLUS Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("PLUS", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp MINUS Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("MINUS", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp STAR Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("STAR", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp DIV Exp{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("DIV", @$.first_line, (union n_un)0, true);
        add_child($$, $1);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | LP Exp RP{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
        add_sibling($2, NewNode2);
    }
    | NOT Exp {
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("NOT", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
    }
    | MINUS Exp {
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("MINUS", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, $2);
    }
    | ID LP Args RP{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, $3);
        add_sibling($3, NewNode3);

    }
    | ID LP RP{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        struct tree_node* NewNode2 = create_node("LP", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode3 = create_node("RP", @$.first_line, (union n_un)0, true);
        add_child($$, NewNode);
        add_sibling(NewNode, NewNode2);
        add_sibling(NewNode2, NewNode3);
    }
    | Exp LB Exp RB{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        struct tree_node* NewNode = create_node("LB", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("RB", @$.first_line, (union n_un)0, true);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
        add_sibling($3, NewNode2);
    }
    | Exp DOT ID{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        struct tree_node* NewNode = create_node("DOT", @$.first_line, (union n_un)0, true);
        struct tree_node* NewNode2 = create_node("ID", @$.first_line, (union n_un)$3, true);
        add_sibling($1, NewNode);
        add_sibling(NewNode, NewNode2);
    }
    | ID{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("ID", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }
    | INT{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("INT", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }
    | FLOAT{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        struct tree_node* NewNode = create_node("FLOAT", @$.first_line, (union n_un)$1, true);
        add_child($$, NewNode);
    }

Args : Exp COMMA Args{
        $$ = create_node("Args", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
        struct tree_node* NewNode = create_node("COMMA", @$.first_line, (union n_un)0, true);
        add_sibling($1, NewNode);
        add_sibling(NewNode, $3);
    }
    | Exp {
        $$ = create_node("Args", @$.first_line, (union n_un)0, false);
        add_child($$, $1);
    }
    | Exp error Args{
        $$ = create_node("Exp", @$.first_line, (union n_un)0, false);
        error_line =  @$.first_line;
        yyerror("error exp");        
    }

%%
#include "lex.yy.c"
int yyerror(char* msg) {
    ProFlag = false;
    if(strcmp(msg,"syntax error") != 0)
        printf("Error type B at Line %d: %s\n", error_line, msg);
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
    if(strcmp(root_node->n_type,"INT") == 0)
        printf("INT: %d \n", root_node->n_value.n_value_i);
    else if(strcmp(root_node->n_type,"FLOAT") == 0)
        printf("FLOAT: %f \n", root_node->n_value.n_value_f);
    else if(strcmp(root_node->n_type,"RELOP") == 0){
        if(root_node->n_value.n_value_i == 1)
            printf("RELOP: <\n");
        if(root_node->n_value.n_value_i == 2)
            printf("RELOP: >\n");
        if(root_node->n_value.n_value_i == 3)
            printf("RELOP: ==\n");
        if(root_node->n_value.n_value_i == 4)
            printf("RELOP: <=\n");
        if(root_node->n_value.n_value_i == 5)
            printf("RELOP: >=\n");
        if(root_node->n_value.n_value_i == 6)
            printf("RELOP: !=\n");
    }
    else if(strcmp(root_node->n_type, "ID") == 0){
        printf("ID: %s\n", root_node->n_value.a);
    }
    else if(strcmp(root_node->n_type, "TYPE") == 0){
        printf("TYPE: %s\n", root_node->n_value.a);
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