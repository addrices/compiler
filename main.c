#include "m.h"

int main(int argc, char** argv){
    root = NULL;
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    read_tree(root,0);
    return 0;
}