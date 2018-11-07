#include "m.h"

int main(int argc, char** argv){
    root = NULL;
    ProFlag = true;
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
    analy_tree(root);
    #endif
    return 0;
}