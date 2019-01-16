#include "m.h"

#ifdef  __LAB4_ENABLE

graph_node* search_gvar(char *var_name, graph_node* vars, int node_num){
    for(int i = 0; i < node_num; i++){
        if(strcmp(var_name,vars[i].name) == 0)
            return &vars[i];
    }
    printf("error var %s\n",var_name);
    assert(0);
}

//获得具体的reg，如果没有会添加load指令进去。type = 1是指会被赋值，type = 2是只是取一次数
int getreg(FILE* f, char* var_name, int type, func_info* func,graph_node* vars,int node_num){
    if(var_name[0] == '&'){
        graph_node* g_var = search_gvar(&var_name[1], vars, node_num);
        int reg_num = g_var->reg_num;
        int sp_shift = g_var->var->sp_shift;
        if(regs_info[reg_num-8].valid == true){    //对应的寄存器中没有被存
            int sp_shift1 = regs_info[reg_num-8].var->var->sp_shift;
            fprintf(f, "sw $%d, %d($sp)\n", reg_num, sp_shift1);//存回内存中去
            regs_info[reg_num-8].var->if_new = true;
        }
        regs_info[reg_num-8].var->if_reg = false;
        regs_info[reg_num-8].valid = false;
        fprintf(f,"addi $%d, $sp, %d\n",reg_num,g_var->var->sp_shift);
        return g_var->reg_num;
    }
    else if(var_name[0] == '*'){
        if(type == 1)
            assert(0);
        else{
            graph_node* g_var = search_gvar(&var_name[1], vars, node_num);
            int reg_num = g_var->reg_num;
            int sp_shift = g_var->var->sp_shift;
            if(regs_info[reg_num-8].valid == true){    //对应的寄存器中没有被存
                int sp_shift1 = regs_info[reg_num-8].var->var->sp_shift;
                fprintf(f, "sw $%d, %d($sp)\n", reg_num, sp_shift1);//存回内存中去
                regs_info[reg_num-8].var->if_new = true;
            }
            regs_info[reg_num-8].var->if_reg = false;
            fprintf(f, "lw $%d, %d($sp)\n",reg_num,g_var->var->sp_shift);
            fprintf(f, "lw $%d, 0($%d)\n",reg_num,reg_num);
            return g_var->reg_num;
        }
    }
    else if(var_name[0] == '#'){
        fprintf(f, "li $25, %s\n", &var_name[1]);
        return 25;
    }
    graph_node* g_var = search_gvar(var_name, vars, node_num);
    if(g_var->if_reg == true){          //已经在reg中
        if(type == 2){
            return g_var->reg_num;
        }
        else{
            g_var->if_new = false;
            return g_var->reg_num;
        }
    }  
    else{                               //不在reg中
        int reg_num = g_var->reg_num;
        if(type == 2){                  //不被修改
            if(regs_info[reg_num-8].valid == false){    //对应的寄存器中没有被存
                int sp_shift = g_var->var->sp_shift;
                fprintf(f, "lw $%d, %d($sp)\n", reg_num ,sp_shift);      //将栈中的数据装载进去
                regs_info[reg_num-8].var = g_var;
                regs_info[reg_num-8].valid = true;
                g_var->if_new = true;
                g_var->if_reg = true;
                return reg_num;
            }
            else{                                       //对应的寄存器中已经有了别的变量
                int sp_shift1 = regs_info[reg_num-8].var->var->sp_shift;
                int sp_shift2 = g_var->var->sp_shift;
                if(regs_info[reg_num-8].var->if_new == false){     //检查发现其在内存中的值不是最新的
                    fprintf(f, "sw $%d, %d($sp)\n", reg_num, regs_info[reg_num-8].var->var->sp_shift);//存回内存中去
                    regs_info[reg_num-8].var->if_new = true;
                }
                regs_info[reg_num-8].var->if_reg = false;
                fprintf(f, "lw $%d, %d($sp)\n", reg_num, g_var->var->sp_shift);
                regs_info[reg_num-8].var = g_var;
                regs_info[reg_num-8].valid = true;
                g_var->if_new = true;
                g_var->if_reg = true;
                return reg_num;
            }
        }
        else{                           //被修改
            if(regs_info[reg_num-8].valid == false){
                int sp_shift = g_var->var->sp_shift;
                fprintf(f, "lw $%d, %d($sp)\n", reg_num ,sp_shift);      //将栈中的数据装载进去
                regs_info[reg_num-8].var = g_var;
                regs_info[reg_num-8].valid = true;
                g_var->if_new = false;
                g_var->if_reg = true;
                return reg_num;
            }
            else{
                int sp_shift1 = regs_info[reg_num-8].var->var->sp_shift;
                int sp_shift2 = g_var->var->sp_shift;
                if(regs_info[reg_num-8].var->if_new == false){
                    fprintf(f, "sw $%d, %d($sp)\n", reg_num, regs_info[reg_num-8].var->var->sp_shift);//存回内存中去
                    regs_info[reg_num-8].var->if_new = true;
                }
                regs_info[reg_num-8].var->if_reg = false;
                fprintf(f, "lw $%d, %d($sp)\n", reg_num, g_var->var->sp_shift);
                //为了保证$1 = $1 + $2这样的语句的正确性，还是将其的值装入一下。
                regs_info[reg_num-8].var = g_var;
                regs_info[reg_num-8].valid = true;
                g_var->if_new = false;
                g_var->if_reg = true;
                return reg_num;
            }
        }
    }
}

//当遇到一个新的block的时候将内存中不新的变量更新一哈,且将上一个块的内存变量放回内存中。
void update_newblock(FILE* f){
    for(int i = 0 ; i < 17; i++){
        if(regs_info[i].valid == true && regs_info[i].var->if_new == false){      //内存中的值对不上
            fprintf(f, "sw $%d, %d($sp)\n", i + 8, regs_info[i].var->var->sp_shift);//存回内存中去
            regs_info[i].var->if_new = true;
        }
    }
}
#endif