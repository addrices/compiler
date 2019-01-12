#include "m.h"
#ifdef __LAB4_ENABLE
void divide_block(inter_code_page* icode){
    inter_code* current = icode->begin;
    int num = 0;
    block_page* new_block = (block_page*)malloc(sizeof(block_page));
    block_head = new_block;
    block_page* current_block = new_block;
    new_block->begin = icode->begin;
    new_block->num = 0;
    for(int i = 0;i < icode->num; i++){
        if(current->kind == FUNC_ic || current->kind == LABEL_ic){
            if(current_block->begin == current){
                current = current->next;
                continue;
            }
            block_page* new_block = (block_page*)malloc(sizeof(block_page));
            new_block->begin = current;
            new_block->num = i;
            current_block->end = current->pre;
            current_block->num = i - current_block->num;
            current_block->next = new_block;
            current_block = new_block;
        }
        else if(current->kind == GOTO_ic || current->kind == IF_ic || current->kind == RETURN_ic || current->kind == CALL_ic){
            if(i == icode->num-1){
                current_block->num = i - current_block->num + 1;
                current_block->end = current;
                current_block->next = NULL;
                current = current->next;
                continue;
            }
            block_page* new_block = (block_page*)malloc(sizeof(block_page));
            new_block->begin = current->next;
            new_block->num = i+1;
            current_block->num = i+1 - current_block->num;
            current_block->end = current;
            current_block->next = new_block;
            current_block = new_block;
        }
        current = current->next;
    }
}
void print_block(){
    printf("\n");
    block_page* current = block_head;
    while(current != NULL){
        printf("%d ",current->num);
        current = current->next;
    }
    printf("\n");
}



void assemblycode_print(FILE* f,inter_code_page* icode){
    inter_code* current = icode->begin;
    while(current != icode->end->next){
        if(current->kind == LABEL_ic)
            fprintf(f, "label%s:\n", current->op1.src.name);
        else if(current->kind == FUNC_ic)
            fprintf(f, "%s:\n", current->op1.src.name);
        else if(current->kind == ASSIGN_ic){
            int left_reg = getreg(current->op2.left.name);
            if(current->op2.right.name[0] == '#'){
                int constant = atoi(&current->op2.right.name[1]);
                fprintf(f, "li $%d, %d\n", left_reg, constant);
            }
            else{
                int right_reg = getreg(current->op2.right.name);
                fprintf(f, "move $%d, $%d\n", left_reg, right_reg);
            }
        }
        else if(current->kind == ADD_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant + right_constant;
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "addi $%d, $%d, %d\n", result_reg, right_reg, left_constant);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(current->op3.left.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "addi $%d, $%d, %d\n", result_reg, left_reg, right_constant);
            }
            else{
                int left_reg = getreg(current->op3.left.name);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "add $%d, $%d, $%d\n", result_reg, left_reg, right_reg);
            }
        }
        else if(current->kind == SUB_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant - right_constant;
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "addi $%d, $%d, -%d\n", result_reg, right_reg, left_constant);
                fprintf(f, "sub $%d, $0, %d\n",result_reg, result_reg);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(current->op3.left.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "addi $%d, $%d, -%d\n", result_reg, left_reg, right_constant);
            }
            else{
                int left_reg = getreg(current->op3.left.name);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "sub $%d, $%d, $%d\n", result_reg, left_reg, right_reg);
            }
        }           
        else if(current->kind == MUL_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant * right_constant;
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                int empty_reg = getreg("$");
                fprintf(f, "li $%d, %d\n", empty_reg, left_constant);
                fprintf(f, "mul $%d, $%d, $%d\n", result_reg, right_reg, empty_reg);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(current->op3.left.name);
                int result_reg = getreg(current->op3.result.name);
                int empty_reg = getreg("$");
                fprintf(f, "li $%d, %d\n", empty_reg, right_constant);
                fprintf(f, "mul $%d, $%d, %d\n", result_reg, left_reg, empty_reg);
            }
            else{
                int left_reg = getreg(current->op3.left.name);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "mul $%d, $%d, $%d\n", result_reg, left_reg, right_reg);
            }
        }
        else if(current->kind == DIV_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant / right_constant;
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                int empty_reg = getreg("$");
                fprintf(f, "li $%d, %d\n", empty_reg, left_constant);
                fprintf(f, "div $%d, $%d\n", empty_reg, right_reg);
                fprintf(f, "mflo $%d\n", result_reg);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(current->op3.left.name);
                int result_reg = getreg(current->op3.result.name);
                int empty_reg = getreg("$");
                fprintf(f, "li $%d, %d\n", empty_reg, right_constant);
                fprintf(f, "div $%d, $%d\n", left_reg, empty_reg);
                fprintf(f, "mflo $%d\n", result_reg);
            }
            else{
                int left_reg = getreg(current->op3.left.name);
                int right_reg = getreg(current->op3.right.name);
                int result_reg = getreg(current->op3.result.name);
                fprintf(f, "div $%d, $%d\n", left_reg, right_reg);
                fprintf(f, "mfio $%d\n", result_reg);
            }
        }
        else if(current->kind == GOTO_ic){
            fprintf("j lable%s\n", current->op1.src.name);
        }           
        else if(current->kind == IF_ic){
            int left_reg = getreg(current->op3.left.name);
            int right_reg = getreg(current->op3.right.name);
            switch(current->branch.relop){
                case NE: fprintf(f, "bne $%d, $%d, label%s\n", left_reg, right_reg, current->branch.dest.name); break;
                case EQ: fprintf(f, "beq $%d, $%d, label%s\n", left_reg, right_reg, current->branch.dest.name); break;
                case LT: fprintf(f, "blt $%d, $%d, label%s\n", left_reg, right_reg, current->branch.dest.name); break;
                case LE: fprintf(f, "ble $%d, $%d, label%s\n", left_reg, right_reg, current->branch.dest.name); break;
                case GT: fprintf(f, "bgt $%d, $%d, label%s\n", left_reg, right_reg, current->branch.dest.name); break;
                case GE: fprintf(f, "bge $%d, $%d, label%s\n", left_reg, right_reg, current->branch.dest.name); break;
                default: assert(0);
            }
        }
        else if(current->kind == READ_ic){
            int result_reg = getreg(current->op1.src.name);
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "jal read\n");
            fprintf(f, "lw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, 4\n");
            fprintf(f, "move $%d, $v0\n", result_reg);
        }
        else if(current->kind == WRITE_ic){
            int result_reg = getreg(current->op1.src.name);
            fprintf(f, "move $a0, $%d\n", result_reg);
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "jal write\n");
            fprintf(f, "lw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, 4\n");
        }
        else if(current->kind == RETURN_ic){
            int result_reg = getreg(current->op1.src.name);
            fprintf(f, "move $v0 $%d\n",result_reg);
            fprintf(f, "jr $ra\n");
        }
        else{
            assert(0);
        }
        current = current->next;
    }
    return;
}

#endif