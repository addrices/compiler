%{
    #include "syntax.tab.h"
    #include "m.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}
%option yylineno
DIGIT [0-9]
DIGIT_16 [0-9a-fA-F]
DIGIT_8 [0-7]
LETTER [_a-zA-Z]
INT [1-9]{DIGIT}*|0
INT_16 0x[1-9a-fA-F]{DIGIT_16}*
INT_8 0[1-7]{DIGIT_8}*
FLOAT {DIGIT}+[.]{DIGIT}+|{DIGIT}+[.]|[.]{DIGIT}+
FLOAT_S {FLOAT}e-?{INT}
SPACE [ \t]
SEMI [;]
COMMA [,]
ASSIGNOP [=]
RELOP >|<|>=|<=|==|!=
PLUS [\+]
MINUS [-]
NOT [!]
STAR [\*]
DIV [/]
AND &&
OR \|\|
DOT [.]
TYPE int|float
LP [\(]
RP [\)]
LB [\[]
RB [\]]
LC [\{]
RC [\}]
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
ID {LETTER}{1,32}
%%
{INT}   { yylval.type_int = atoi(yytext); return INT;}
{INT_8} { 
        int num = 0;
        for(int i = 1; yytext[i] != '\0';i++){
            num = num * 8 + yytext[i] - '0';
        }
        yylval.type_int = num;
        return INT;
    }
{INT_16} {
        int num = 0;
        for(int i = 2; yytext[i] != '\0';i++){
            if(yytext[i] >= '0' && yytext[i] <= '9')
                num = num * 16 + yytext[i] - '0';
            else if(yytext[i] >= 'a' && yytext[i] <= 'f')
                num = num * 16 + yytext[i] - 'a' + 10;
            else
                num = num * 16 + yytext[i] - 'A' + 10;
        }
        yylval.type_int = num;
        return INT;
    }
{FLOAT} { 
        int i = 0;
        float num = 0;
        for(; yytext[i] != '.'; i++){
            num = num * 10 + (yytext[i] - '0');
        }
        i++;
        int a = 1;
        float num_f = 0;
        for(; yytext[i] != '\0'; i++,a++){
            num_f = yytext[i] - '0';
            for(int b = 0; b < a; b++){
                num_f /= 10;
            }
            num += num_f;
        }
        yylval.type_float = num;
        return FLOAT;
    }
{FLOAT_S} {
        int i = 0;
        float num = 0;
        for(; yytext[i] != '.'; i++){
            num = num * 10 + (yytext[i] - '0');
        }
        i++;
        int a = 1;
        float num_f = 0;
        for(; yytext[i] != 'e'; i++,a++){
            num_f = yytext[i] - '0';
            for(int b = 0; b < a; b++){
                num_f /= 10;
            }
            num += num_f;
        }
        i++;
        bool minus_flag = false;
        if(yytext[i] == '-'){
            minus_flag = true;
            i++;
        }
        int index = atoi(yytext+i);
        for(int q = 0; q < index; q++){
            if(minus_flag == true)
                num /= 10;
            else
                num *= 10;
        } 
        yylval.type_float = num;
        return FLOAT;
    }
{SEMI}  { return SEMI;}
{RELOP} { 
    if(strcmp(yytext,"<") == 0)
        yylval.type_relop = 1;
    else if(strcmp(yytext,">") == 0)
        yylval.type_relop = 2;
    else if(strcmp(yytext,"==") == 0)
        yylval.type_relop = 3;
    else if(strcmp(yytext,"<=") == 0)
        yylval.type_relop = 4;
    else if(strcmp(yytext,">=") == 0)
        yylval.type_relop = 5;
    else if(strcmp(yytext,"!=") == 0)
        yylval.type_relop = 6;
    return RELOP;
    }
{ASSIGNOP} { return ASSIGNOP;}
{COMMA} { return COMMA;}
{PLUS}   { return PLUS;}
{MINUS} { return MINUS;}
{STAR}  { return STAR;}
{DIV}   { return DIV;}
{DOT} { return DOT;}
{AND} { return AND;}
{NOT} { return NOT;}
{OR}  { return OR;}
{TYPE} {
    yylval.type_id = (char*)malloc(32 * sizeof(char)); 
    strcpy(yylval.type_id,yytext); return TYPE;
    }
{LP} { return LP;}
{RP}  { return RP;}
{LB} { return LB;}
{RB} { return RB;}
{LC} { return LC;}
{RC} { return RC;}
{STRUCT}  { return STRUCT;}
{RETURN} {return RETURN;}
{IF}  { return IF;}
{ELSE}  { return ELSE;}
{WHILE} {return WHILE;}
{ID}  { 
    yylval.type_id = (char*)malloc(32 * sizeof(char)); 
    strcpy(yylval.type_id,yytext);return ID;
    }
{SPACE} {}
\n {yycolumn = 1;}
"//" {
    char c = input();
    while (c != '\n' && c != 0)
        c = input();
}
"/*" {
    char c = input();
    while(c != 0){
        if(c != '*')
            c = input();
        else{
            c = input();
            if(c == '/')
                break;
        }
    }
}
. {
    printf("Error type A at Line %d : Mysterious characters \'%s\'\n",
      yylineno, yytext);
    ProFlag = false;
}
%%
