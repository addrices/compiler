# COMPILER
## Lab1

## Lab2(语义分析)
添加符号表，我将符号表分成3个部分，分别为函数定义表，结构体表，变量表.

均以双向链表的形式保存。

### 函数定义表节点说明
|成员名|作用|
|:---|:---|
|name|函数名|
|parameter_num|参数数量|
|parameters|参数列表数组的头指针|
parameters中按顺序保存着parameter_num个char类型的，a代表int，b代表float，c代表struct。

### 结构体表(不允许嵌套struct定义)
|成员名|作用|
|:---|:---|
|name|函数名|
|member_num|成员数量|
|members|成员列表数组的头指针|
members中按顺序保存着member_num个struct_node类型变量，用于描述成员。

struct_node类型变量
|成员名|作用|
|:---|:---|
|name|函数名|
|type|成员类型|

### 变量表
|成员名|作用|
|:---|:---|
|name|函数名|
|type|成员类型|
|struct_type|指针|
type如果是struct类型的话，struct_type指向对应结构体表中的变量，否则指向0。
