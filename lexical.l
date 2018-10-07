%{
    int lines = 1;
%}
DIGIT [0-9]
LETTER [_a-zA-Z]
INT {DIGIT}+
FLOAT {DIGIT}+[.]{DIGIT}+
SPACE [ \t]
SEMI [;]
COMMA [,]
ASSIGNOP [=]
RELOP ([><][=]?)|(==)|(!=)
PLUS [\+]
MINUS [-]
STARS [\*]
DIV [/]
AND &&
OR \|\|
NOT [!]
TYPE {INT}|{FLOAT}
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
WHITE while
ID {LETTER}+
%%
{INT}   { printf("INT ");}
{FLOAT} { printf("FLOAT ");}
{SEMI}  { printf("SEMI ");}
{RELOP} { printf("RELOP "); }
{ASSIGNOP}   { printf("ASSIGNOP ");}
{COMMA} { printf("COMMA ");}
{PLUS}   { printf("PLUS ");}
{MINUS} { printf("MINUS ");}
{STARS}  { printf("STARS ");}
{DIV}   { printf("DIV ");}
{AND} { printf("AND ");}
{OR}  { printf("OR ");}
{NOT}   { printf("NOT ");}
{LP} { printf("LP ");}
{RP}  { printf("RP ");}
{LB} { printf("LB ");}
{RB}  { printf("RB ");}
{LC} { printf("LC ");}
{RC} { printf("RC ");}
{STRUCT}  { printf("STRUCT ");}
{RETURN} { printf("RETURN ");}
{IF}  { printf("IF ");}
{ELSE}  { printf("ELSE ");}
{WHITE} { printf("WHITE ");}
{ID}  { printf("ID ");}
{SPACE} {}
\n {lines++;}
. {
    printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
      lines, yytext);
}
%%
