#include "m.h"

#ifdef __LAB4_ENABLE
void begin_code_print(FILE* f);
void funccode_print(FILE* f, func_info* func);
void blockcode_print(FILE* f,block_page* block,func_info* func,graph_node* vars, int num);

void assemble_print(FILE* f, func_info* func){
    begin_code_print(f);
    func_info* current_func = func;
    while(current_func != NULL){
        funccode_print(f, current_func);  
        current_func = current_func->next_func;
    }
}

void begin_code_print(FILE* f){
    fprintf(f, ".data\n");
    fprintf(f, "_prompt: .asciiz \"Enter an Integer:\"\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, ".globl main\n");
    fprintf(f, ".text\n");
    fprintf(f, "read:\n");
    fprintf(f, "li $v0, 4\n");
    fprintf(f, "la $a0, _prompt\n");
    fprintf(f, "syscall\n");
    fprintf(f, "li $v0, 5\n");
    fprintf(f, "syscall\n");
    fprintf(f, "jr $ra\n");
    fprintf(f, "write:\n");
    fprintf(f, "li $v0, 1\n");
    fprintf(f, "syscall\n");
    fprintf(f, "li $v0, 4\n");
    fprintf(f, "la $a0, _ret\n");
    fprintf(f, "syscall\n");
    fprintf(f, "move $v0, $0\n");
    fprintf(f, "jr $ra\n");
}

void funccode_print(FILE* f, func_info* func){
    block_page* current_page = func->block_list;
    inter_code* FUNCTION = current_page->begin;
    fprintf(f, "%s:\n", FUNCTION->op1.src.name);
    fprintf(f, "addi $sp, $sp, %d\n",-func->size);
    while(current_page != NULL){
        int num = 0;
        graph_node* graph_head = block_graph_color(func, current_page, &num);
        for(int i = 0; i < 17; i++){
            regs_info[i].valid = false;
        }
        blockcode_print(f, current_page, func, graph_head, num);
        current_page = current_page->next;
    }
}

void blockcode_print(FILE* f,block_page* block,func_info* func,graph_node* vars, int num){
    inter_code* current = block->begin;
    for(int i = 0; i < block->num; i++){
        if(i == block->num -1)
            update_newblock(f);
        if(current->kind == LABEL_ic)
            fprintf(f, "label%s:\n", current->op1.src.name);
        else if(current->kind == FUNC_ic){
            inter_code* p_current = current->next;
            int p_num = 0;
            while(p_current->kind == PARAM_ic){
                p_num++;
                p_current = p_current->next;
            }
            for(int j = 0; j < p_num; j++){
                current = current->next;
                i++;
                if(j < 4){
                    int reg_num = getreg(f,current->op1.src.name,1,func,vars,num);
                    fprintf(f,"move $%d, $%d\n",reg_num, j+4);
                }
                else{
                    int shift = 4 + (p_num - j)*4;
                    int reg_num = getreg(f,current->op1.src.name,1,func,vars,num);
                    fprintf(f,"lw $%d, %d($sp)\n",reg_num,shift + func->size);
                }
            }
        }
        else if(current->kind == ASSIGN_ic){
            int left_reg = getreg(f, current->op2.left.name,1,func,vars,num);
            if(current->op2.right.name[0] == '#'){
                int constant = atoi(&current->op2.right.name[1]);
                fprintf(f, "li $%d, %d\n", left_reg, constant);
            }
            else{
                int right_reg = getreg(f, current->op2.right.name,2,func,vars,num);
                fprintf(f, "move $%d, $%d\n", left_reg, right_reg);
            }
        }
        else if(current->kind == ADD_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant + right_constant;
                int result_reg = getreg(f, current->op3.result.name,1,func,vars,num);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "addi $%d, $%d, %d\n", result_reg, right_reg, left_constant);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "addi $%d, $%d, %d\n", result_reg, left_reg, right_constant);
            }
            else{
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "add $%d, $%d, $%d\n", result_reg, left_reg, right_reg);
            }
        }
        else if(current->kind == SUB_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant - right_constant;
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "addi $%d, $%d, -%d\n", result_reg, right_reg, left_constant);
                fprintf(f, "sub $%d, $0, %d\n",result_reg, result_reg);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "addi $%d, $%d, -%d\n", result_reg, left_reg, right_constant);
            }
            else{
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "sub $%d, $%d, $%d\n", result_reg, left_reg, right_reg);
            }
        }           
        else if(current->kind == MUL_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant * right_constant;
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                int empty_reg = 25;
                fprintf(f, "li $%d, %d\n", empty_reg, left_constant);
                fprintf(f, "mul $%d, $%d, $%d\n", result_reg, right_reg, empty_reg);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                int empty_reg = 25;
                fprintf(f, "li $%d, %d\n", empty_reg, right_constant);
                fprintf(f, "mul $%d, $%d, $%d\n", result_reg, left_reg, empty_reg);
            }
            else{
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "mul $%d, $%d, $%d\n", result_reg, left_reg, right_reg);
            }
        }
        else if(current->kind == DIV_ic){
            if(current->op3.left.name[0] == '#' && current->op3.right.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_constant = atoi(&current->op3.right.name[1]);
                int result_constant = left_constant / right_constant;
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "li $%d, %d\n", result_reg, result_constant);
            }
            else if(current->op3.left.name[0] == '#'){
                int left_constant = atoi(&current->op3.left.name[1]);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                int empty_reg = 25;
                fprintf(f, "li $%d, %d\n", empty_reg, left_constant);
                fprintf(f, "div $%d, $%d\n", empty_reg, right_reg);
                fprintf(f, "mflo $%d\n", result_reg);
            }            
            else if(current->op3.right.name[0] == '#'){
                int right_constant = atoi(&current->op3.right.name[1]);
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                int empty_reg = 25;
                fprintf(f, "li $%d, %d\n", empty_reg, right_constant);
                fprintf(f, "div $%d, $%d\n", left_reg, empty_reg);
                fprintf(f, "mflo $%d\n", result_reg);
            }
            else{
                int left_reg = getreg(f,current->op3.left.name,2,func,vars,num);
                int right_reg = getreg(f,current->op3.right.name,2,func,vars,num);
                int result_reg = getreg(f,current->op3.result.name,1,func,vars,num);
                fprintf(f, "div $%d, $%d\n", left_reg, right_reg);
                fprintf(f, "mflo $%d\n", result_reg);
            }
        }
        else if(current->kind == GOTO_ic){
            fprintf(f, "j label%s\n", current->op1.src.name);
        }           
        else if(current->kind == IF_ic){
            int left_reg = 0;
            int right_reg = 0;
            if(current->branch.left.name[0] == '#' && current->branch.right.name[0] == '#'){
                int left_num = atoi(&current->branch.left.name[1]);
                left_reg = 2;
                fprintf(f, "li $2, %d\n",left_num);
                right_reg = getreg(f,current->branch.right.name,2,func,vars,num);  
            }
            else{
                left_reg = getreg(f,current->branch.left.name,2,func,vars,num);
                right_reg = getreg(f,current->branch.right.name,2,func,vars,num);
            }
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
            int result_reg = getreg(f,current->op1.src.name,1,func,vars,num);
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "jal read\n");
            fprintf(f, "addi $sp, $sp, 4\n");
            fprintf(f, "lw $ra, 0($sp)\n");
            fprintf(f, "move $%d, $v0\n", result_reg);
        }
        else if(current->kind == WRITE_ic){
            int result_reg = getreg(f,current ->op1.src.name,2,func,vars,num);
            fprintf(f, "move $a0, $%d\n", result_reg);
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "jal write\n");
            fprintf(f, "addi $sp, $sp, 4\n");
            fprintf(f, "lw $ra, 0($sp)\n");
        }
        else if(current->kind == RETURN_ic){
            int result_reg = getreg(f,current->op1.src.name,2,func,vars,num);
            fprintf(f, "move $v0 $%d\n",result_reg);
            fprintf(f, "addi $sp, $sp, %d\n",func->size);
            fprintf(f, "jr $ra\n");
        }
        else if(current->kind == GETADDR_ic){
            int result_reg = getreg(f,current->op2.left.name,1,func,vars,num);
            graph_node* current_vars = vars;
            while(current_vars != NULL){
                if(strcmp(current->op2.right.name, current_vars->name) == 0)
                    break;
                current_vars = current_vars->next;
            }
            fprintf(f, "addi $%d, $sp, %d\n",result_reg, current_vars->var->sp_shift);
        }
        else if(current->kind == GETVALUE_ic){
            int result_reg = getreg(f,current->op2.left.name,1,func,vars,num);
            int left_reg = getreg(f,current->op2.right.name,2,func,vars,num);
            fprintf(f, "lw $%d, 0($%d)\n",result_reg, left_reg);
        }
        else if(current->kind == ADDRASS_ic){
            int result_reg = getreg(f,current->op2.left.name,2,func,vars,num);
            int left_reg = getreg(f,current->op2.right.name,2,func,vars,num);
            fprintf(f, "sw $%d, 0($%d)\n",left_reg, result_reg);
        }
        else if(current->kind == DEC_ic || current->kind == PARAM_ic){
            ;
        }
        else if(current->kind == ARG_ic){
            int param_num = 0;
            int sp_size = 0;
            while(current->kind == ARG_ic){
                if(param_num < 4){
                    int reg = getreg(f,current->op1.src.name,2,func,vars,num);
                    fprintf(f,"move $%d, $%d\n",4+param_num,reg);
                    param_num = param_num + 1;
                    current = current->next;
                    i++;
                }
                else{
                    int reg = getreg(f,current->op1.src.name,2,func,vars,num);
                    fprintf(f,"sw $%d, -%d($sp)\n",reg,sp_size);
                    param_num = param_num + 1;
                    current = current->next;
                    i++;
                    sp_size += 4;
                }
            }
            //call
            fprintf(f, "addi $sp, $sp, -%d\n",sp_size);
            int reg_num = getreg(f,current->op2.left.name,1,func,vars,num);
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, -%d\n",4);
            fprintf(f, "jal %s\n",current->op2.right.name);
            fprintf(f, "addi $sp, $sp, %d\n",4);
            fprintf(f, "lw $ra, 0($sp)\n");
            fprintf(f, "move $%d, $v0\n",reg_num);
        }
        else{
            assert(0);
        }
        if(i == block->num -1)
            update_newblock(f);
        current = current->next;
    }
    return;
}
#endif