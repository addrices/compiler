#include "m.h"
struct tree_node *root = NULL;
bool ProFlag = true;

int main(int argc, char** argv){
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    #ifdef __LAB3_ENABLE
    init_symbol();
    #endif
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
    if(ProFlag == true){
        analy_Program(root);
        ProFlag = print_error2s();
        #ifdef __DEBUG
        print_varlist();
        print_structlist();
        print_funclist();
        #endif
    }
    #endif

    #ifdef __LAB3_ENABLE
    if(ProFlag){
        FILE* w = fopen(argv[2], "w");
        if (!w)
        {
            perror(argv[2]);
            return 1;
        }
        inter_code_page* icode = translate(root);
        intercode_print(w, icode); //TODO: delete
    #ifndef __LAB4_ENABLE
        intercode_print(w, icode);
    #endif
    #ifdef __LAB4_ENABLE
        divide_block(icode);
        print_block();
        //assemblycode_print(w, icode);
    #endif
        fclose(w);
    }
    #endif
    return 0;
}