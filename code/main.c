#include "m.h"
struct tree_node *root = NULL;
bool ProFlag = true;

int main(int argc, char** argv){
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    #ifdef __LAB1_ENABLE
    if(ProFlag == true){
        read_tree(root,0);
    }
    #endif
    #ifdef __LAB2_ENABLE
    analy_Program(root);
    print_error2s();
    #ifdef __DEBUG
    print_varlist();
    print_structlist();
    print_funclist();
    #endif
    #endif
    return 0;
}