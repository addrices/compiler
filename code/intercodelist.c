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

void intercode_print(FILE* f){
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
    if(forward == NULL){
        inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
        new_page->begin = behind;
        new_page->end = behind;
        new_page->num = 1;
    }
    else if(behind == NULL){
        return forward;
    }
    forward->end->next = behind;
    behind->pre = forward->end;
    forward->num++;
    return forward;
}

inter_code_page* intercode_1pmerge(inter_code* forward, inter_code_page* behind){
    if(behind == NULL){
        inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
        new_page->begin = forward;
        new_page->end = forward;
        new_page->num = 1;
    }
    else if(forward == NULL){
        return behind;
    }
    behind->begin->pre = forward;
    forward->next = behind->begin;
    behind->num++;
    return behind;
}

inter_code_page* intercode_11merge(inter_code* forward, inter_code* behind){
    inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
    forward->next = behind;
    behind->pre = forward;
    new_page->begin = forward;
    new_page->end = behind;
    new_page->num = 2;
    return new_page;
}

inter_code_page* intercode_1merge(inter_code* forward){
    inter_code_page* new_page = (inter_code_page*)malloc(sizeof(inter_code_page));
    new_page->begin = forward;
    new_page->end = forward;
    new_page->num = 1;
    return new_page;
}
#endif