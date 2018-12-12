#include "m.h"

#ifdef __LAB3_ENABLE
//inter_codes* begin_intercode = NULL;
//inter_codes* end_intercode = NULL;
/*void intercode_add(inter_codes* new_code){
    if(begin_intercode == NULL){
        begin_intercode = new_code;
        end_intercode = new_code;
        end_intercode->next = NULL;
        begin_intercode->pre = NULL;
    }
    else{
        end_intercode->next = new_code;
        new_code->pre = end_intercode;
        new_code->next = NULL;
        end_intercode = new_code;
    }
    return;
}

void intercodes_add(inter_codes* new_code){
        if(begin_intercode == NULL){
        begin_intercode = new_code;
        end_intercode = new_code;
        while(end_intercode->next != NULL){
            end_intercode = end_intercode->next;
        }
        begin_intercode->pre = NULL;
    }
    else{
        end_intercode->next = new_code;
        new_code->pre = end_intercode;
        while(end_intercode->next != NULL){
            end_intercode = end_intercode->next;
        }
    }
    return;
}*/

void intercode_print(FILE* f,inter_code_page* icode){
    printf("%d",icode->num);
    inter_code* current = icode->begin;
    while(current != NULL){
        if(current->kind == LABEL_ic)
            fprintf(f, "LABEL %s :\n", current->op1.src.name);
        else if(current->kind == FUNC_ic)
            fprintf(f, "FUNCTION %s :\n", current->op1.src.name);
        else if(current->kind == ASSIGN_ic)
            fprintf(f, "%s := %s\n", current->op2.left.name, current->op2.right.name);
        else if(current->kind == ADD_ic)
            fprintf(f, "%s := %s + %s\n", current->op3.result.name, current->op3.left.name, current->op3.right.name);
        else if(current->kind == SUB_ic)
            fprintf(f, "%s := %s - %s\n", current->op3.result.name, current->op3.left.name, current->op3.right.name);
        else if(current->kind == MUL_ic)
            fprintf(f, "%s := %s * %s\n", current->op3.result.name, current->op3.left.name, current->op3.right.name);
        else if(current->kind == DIV_ic)
            fprintf(f, "%s := %s / %s\n", current->op3.result.name, current->op3.left.name, current->op3.right.name);
        else if(current->kind == GETADDR_ic)
            fprintf(f, "%s := &%s\n", current->op2.left.name, current->op2.right.name);
        else if(current->kind == GETVALUE_ic)
            fprintf(f, "%s := *%s\n", current->op2.left.name, current->op2.right.name);
        else if(current->kind == ADDRASS_ic)
            fprintf(f, "*%s := %s\n", current->op2.left.name, current->op2.right.name);
        else if(current->kind == GOTO_ic)
            fprintf(f, "GOTO %s\n", current->op1.src.name);
        else if(current->kind == IF_ic){
            switch(current->branch.relop){
                case NE: fprintf(f, "IF %s != %s GOTO %s\n", current->branch.left.name, current->branch.right.name, current->branch.dest.name); break;
                case EQ: fprintf(f, "IF %s == %s GOTO %s\n", current->branch.left.name, current->branch.right.name, current->branch.dest.name); break;
                case LT: fprintf(f, "IF %s < %s GOTO %s\n", current->branch.left.name, current->branch.right.name, current->branch.dest.name); break;
                case LE: fprintf(f, "IF %s <= %s GOTO %s\n", current->branch.left.name, current->branch.right.name, current->branch.dest.name); break;
                case GT: fprintf(f, "IF %s > %s GOTO %s\n", current->branch.left.name, current->branch.right.name, current->branch.dest.name); break;
                case GE: fprintf(f, "IF %s >= %s GOTO %s\n", current->branch.left.name, current->branch.right.name, current->branch.dest.name); break;
                default: assert(0);
            }
        }
        else if(current->kind == RETURN_ic)
            fprintf(f, "RETURN %s\n", current->op1.src.name);
        else if(current->kind == DEC_ic)
            assert(0);
        else if(current->kind == ARG_ic)
            fprintf(f, "ARG %s\n", current->op1.src.name);
        else if(current->kind == CALL_ic)
            fprintf(f, "%s := CALL %s\n", current->op2.left.name, current->op2.right.name);
        else if(current->kind == PARAM_ic)
            fprintf(f, "PARAM %s\n", current->op1.src.name);
        else if(current->kind == READ_ic)
            fprintf(f, "READ %s\n", current->op1.src.name);
        else if(current->kind == WRITE_ic)
            fprintf(f, "WRITE %s\n", current->op1.src.name);
        else
            assert(0);
        current = current->next;
    }
    return;
}

//将2个code_page合并成一个，并释放掉对应的原来的空间即可。
inter_code_page* intercode_ppmerge(inter_code_page* forward, inter_code_page* behind){
    if(forward == NULL){
        return behind;
    }
    else if(behind == NULL){
        return forward;
    }
    inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
    forward->end->next = behind->begin;
    behind->begin->pre = forward->end;
    new_page->begin = forward->begin;
    new_page->end = behind->end;
    new_page->num = forward->num + behind->num;
    free(forward);
    free(behind);
    return new_page;
}

inter_code_page* intercode_p1merge(inter_code_page* forward, inter_code* behind){
    if(forward == NULL && behind == NULL){
        return NULL;
    }
    else if(forward == NULL){
        inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
        new_page->begin = behind;
        new_page->end = behind;
        new_page->num = 1;
        return new_page;
    }
    else if(behind == NULL){
        return forward;
    }
    forward->end->next = behind;
    behind->pre = forward->end;
    forward->num++;
    forward->end = behind;
    return forward;
}

inter_code_page* intercode_1pmerge(inter_code* forward, inter_code_page* behind){
    if(forward == NULL && behind == NULL){
        return NULL;
    }
    else if(behind == NULL){
        inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
        new_page->begin = forward;
        new_page->end = forward;
        new_page->num = 1;
        return new_page;
    }
    else if(forward == NULL){
        return behind;
    }
    behind->begin->pre = forward;
    forward->next = behind->begin;
    behind->num++;
    behind->begin = forward;
    return behind;
}

inter_code_page* intercode_11merge(inter_code* forward, inter_code* behind){
    if(forward == NULL && behind == NULL){
        return NULL;
    }
    else if(behind == NULL){
        inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
        new_page->begin = forward;
        new_page->end = forward;
        new_page->num = 1;
        return new_page;
    }
    else if(forward == NULL){
        inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
        new_page->begin = behind;
        new_page->end = behind;
        new_page->num = 1;
        return new_page;
    }
    inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
    forward->next = behind;
    behind->pre = forward;
    new_page->begin = forward;
    new_page->end = behind;
    new_page->num = 2;
    return new_page;
}

inter_code_page* intercode_1merge(inter_code* forward){
    if(forward == NULL){
        return NULL;
    }
    inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
    new_page->begin = forward;
    new_page->end = forward;
    new_page->num = 1;
    return new_page;
}
#endif