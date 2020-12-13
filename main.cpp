#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define MAX 255
char name[20][20] = {"const", "int", "char",
                     "void", "main",
                     "if", "else",
                     "switch", "case", "default",
                     "while", "for",
                     "scanf", "printf",
                     "return"};
char reserved_word[20][20] = {"CONSTTK", "INTTK", "CHARTK",
                     "VOIDTK", "MAINTK",
                     "IFTK", "ELSETK",
                     "SWITCHTK", "CASETK", "DEFAULTTK",
                     "WHILETK", "FORTK",
                     "SCANFTK", "PRINTFTK",
                     "RETURNTK"};
char head[100][MAX];    // head_statement

typedef struct node
{
    char word[MAX];
    char type[20];
    int line;   //  行号
}   user;

typedef struct node2
{
    int line;   //  行号
    char type;  //  错误类型
}   user2;

typedef struct node3
{
    char word[MAX]; //   常量 or 变量名
    int type;   // 常量 or 变量类型编号
    int func;   //  表示符号在何处定义
    int dim;    //  值的维数
    int i, j;   //  存储数组的下标
    int value[30];  //    常量 or 变量值
}   user3;

typedef struct func
{
    char name[MAX]; //  声明的函数名
    int type;   //  函数的返回类型
    int num;    //  参数个数
    char id[MAX];   //  参数类型标记
    int loc;    //  函数开始位置
}   user4;

user store[10000];
user2 err_store[100];
int addr = 0;   //  结构体长度
int now = 0;    //  目前单词下标
int headnum = 0;    //  有返回值函数名个数(函数名数组长度)
int errnum = 0; //  返回错误个数(错误存储数组长度)
char err_type;  //  错误类型

int global_flag = 1;    //  全局指示符
user3 global_tab[100];  //  全局符号表
user3 local_tab[100];   //  局部符号表
int gt_num = 0, lt_num = 0; //  记录全局表和局部表的长度
int temp_val = 0;   //  暂存常量或变量值
char temp_str[MAX]; //  暂存字符串

user4 func_tab[100];    //  函数表
int func_num = 0;   //  声明的函数总个数
int func_label = -1;    //  存储对应下标
char now_type = ' ';    //  存储当前实参类型

int void_ret = 0;   //  进入无返回值函数定义标识符
int have_ret = 0;   //  进入有返回值函数定义标识符
int ret_valid = 0;  //  检测是否存在返回语句标识符
int ret_value = 0;  //  存储返回值

int switch_type = 0;    //  存储switch判断的常量类型

int run = 1;    //  表示语句是否执行
int def = 1;    //  表示是否在函数定义阶段

FILE *fp_in = NULL;
FILE *fp_out = NULL;
FILE *fp_err = NULL;
FILE *fp_result = NULL;

int isletter(char letter)//判断是否为字符
{
    if((letter >= 65 && letter <= 90)
    || (letter >= 97 && letter <= 122)
    || letter == 95)
        return 1;
    else
        return 0;
}

void getsym()   //取词
{
    fprintf(fp_out,"%s %s\n", store[now].type, store[now].word);
    now++;
}

int* get_index(char name[]) //  获取在标识符表中的下标以及在哪个表中(局部1 全局2)
//返回长度为2的数组array：array[0]为下标，array[1]为表号
{
    int i = 0, j = 0;
    int *array;
    char sym[MAX] = {};
    char tab[MAX] = {};
    array = (int *)malloc(2*sizeof(int));
    for(i = 0; i < strlen(name); i++) {
        sym[i] = tolower(name[i]);
    }
    for(i = 0; i < lt_num; i++) // 1st: 检索局部表
    {
        for(j = 0; j < strlen(local_tab[i].word); j++) {
            tab[j] = tolower(local_tab[i].word[j]);
        }
        if(strcmp(sym, tab) == 0)
        {
            array[0] = i;
            array[1] = 1;
            return array;
        }
        memset(tab, 0, sizeof(tab));
    }
    for(i = 0; i < gt_num; i++) //  2nd: 检索全局表
    {
        for(j = 0; j < strlen(global_tab[i].word); j++) {
            tab[j] = tolower(global_tab[i].word[j]);
        }
        if(strcmp(sym,tab) == 0)
        {
            array[0] = i;
            array[1] = 2;
            return array;
        }
        memset(tab, 0, sizeof(tab));
    }
}

char search_nametab()   //  查全局 or 局部表返回数据类型
{
    int i = 0, j = 0;
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i = 0; i < strlen(store[now].word); i++) {
        sym[i] = tolower(store[now].word[i]);
    }
    for(i = 0; i < lt_num; i++) // 1st: 检索局部表
    {
        for(j = 0; j < strlen(local_tab[i].word); j++) {
            tab[j] = tolower(local_tab[i].word[j]);
        }
        if(strcmp(sym,tab) == 0)
        {
            if(local_tab[i].type == 1 || local_tab[i].type == 3) {
                return 'i';
            }
            else if(local_tab[i].type == 2 || local_tab[i].type == 4) {
                return 'c';
            }
        }
        memset(tab, 0, sizeof(tab));
    }
    for(i = 0; i<gt_num; i++)   // 2nd: 检索全局表
    {
        for(j = 0; j < strlen(global_tab[i].word); j++) {
            tab[j] = tolower(global_tab[i].word[j]);
        }
        if(strcmp(sym,tab) == 0)
        {
            if(global_tab[i].type == 1 || global_tab[i].type == 3) {
                return 'i';
            }
            else if(global_tab[i].type == 2 || global_tab[i].type == 4) {
                return 'c';
            }
        }
        memset(tab, 0, sizeof(tab));
    }
}

char search_const() //  查const
{
    int i = 0, j = 0;
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i = 0; i < strlen(store[now].word); i++) {
        sym[i] = tolower(store[now].word[i]);
    }
    for(i = 0; i < lt_num; i++)
    {
        for(j = 0; j < strlen(local_tab[i].word); j++) {
            tab[j] = tolower(local_tab[i].word[j]);
        }
        if(strcmp(sym,tab) == 0)
        {
            if(local_tab[i].type == 3 || local_tab[i].type == 4) {
                return 'x'; //  const类型
            }
        }
        memset(tab, 0, sizeof(tab));
    }
    for(i = 0; i < gt_num; i++)
    {
        for(j = 0; j < strlen(global_tab[i].word); j++) {
            tab[j] = tolower(global_tab[i].word[j]);
        }
        if(strcmp(sym,tab) == 0)
        {
            if(global_tab[i].type == 3 || global_tab[i].type == 4) {
                return 'x'; //  const类型
            }
        }
        memset(tab, 0, sizeof(tab));
    }
    return 'n';
}

void error_report() //  错误报告
{
    printf("error : %s at line: %d\n",store[now].word,store[now].line);
}
void error_a()  //  错误处理a
{
    err_store[errnum].line = store[addr].line;
    err_store[errnum].type = 'a';
    errnum++;
}
void error_b(int type)  //  错误处理b
{
    int i = 0, j = 0;
    int flag = 0;//判断是否重复定义标识符
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i=0; i<strlen(store[now].word); i++)
        sym[i] = tolower(store[now].word[i]);
    if(global_flag == 1)//全局变量判断
    {
        for(i=0; i<gt_num; i++)
        {
            for(j=0; j<strlen(global_tab[i].word); j++)
                tab[j] = tolower(global_tab[i].word[j]);
            if(strcmp(sym,tab) == 0)
            {
                flag = 1;
                break;
            }
            memset(tab, 0, sizeof(tab));
        }
    }
    else if(global_flag == 0)//局部变量判断
    {
        for(i=0; i<lt_num; i++)
        {
            for(j=0; j<strlen(local_tab[i].word); j++)
                tab[j] = tolower(local_tab[i].word[j]);
            if(strcmp(sym,tab) == 0)
            {
                flag = 1;
                break;
            }
            memset(tab, 0, sizeof(tab));
        }
    }
    if(flag == 1)
    {
        err_store[errnum].line = store[now].line;
        err_store[errnum].type = 'b';
        errnum++;
    }
    else
    {
        if(global_flag == 1)
        {
            strcpy(global_tab[gt_num].word,store[now].word);
            global_tab[gt_num].type = type;
            //gt_num++;
        }
        else
        {
            strcpy(local_tab[lt_num].word,store[now].word);
            local_tab[lt_num].type = type;
            //lt_num++;
        }
    }
}
void error_c(int diff)  //  错误处理c
{
    int i = 0, j = 0;
    int flag = 0;//判断是否声明
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i=0; i<strlen(store[now].word); i++)
        sym[i] = tolower(store[now].word[i]);
    if(diff == 1)//函数名
    {
        for(i=0; i<gt_num; i++)//遍历全局栈中的函数名
        {
            for(j=0; j<strlen(global_tab[i].word); j++)
                tab[j] = tolower(global_tab[i].word[j]);
            if(strcmp(sym,tab) == 0 && (global_tab[i].type>=5 && global_tab[i].type<=7))
            {
                flag = 1;
                break;
            }
            memset(tab, 0, sizeof(tab));
        }
    }
    else if(diff == 2)//常量\变量名
    {
        for(i=0; i<gt_num; i++)//遍历全局栈中的常量\变量名
        {
            for(j=0; j<strlen(global_tab[i].word); j++)
                tab[j] = tolower(global_tab[i].word[j]);
            if(strcmp(sym,tab) == 0 && (global_tab[i].type>=1 && global_tab[i].type<=4))
            {
                flag = 1;
                break;
            }
            memset(tab, 0, sizeof(tab));
        }
        for(i=0; i<lt_num && flag!=1; i++)//遍历局部栈中的常量\变量名
        {
            for(j=0; j<strlen(local_tab[i].word); j++)
                tab[j] = tolower(local_tab[i].word[j]);
            if(strcmp(sym,tab) == 0)
            {
                flag = 1;
                break;
            }
            memset(tab, 0, sizeof(tab));
        }
    }
    if(flag == 0)
    {
        err_store[errnum].line = store[now].line;
        err_store[errnum].type = 'c';
        errnum++;
    }
}
void error_d()  //  错误处理d
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'd';
    errnum++;
}
void error_e()  //  错误处理e
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'e';
    errnum++;
}
void error_f()  //  错误处理f
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'f';
    errnum++;
}
void error_g()  //  错误处理g
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'g';
    errnum++;
}
void error_h()  //  错误处理h
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'h';
    errnum++;
}
void error_i()  //  错误处理i
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'i';
    errnum++;
}
void error_j()  //  错误处理j
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'j';
    errnum++;
}
void error_k()  //  错误处理k
{
    err_store[errnum].line = store[now-1].line;
    err_store[errnum].type = 'k';
    errnum++;
}
void error_l()  //  错误处理l
{
    err_store[errnum].line = store[now-1].line;
    err_store[errnum].type = 'l';
    errnum++;
}
void error_m()  //  错误处理m
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'm';
    errnum++;
}
void error_n()  //  错误处理n
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'n';
    errnum++;
}
void error_o()  //  错误处理o
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'o';
    errnum++;
}
void error_p()  //  错误处理p
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'p';
    errnum++;
}

void addition_operator();   //  加法运算符
void multiplication_operator(); //  乘法运算符
void relational_operators();    //  关系运算符
void letter();  //  字母
void number();  //  数字
void character();   //  字符
void string();  //  字符串
void program(); //  程序
void constant_description();    //  常量说明
void constant_definition(); //  常量定义
void unsigned_integer();  //  无符号整数
void integer(); //  整数
void identifier();  //  标识符
void declaration_head();    //  声明头部
void constant(int type);    //  常量
void variable_description();    //  变量说明
void variable_definition(); //  变量定义
void variable_definition_has_no_initialization();   //  变量定义无初始化
void variable_definition_and_initialization();  //  变量定义及初始化
void type_identifier(); //  类型标识符
void function_definition_with_return_value(int value[]);    //  有返回值函数定义
void function_definition_without_return_value(int value[]); //  无返回值函数定义
void compound_statement();  //  复合语句
void parameter_table(int value[]);  //  参数表
void principal_function();  //  主函数
void expression();  //  表达式
void term();    //  项
void factor();  //  因子
void sentence();    //  语句
void assignment_statement();    //  赋值语句
void conditional_statement();   //  条件语句
int condition();    //  条件
void loop_statement();  //  循环语句
void step();    //  步长
void case_statement();  //  情况语句
int situation_table(int aim);   //  情况表
int case_sub_statement(int aim);    //  情况子语句
void default_sentence(int sign);    //  缺省
void function_call_statement_with_return_value();   //  有返回值函数调用语句
void function_call_statement_without_return_value();    //  无返回值函数调用语句
void value_parameter_table(int value[]);    //  值参数表
void statement_column();    //  语句列
void read_statement();  //  读语句
void writing_statements();  //  写语句
void return_statement();    //  返回语句

void fopen_initialize_debug() {
    fp_in = fopen("../testfile.txt", "r");
    fp_out = fopen("../output.txt", "w");
    fp_err = fopen("../error.txt", "w");
    fp_result = fopen("../pcoderesult.txt", "w");
}

void fopen_initialize() {
    fp_in = fopen("testfile.txt", "r");
    fp_out = fopen("output.txt", "w");
    fp_err = fopen("error.txt", "w");
    fp_result = fopen("pcoderesult.txt", "w");
}
int main()
{
    int i = 0, j = 0, k = 0;
    int pin = 0, len = 0, flag = 0;
    int line = 1;
    int tempi = 0;
    char tempc = ' ';
    char token = ' ';   // 当前字母
    char buff[MAX] = "";
    char temp[MAX] = "", lower[MAX] = "";
//    fopen_initialize_debug();
    fopen_initialize();
    while(fgets(buff, MAX, fp_in) != NULL)
    {
        len = strlen(buff);
        for(i = 0; i < len; i++, pin = 0, memset(temp, 0, sizeof(temp)))
        {
            token = buff[i];
            if(isspace(token) || token == '\0')
            {
                if(token == '\n' || token == '\0') {
                    break;
                }
                else {
                    continue;
                }
            }
            else if(isletter(token))
            {
                temp[pin++] = token;
                for(i = i + 1; i < len; i++)
                {
                    token = buff[i];
                    if(isletter(token) || isdigit(token)) {
                        temp[pin++] = token;
                    }
                    else {
                        break;
                    }
                }
                i--;
                for(j = 0; j < strlen(temp); j++) {
                    lower[j] = tolower(temp[j]);
                }
                for(j = 0; j < 15; j++)
                {
                    if(strcmp(lower, name[j]) == 0)
                    {
                        flag = 1;
                        strcpy(store[addr].word, temp);
                        strcpy(store[addr].type, reserved_word[j]);
                        store[addr].line = line;
                        addr++;
                        break;
                    }
                }
                if(flag == 0)
                {
                    strcpy(store[addr].word, temp);
                    strcpy(store[addr].type,"IDENFR");
                    store[addr].line = line;
                    addr++;
                }
                flag = 0;
                memset(lower, 0, sizeof(lower));
            }
            else if(isdigit(token))
            {
                temp[pin++] = token;
                for(i = i+1; i < len; i++)
                {
                    token = buff[i];
                    if(isdigit(token)) {
                        temp[pin++] = token;
                    }
                    else {
                        break;
                    }
                }
                i--;
                strcpy(store[addr].word, temp);
                strcpy(store[addr].type,"INTCON");
                store[addr].line = line;
                addr++;
            }
            else if(token == '\'') {
                i++;
                token = buff[i];
                while(token != '\'') {
                    if((buff[i] < 48 || buff[i] > 57)
                    && (buff[i] < 65 || buff[i] > 90)
                    && (buff[i] < 97 || buff[i] > 122)
                    &&  buff[i] != 42 && buff[i] != 43 && buff[i] != 45 && buff[i] != 47
                    && buff[i] != 95) {
                        flag=1;
                    }
                    temp[pin++] = buff[i++];
                    token = buff[i];
                }
                strcpy(store[addr].word, temp);
                strcpy(store[addr].type,"CHARCON");
                store[addr].line = line;
                if(flag == 1) {
                    error_a();
                }
                addr++;
                flag = 0;
            }
            else if(token == '\"'){
                i++;
                token = buff[i];
                if(token == '\"') flag = 1;
                while(token != '\"'){
                    if((buff[i] < 35 || buff[i] > 126)
                    && buff[i] != 33 && buff[i] != 32) {
                        flag=1;
                    }
                    temp[pin++] = buff[i++];
                    token = buff[i];
                }
                strcpy(store[addr].word, temp);
                strcpy(store[addr].type,"STRCON");
                store[addr].line = line;
                if(flag == 1) {
                    error_a();
                }
                addr++;
                flag = 0;
            }
            else if(token == '+')
            {
                strcpy(store[addr].word,"+");
                strcpy(store[addr].type,"PLUS");
                store[addr].line = line;
                addr++;
            }
            else if(token == '-')
            {
                strcpy(store[addr].word,"-");
                strcpy(store[addr].type,"MINU");
                store[addr].line = line;
                addr++;
            }
            else if(token == '*')
            {
                strcpy(store[addr].word,"*");
                strcpy(store[addr].type,"MULT");
                store[addr].line = line;
                addr++;
            }
            else if(token == '/')
            {
                strcpy(store[addr].word,"/");
                strcpy(store[addr].type,"DIV");
                store[addr].line = line;
                addr++;
            }
            else if(token == ':')
            {
                strcpy(store[addr].word,":");
                strcpy(store[addr].type,"COLON");
                store[addr].line = line;
                addr++;
            }
            else if(token == ';')
            {
                strcpy(store[addr].word,";");
                strcpy(store[addr].type,"SEMICN");
                store[addr].line = line;
                addr++;
            }
            else if(token == ',')
            {
                strcpy(store[addr].word,",");
                strcpy(store[addr].type,"COMMA");
                store[addr].line = line;
                addr++;
            }
            else if(token == '(')
            {
                strcpy(store[addr].word,"(");
                strcpy(store[addr].type,"LPARENT");
                store[addr].line = line;
                addr++;
            }
            else if(token == ')')
            {
                strcpy(store[addr].word,")");
                strcpy(store[addr].type,"RPARENT");
                store[addr].line = line;
                addr++;
            }
            else if(token =='[')
            {
                strcpy(store[addr].word,"[");
                strcpy(store[addr].type,"LBRACK");
                store[addr].line = line;
                addr++;
            }
            else if(token == ']')
            {
                strcpy(store[addr].word,"]");
                strcpy(store[addr].type,"RBRACK");
                store[addr].line = line;
                addr++;
            }
            else if(token == '{')
            {
                strcpy(store[addr].word,"{");
                strcpy(store[addr].type,"LBRACE");
                store[addr].line = line;
                addr++;
            }
            else if(token == '}')
            {
                strcpy(store[addr].word,"}");
                strcpy(store[addr].type,"RBRACE");
                store[addr].line = line;
                addr++;
            }
            else if(token == '<'){
                temp[pin++] = token;
                i++;
                token = buff[i];
                if(token == '=')
                {
                    temp[pin++] = token;
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"LEQ");
                    store[addr].line = line;
                    addr++;
                }
                else
                {
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"LSS");
                    store[addr].line = line;
                    addr++;
                    i--;
                }
            }
            else if(token == '>'){
                temp[pin++] = token;
                i++;
                token = buff[i];
                if(token == '=')
                {
                    temp[pin++] = token;
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"GEQ");
                    store[addr].line = line;
                    addr++;
                }
                else
                {
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"GRE");
                    store[addr].line = line;
                    addr++;
                    i--;
                }
            }
            else if(token == '='){
                temp[pin++] = token;
                i++;
                token = buff[i];
                if(token == '=')
                {
                    temp[pin++] = token;
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"EQL");
                    store[addr].line = line;
                    addr++;
                }
                else
                {
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"ASSIGN");
                    store[addr].line = line;
                    addr++;
                    i--;
                }
            }
            else if(token == '!'){
                temp[pin++] = token;
                i++;
                token = buff[i];
                if(token == '=')
                {
                    temp[pin++] = token;
                    strcpy(store[addr].word,temp);
                    strcpy(store[addr].type,"NEQ");
                    store[addr].line = line;
                    addr++;
                }
                else
                {
                    printf("ERROR1:WRONG INPUT IN LINE %d\n", line);
                    i--;
                }
            }
            else
                printf("ERROR2:WRONG INPUT IN LINE %d\n", line);
        }
        line++;
        memset(buff,0,sizeof(buff));
    }

    program();

    for(i = 0; i < errnum - 1; i++) //  错误输出排序
    {
        for(j = 0; j < errnum - 1 - i; j++)
        {
            if(err_store[j + 1].line < err_store[j].line)
            {
                tempi = err_store[j + 1].line;
                err_store[j + 1].line = err_store[j].line;
                err_store[j].line = tempi;
                tempc = err_store[j + 1].type;
                err_store[j + 1].type = err_store[j].type;
                err_store[j].type = tempc;
            }
        }
    }
    for(i = 0; i < errnum; i++) {
        fprintf(fp_err, "%d %c\n", err_store[i].line, err_store[i].type);
    }
    for(i = 0; i < gt_num; i++) // 全局表测试
    {
        if(global_tab[i].dim == 0)
        {
            if(global_tab[i].type == 1 || global_tab[i].type == 3) {
                printf("%s %d %d %d\n", global_tab[i].word, global_tab[i].type, global_tab[i].func,
                       global_tab[i].value[0]);
            }
            else if(global_tab[i].type == 2 || global_tab[i].type == 4) {
                printf("%s %d %d %c\n", global_tab[i].word, global_tab[i].type, global_tab[i].func,
                       global_tab[i].value[0]);
            }
            else {
                printf("%s %d %d\n", global_tab[i].word, global_tab[i].type, global_tab[i].func);
            }
        }
        else if(global_tab[i].dim == 1)
        {
            printf("%s[%d] %d\n", global_tab[i].word, global_tab[i].i, global_tab[i].type);
            printf("\t");
            for(j = 0; j < global_tab[i].i; j++)
            {
                if(global_tab[i].type == 1 || global_tab[i].type == 3) {
                    printf("%d ", global_tab[i].value[j]);
                }
                else if(global_tab[i].type == 2 || global_tab[i].type == 4) {
                    printf("%c ", global_tab[i].value[j]);
                }
            }
            printf("\n");
        }
        else if(global_tab[i].dim == 2)
        {
            printf("%s[%d][%d] %d\n", global_tab[i].word, global_tab[i].i, global_tab[i].j, global_tab[i].type);
            for(j = 0; j < global_tab[i].i; j++)
            {
                printf("\t");
                for(k = 0; k < global_tab[i].j; k++)
                {
                    if(global_tab[i].type == 1 || global_tab[i].type == 3) {
                        printf("%d ", global_tab[i].value[j * global_tab[i].j + k]);
                    }
                    else if(global_tab[i].type == 2 || global_tab[i].type == 4) {
                        printf("%c ", global_tab[i].value[j * global_tab[i].j + k]);
                    }
                }
                printf("\n");
            }
        }
    }
    for(i = 0; i < lt_num; i++)//局部表测试
    {
        if(local_tab[i].dim == 0)
        {
            if(local_tab[i].type == 1|| local_tab[i].type == 3) {
                printf("%s %d %d %d\n", local_tab[i].word, local_tab[i].type, local_tab[i].func, local_tab[i].value[0]);
            }
            else if(local_tab[i].type == 2 || local_tab[i].type == 4) {
                printf("%s %d %d %c\n", local_tab[i].word, local_tab[i].type, local_tab[i].func, local_tab[i].value[0]);
            }
            else {
                printf("%s %d %d\n", local_tab[i].word, local_tab[i].type, local_tab[i].func);
            }
        }
        else if(local_tab[i].dim == 1)
        {
            printf("%s[%d] %d\n", local_tab[i].word, local_tab[i].i, local_tab[i].type);
            printf("\t");
            for(j = 0; j < local_tab[i].i; j++)
            {
                if(local_tab[i].type == 1 || local_tab[i].type == 3) {
                    printf("%d ", local_tab[i].value[j]);
                }
                else if(local_tab[i].type == 2 || local_tab[i].type == 4) {
                    printf("%c ", local_tab[i].value[j]);
                }
            }
            printf("\n");
        }
        else if(local_tab[i].dim == 2)
        {
            printf("%s[%d][%d] %d\n",local_tab[i].word, local_tab[i].i, local_tab[i].j, local_tab[i].type);
            for(j = 0; j < local_tab[i].i; j++)
            {
                printf("\t");
                for(k = 0; k < local_tab[i].j; k++)
                {
                    if(local_tab[i].type == 1 || local_tab[i].type == 3) {
                        printf("%d ", local_tab[i].value[j * local_tab[i].j + k]);
                    }
                    else if(local_tab[i].type == 2 || local_tab[i].type == 4) {
                        printf("%c ", local_tab[i].value[j * local_tab[i].j + k]);
                    }
                }
                printf("\n");
            }
        }
    }

    fclose(fp_in);
    fclose(fp_out);
    fclose(fp_err);
    fclose(fp_result);
    return 0;
}
int isReservedWord(int addr, char s[]) {
    if (strcmp(store[now + addr].type, s) == 0) {
        return 1;
    }
    return 0;
}
void program()  //  <程序>::=［<常量说明>］［<变量说明>］{<有返回值函数定义>|<无返回值函数定义>}<主函数>
{
    int *p = NULL;
    if(isReservedWord(0, "CONSTTK"))
    {
        constant_description(); // 常量说明
    }
    if((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")) && !isReservedWord(2, "LPARENT"))
    {
        variable_description(); // 变量说明
    }
    run = 0;
    while((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")) || (isReservedWord(0, "VOIDTK")
        && isReservedWord(1, "IDENFR")))
    {
        if((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")))
        {
            function_definition_with_return_value(p);   // 有返回值函数定义
        }
        else if((isReservedWord(0, "VOIDTK") && isReservedWord(1, "IDENFR")))
        {
            function_definition_without_return_value(p);    // 无返回值函数定义
        }
    }
    run = 1;
    def = 0;
    if ((isReservedWord(0, "VOIDTK") && isReservedWord(1, "MAINTK"))) {
        principal_function();   // 主函数
    }
    fprintf(fp_out,"<程序>\n");
}
void principal_function()   //  ＜主函数＞::=void main'('')''{'＜复合语句＞'}'
{
    global_flag = 0;
    void_ret = 1;
    if(isReservedWord(0, "VOIDTK"))
    {
        getsym();
        if(isReservedWord(0, "MAINTK"))
        {
            getsym();
            if(!isReservedWord(0, "LPARENT")) {
                error_report();
            }
            else
            {
                getsym();
                if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  错误l
                }
                else {
                    getsym();
                }
                if(!isReservedWord(0, "LBRACE")) {
                    error_report();
                }
                else
                {
                    getsym();
                    compound_statement();   //  复合语句
                    if(!isReservedWord(0, "RBRACE")) {
                        error_report();
                    }
                    else {
                        getsym();
                    }
                }
            }
        }
    }
    void_ret = 0;
    fprintf(fp_out,"<主函数>\n");
}
void function_definition_with_return_value(int value[])//<有返回值函数定义>::=<声明头部>'('<参数表>')''{'<复合语句>'}'
{
    int i = 0;
    have_ret = 1;
    user3 temp_tab[100];    //  暂存符号表
    int temp_num = 0;
    if(def == 1)
    {
        declaration_head(); //  声明头部
        global_flag = 0;
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            func_tab[func_num].loc = now;   //  记录位置
            parameter_table(0); //  参数表
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
            if(!isReservedWord(0, "LBRACE")) {
                error_report();
            }
            else
            {
                getsym();
                compound_statement();   //  复合语句
                if(!isReservedWord(0, "RBRACE")) {
                    error_report();
                }
                else
                {
                    if(ret_valid == 0) {
                        error_h();  //  错误h - 缺少return语句
                    }
                    getsym();
                }
            }
        }
        memset(&local_tab, 0, sizeof(local_tab));
        lt_num = 0;
        global_flag = 1;
        func_num++;
        ret_valid = 0;
        have_ret = 0;
    }
    else if(def == 0 && ret_valid == 0)
    {
        for(i=0; i<lt_num; i++) {
            temp_tab[i] = local_tab[i]; //  暂存局部表
        }
        temp_num = lt_num;
        memset(&local_tab, 0, sizeof(local_tab));   //  清空局部表
        lt_num = 0;
        parameter_table(value); //  参数表
        getsym();
        getsym();
        compound_statement();   //  复合语句
        getsym();
        memset(&local_tab, 0, sizeof(local_tab));//清空局部表
        for(i = 0; i < temp_num; i++) {
            local_tab[i] = temp_tab[i]; //  还原局部表
        }
        lt_num = temp_num;
    }
    fprintf(fp_out,"<有返回值函数定义>\n");
}
void function_definition_without_return_value(int value[])//<无返回值函数定义>::=void＜标识符>'('<参数表>')''{'<复合语句>'}'
{
    int i = 0;
    char lower[MAX] = {};
    void_ret = 1;
    user3 temp_tab[100];    //  暂存符号表
    int temp_num = 0;
    int *p = NULL;
    if(def == 1)
    {
        if(isReservedWord(0, "VOIDTK"))
        {
            getsym();
            if(isReservedWord(0, "IDENFR"))
            {
                for(i = 0; i < strlen(store[now].word); i++) {
                    lower[i] = tolower(store[now].word[i]);
                }
                strcpy(func_tab[func_num].name, lower);  //  存函数名
                func_tab[func_num].type = 7;    //  存函数返回类型
                error_b(7); //  错误b分析
                global_tab[gt_num].func = 1;
                gt_num++;
                global_flag = 0;
                getsym();
                if(!isReservedWord(0, "LPARENT")) {
                    error_report();
                }
                else
                {
                    getsym();
                    func_tab[func_num].loc = now;   //  记录位置
                    parameter_table(p); //  参数表
                    if(!isReservedWord(0, "RPARENT")) {
                        error_l();  //  错误l
                    }
                    else {
                        getsym();
                    }
                    if(!isReservedWord(0, "LBRACE")) {
                        error_report();
                    }
                    else
                    {
                        getsym();
                        compound_statement();   //  复合语句
                        if(!isReservedWord(0, "RBRACE")) {
                            error_report();
                        }
                        else {
                            getsym();
                        }
                    }
                }
            }
        }
        memset(&local_tab, 0, sizeof(local_tab));
        lt_num = 0;
        global_flag = 1;
        func_num++;
        ret_valid = 0;
        void_ret = 0;
        fprintf(fp_out,"<无返回值函数定义>\n");
    }
    else if (def == 0 && ret_valid == 0)
    {
        for(i = 0; i < lt_num; i++) {
            temp_tab[i] = local_tab[i]; //  暂存局部表
        }
        temp_num = lt_num;
        memset(&local_tab, 0, sizeof(local_tab));   //  清空局部表
        lt_num = 0;
        parameter_table(value); //  参数表
        getsym();
        getsym();
        compound_statement();   //  复合语句
        getsym();
        memset(&local_tab, 0, sizeof(local_tab));   //  清空局部表
        for(i = 0; i < temp_num; i++) {
            local_tab[i] = temp_tab[i]; //  还原局部表
        }
        lt_num = temp_num;
    }
}
void compound_statement()   //  <复合语句>::=［<常量说明>］［<变量说明>］<语句列>
{
    if(isReservedWord(0, "CONSTTK"))
    {
        constant_description(); //  常量说明
    }
    if((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK"))
        && !isReservedWord(0, "LPARENT"))
    {
        variable_description(); //  变量说明
    }
    statement_column(); //  语句列
    fprintf(fp_out,"<复合语句>\n");
}
void statement_column() //  ＜语句列＞::=｛＜语句＞｝
{
    while(!isReservedWord(0, "RBRACE")) {
        sentence(); //  语句
    }
    fprintf(fp_out,"<语句列>\n");
}
void sentence() //  ＜语句＞::=＜循环语句＞
                        // ｜＜条件语句＞
                        // |＜有返回值函数调用语句＞;|＜无返回值函数调用语句＞;
                        // ｜＜赋值语句＞;
                        // ｜＜读语句＞;｜＜写语句＞;
                        // ｜＜情况语句＞
                        // ｜＜空＞;
                        // |＜返回语句＞;
                        // |'{'＜语句列＞'}'
{
    int i = 0, j = 0, sign = 0;
    char cmp[MAX] = {};
    if(isReservedWord(0, "WHILETK") || isReservedWord(0, "FORTK")) {
        loop_statement();   //  循环语句
    }
    else if(isReservedWord(0, "IFTK")) {
        conditional_statement();    //  条件语句
    }
    else if(isReservedWord(0, "IDENFR") && isReservedWord(1, "LPARENT"))
    {
        for(i = 0; i < headnum; i++)
        {
            for(j = 0; j < strlen(store[now].word); j++) {
                cmp[j] = tolower(store[now].word[j]);
            }
            if(strcmp(cmp, head[i]) == 0)
            {
                sign = 1;
                break;
            }
        }
        if(sign == 1)
        {
            function_call_statement_with_return_value();    //  有返回值函数调用语句
            func_label = -1;
        }
        else {
            function_call_statement_without_return_value(); //  无返回值函数调用语句
        }
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  错误k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "IDENFR")
    && (isReservedWord(1, "ASSIGN") || isReservedWord(1, "LBRACK")))
    {
        assignment_statement(); //  赋值语句
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  错误k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "SCANFTK"))
    {
        read_statement();   //  读语句
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  错误k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "PRINTFTK"))
    {
        writing_statements();   //  写语句
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  错误k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "SWITCHTK")) {
        case_statement();   //  情况语句
    }
    else if(isReservedWord(0, "RETURNTK"))
    {
        return_statement(); //  返回语句
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  错误k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "LBRACE"))
    {
        getsym();
        statement_column(); //  语句列
        if(!isReservedWord(0, "RBRACE")) {
            error_report();
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "SEMICN")) {
        getsym();   //  ＜空＞
    }
    else if(!isReservedWord(0, "SEMICN")) {
        error_k();  //  错误k ＜空＞
    }
    fprintf(fp_out,"<语句>\n");
}
void loop_statement()   //  ＜循环语句＞::=while'('＜条件＞')'＜语句＞
                        // | for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
{
    int state = run;
    int con = 0;
    int back;
    int renew;
    int *index1;
    int *index2;
    int *index3;
    int opr = 0;
    int new_val = 0;
    int new_tab = 0;
    int new_ind = 0;
    char type = ' ';
    int step_long = 0;
    char temp_word1[MAX] = {};
    char temp_word2[MAX] = {};
    if(isReservedWord(0, "WHILETK"))    // while
    {
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            back = now;
            con = condition();  //  条件
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
            if(con == 0) {
                run = 0;
            }
            sentence(); //  语句
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }
            renew = now;
            while(con == 1 && run != 0) //  运行阶段
            {
                now = back;
                con = condition();  //  条件
                if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  错误l
                }
                else {
                    getsym();
                }
                if(con == 0 || run == 0)
                {
                    now = renew;
                    break;
                }
                sentence(); //  语句
            }
        }
    }
    else if(isReservedWord(0, "FORTK")) // for
    {
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            if(!isReservedWord(0, "IDENFR")) {
                error_report();
            }
            else
            {
                error_c(2); //  错误c分析
                type = search_const();  //  检测是否为const类型
                if(type == 'x') {
                    error_j();  //  错误j
                }
                index1 = get_index(store[now].word);    //  获取标识符表对应变量下标及表号
                getsym();
                if(!isReservedWord(0, "ASSIGN")) {
                    error_report();
                }
                else
                {
                    getsym();
                    expression();   //  表达式
                    if(run == 1 && ret_valid == 0)
                    {
                        if(index1[1] == 1)  //  在局部表中
                        {
                            local_tab[index1[0]].value[0] = temp_val;
                        }
                        else if(index1[1] == 2) //  在全局表中
                        {
                            global_tab[index1[0]].value[0] = temp_val;
                        }
                    }
                    if(!isReservedWord(0, "SEMICN")) {
                        error_k();  //  错误k
                    }
                    else {
                        getsym();
                    }
                    back = now;
                    con = condition();  //  条件
                    if(!isReservedWord(0, "SEMICN")) {
                        error_k();  //  错误k
                    }
                    else {
                        getsym();
                    }
                    if(con == 0) {
                        run = 0;
                    }
                    if(!isReservedWord(0, "IDENFR")) {
                        error_report();
                    }
                    else
                    {
                        error_c(2); //  错误c分析
                        strcpy(temp_word1, store[now].word); //  暂存被赋值标识符的符号名
                        getsym();
                        if(!isReservedWord(0, "ASSIGN")) {
                            error_report();
                        }
                        else
                        {
                            getsym();
                            if(!isReservedWord(0, "IDENFR")) {
                                error_report();
                            }
                            else
                            {
                                error_c(2); //  错误c分析
                                strcpy(temp_word2,  store[now].word); //  暂存赋值标识符的符号名
                                getsym();
                                if(!isReservedWord(0, "PLUS")
                                && !isReservedWord(0 ,"MINU")) {
                                    error_report();
                                }
                                else
                                {
                                    if(isReservedWord(0, "PLUS")) {
                                        opr = 1;    //  +
                                    }
                                    else if(isReservedWord(0, "MINU")) {
                                        opr = 2;    //  -
                                    }
                                    getsym();
                                    step(); //  步长
                                    step_long = temp_val;   //  暂存步长
                                    if(!isReservedWord(0, "RPARENT")) {
                                        error_l();  //  错误l
                                    }
                                    else {
                                        getsym();
                                    }
                                    sentence(); //  语句
                                    if(run == 1 && ret_valid == 0)
                                    {
                                        index2 = get_index(temp_word1); //  获取被赋值标识符表对应变量下标及表号
                                        index3 = get_index(temp_word2); //  获取赋值标识符表对应变量下标及表号
                                        if(index2[1] == 1)  //  在局部表中
                                        {
                                            if(index3[1] == 1)  //  在局部表中
                                            {
                                                if(opr == 1) {
                                                    local_tab[index2[0]].value[0] =
                                                            local_tab[index3[0]].value[0] + step_long;
                                                }
                                                else if(opr == 2) {
                                                    local_tab[index2[0]].value[0] =
                                                            local_tab[index3[0]].value[0] - step_long;
                                                }
                                            }
                                            else if(index3[1] == 2) //  在全局表中
                                            {
                                                if(opr == 1) {
                                                    local_tab[index2[0]].value[0] =
                                                            global_tab[index3[0]].value[0] + step_long;
                                                }
                                                else if(opr == 2) {
                                                    local_tab[index2[0]].value[0] =
                                                            global_tab[index3[0]].value[0] - step_long;
                                                }
                                            }
                                        }
                                        else if(index2[1] == 2) //  在全局表中
                                        {
                                            if(index3[1] == 1)  //  在局部表中
                                            {
                                                if(opr == 1) {
                                                    global_tab[index2[0]].value[0] =
                                                            local_tab[index3[0]].value[0] + step_long;
                                                }
                                                else if(opr == 2) {
                                                    global_tab[index2[0]].value[0] =
                                                            local_tab[index3[0]].value[0] - step_long;
                                                }
                                            }
                                            else if(index3[1] == 2) //  在全局表中
                                            {
                                                if(opr == 1) {
                                                    global_tab[index2[0]].value[0] =
                                                            global_tab[index3[0]].value[0] + step_long;
                                                }
                                                else if(opr == 2) {
                                                    global_tab[index2[0]].value[0] =
                                                            global_tab[index3[0]].value[0] - step_long;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if(state == 1 && ret_valid == 0) {
                        run = 1;
                    }
                    renew = now;
                    while(con == 1 && run != 0) //  运行阶段
                    {
                        now = back;
                        con = condition();  //  条件
                        if(!isReservedWord(0, "SEMICN")) {
                            error_k();  //  错误k
                        }
                        else {
                            getsym();
                        }
                        if(con == 0 || run == 0)
                        {
                            now = renew;
                            break;
                        }
                        if(!isReservedWord(0, "IDENFR")) {
                            error_report();
                        }
                        else
                        {
                            error_c(2); //  错误c分析
                            strcpy(temp_word1, store[now].word); //  暂存被赋值标识符的符号名
                            getsym();
                            if(!isReservedWord(0, "ASSIGN")) {
                                error_report();
                            }
                            else
                            {
                                getsym();
                                if(!isReservedWord(0, "IDENFR")) {
                                    error_report();
                                }
                                else
                                {
                                    error_c(2); //  错误c分析
                                    strcpy(temp_word2, store[now].word); //  暂存赋值标识符的符号名
                                    getsym();
                                    if(!isReservedWord(0, "PLUS") && !isReservedWord(0, "MINU")) {
                                        error_report();
                                    }
                                    else
                                    {
                                        if(isReservedWord(0, "PLUS")) {
                                            opr = 1;    //  +
                                        }
                                        else if(isReservedWord(0, "MINU")) {
                                            opr = 2;    //  -
                                        }
                                        getsym();
                                        step(); //  步长
                                        step_long = temp_val;   //  暂存步长
                                        if(!isReservedWord(0, "RPARENT")) {
                                            error_l();  //  错误l
                                        }
                                        else {
                                            getsym();
                                        }
                                        sentence(); //  语句
                                        if(run == 1 && ret_valid == 0)
                                        {
                                            index2 = get_index(temp_word1); //  获取被赋值标识符表对应变量下标及表号
                                            index3 = get_index(temp_word2); //  获取赋值标识符表对应变量下标及表号
                                            if(index2[1] == 1)  //  在局部表中
                                            {
                                                if(index3[1] == 1)  //  在局部表中
                                                {
                                                    if(opr == 1) {  // +
                                                        local_tab[index2[0]].value[0] =
                                                                local_tab[index3[0]].value[0] + step_long;
                                                    }
                                                    else if(opr == 2) { // -
                                                        local_tab[index2[0]].value[0] =
                                                                local_tab[index3[0]].value[0] - step_long;
                                                    }
                                                }
                                                else if(index3[1] == 2) //  在全局表中
                                                {
                                                    if(opr == 1) {
                                                        local_tab[index2[0]].value[0] =
                                                                global_tab[index3[0]].value[0] + step_long;
                                                    }
                                                    else if(opr == 2) {
                                                        local_tab[index2[0]].value[0] =
                                                                global_tab[index3[0]].value[0] - step_long;
                                                    }
                                                }
                                            }
                                            else if(index2[1] == 2) //  在全局表中
                                            {
                                                if(index3[1] == 1)  //  在局部表中
                                                {
                                                    if(opr == 1) {
                                                        global_tab[index2[0]].value[0] =
                                                                local_tab[index3[0]].value[0] + step_long;
                                                    }
                                                    else if(opr == 2) {
                                                        global_tab[index2[0]].value[0] =
                                                                local_tab[index3[0]].value[0] - step_long;
                                                    }
                                                }
                                                else if(index3[1] == 2) //  在全局表中
                                                {
                                                    if(opr == 1) {
                                                        global_tab[index2[0]].value[0] =
                                                                global_tab[index3[0]].value[0] + step_long;
                                                    }
                                                    else if(opr == 2) {
                                                        global_tab[index2[0]].value[0] =
                                                                global_tab[index3[0]].value[0] - step_long;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    fprintf(fp_out,"<循环语句>\n");
}
void conditional_statement()    //  ＜条件语句＞::=if'('＜条件＞')'＜语句＞［else＜语句＞］
{
    int con=0;  //  表示条件真假：1为真, 0为假
    int state;
    state = run;
    if(isReservedWord(0, "IFTK"))
    {
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            con = condition();  //  条件
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
            if(con == 0) {
                run = 0;
            }
            sentence(); //  语句
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }
            if(isReservedWord(0, "ELSETK"))
            {
                getsym();
                if(con == 1) {
                    run = 0;
                }
                sentence(); //  语句
                if(state == 1 && ret_valid == 0) {
                    run = 1;
                }
            }
        }
    }
    fprintf(fp_out,"<条件语句>\n");
}
void function_call_statement_with_return_value()    //  ＜有返回值函数调用语句＞::=＜标识符＞'('＜值参数表＞')'
{
    int i = 0;
    int flag = 0;   //  是否找到相同函数名的标志
    char use_name[MAX] = {};    //  函数名小写形式
    int value[30] = {};
    int back = 0;
    int to = 0;
    int state = 0;
    state = run;
    if(isReservedWord(0, "IDENFR"))
    {
        error_c(1); //  错误c分析
        for(i = 0; i < strlen(store[now].word); i++) {
            use_name[i] = tolower(store[now].word[i]);
        }
        for(i = 0; i < func_num; i++)
        {
            if(strcmp(use_name, func_tab[i].name) == 0)
            {
                func_label = i;
                to = func_tab[i].loc;
                flag = 1;
                break;
            }
        }
        if(flag == 0) {
            func_label = -1;
        }
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            value_parameter_table(value);   //  值参数表
            if(state == 1 && ret_valid == 0)
            {
                back = now;
                now = to;
                function_definition_with_return_value(value);   //  有返回值函数定义
                now = back;
                run = 1;
                ret_valid = 0;
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
        }
    }
    fprintf(fp_out,"<有返回值函数调用语句>\n");
}
void function_call_statement_without_return_value() //  ＜无返回值函数调用语句＞::=＜标识符＞'('＜值参数表＞')'
{
    int i = 0;
    int flag = 0;
    int value[30] = {};
    int back = 0;
    int to = 0;
    char use_name[MAX] = {};    //  函数名小写形式
    int state = 0;
    state = run;
    if (isReservedWord(0, "IDENFR"))
    {
        error_c(1); //  错误c分析
        for (i = 0; i < strlen(store[now].word); i++) {
            use_name[i] = tolower(store[now].word[i]);
        }
        for (i = 0; i < func_num; i++)
        {
            if(strcmp(use_name, func_tab[i].name) == 0)
            {
                func_label = i;
                to = func_tab[i].loc;
                flag = 1;
                break;
            }
        }
        if (flag == 0) {
            func_label = -1;
        }
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            value_parameter_table(value);   //  值参数表
            if(state == 1 && ret_valid == 0)
            {
                back = now;
                now = to;
                function_definition_without_return_value(value);    //  无返回值函数定义
                now = back;
                run = 1;
                ret_valid = 0;
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
        }
    }
    func_label = -1;
    fprintf(fp_out,"<无返回值函数调用语句>\n");
}
void assignment_statement() //  ＜赋值语句＞::=＜标识符＞{'['＜表达式＞']'}＝＜表达式＞
{
    int *index;
    int i = 0, j = 0;
    char type = ' ';
    if(isReservedWord(0, "IDENFR"))
    {
        error_c(2); //  错误c分析
        type = search_const();  //  检测是否为const类型
        if(type == 'x') {
            error_j();  //  错误j
        }
        index = get_index(store[now].word); //  获取标识符表对应变量下标及表号
        getsym();
        if(isReservedWord(0, "ASSIGN"))
        {
            getsym();
            expression();   //  表达式
            if(run == 1 && ret_valid == 0)
            {
                if(index[1] == 1)   //  在局部表中
                {
                    local_tab[index[0]].value[0] = temp_val;
                }
                else if(index[1] == 2)  //  在全局表中
                {
                    global_tab[index[0]].value[0] = temp_val;
                }
            }
        }
        else if(isReservedWord(0, "LBRACK"))
        {
            getsym();
            expression();   //  表达式
            i = temp_val;
            if(!isReservedWord(0, "RBRACK")) {
                error_m();  //  错误m
            }
            else {
                getsym();
            }
            if(isReservedWord(0, "ASSIGN"))
            {
                getsym();
                expression();   //  表达式
                if(run == 1 && ret_valid == 0)
                {
                    if(index[1] == 1)   //  在局部表中
                    {
                        local_tab[index[0]].value[i] = temp_val;
                    }
                    else if(index[1] == 2)  //  在全局表中
                    {
                        global_tab[index[0]].value[i] = temp_val;
                    }
                }
            }
            else if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                expression();   //  表达式
                j = temp_val;
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  错误m
                }
                else {
                    getsym();
                }
                if(!isReservedWord(0, "ASSIGN")) {
                    error_report();
                }
                else
                {
                    getsym();
                    expression();   //  表达式
                    if(run == 1 && ret_valid == 0)
                    {
                        if(index[1] == 1)   //  在局部表中
                        {
                            local_tab[index[0]].value[i * local_tab[index[0]].j + j] = temp_val;
                        }
                        else if(index[1] == 2)  //  在全局表中
                        {
                            global_tab[index[0]].value[i * global_tab[index[0]].j + j] = temp_val;
                        }
                    }
                }
            }
        }
    }
    fprintf(fp_out,"<赋值语句>\n");
}
void read_statement()   //  ＜读语句＞::=scanf'('＜标识符＞')'
{
    int *index;
    char temp[30] = {};
    char type = ' ';
    if(isReservedWord(0, "SCANFTK"))
    {
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            if(!isReservedWord(0, "IDENFR")) {
                error_report();
            }
            else
            {
                error_c(2); //  错误c分析
                type = search_const();  //  检测是否为const类型
                if(type == 'x') {
                    error_j();  //  错误j
                }
                index = get_index(store[now].word); //  获取标识符表对应变量下标及表号
                if(run == 1 && ret_valid == 0)
                {
                    gets(temp);
                    if(index[1] == 1)   //  在局部表中
                    {
                        if(local_tab[index[0]].type == 1)   //  int类型
                        {
                            local_tab[index[0]].value[0] = atoi(temp);
                        }
                        else if(local_tab[index[0]].type == 2)  //  char类型
                        {
                            local_tab[index[0]].value[0] = temp[0];
                        }
                    }
                    else if(index[1] == 2)  //  在全局表中
                    {
                        if(global_tab[index[0]].type == 1)  //  int类型
                        {
                            global_tab[index[0]].value[0] = atoi(temp);
                        }
                        else if(global_tab[index[0]].type == 2) //  char类型
                        {
                            global_tab[index[0]].value[0] = temp[0];
                        }
                    }
                }
                getsym();
                if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  错误l
                }
                else {
                    getsym();
                }
            }
        }
    }
    fprintf(fp_out,"<读语句>\n");
}
void writing_statements()   //  ＜写语句＞::=printf'('＜字符串＞,＜表达式＞')'
                            // |printf'('＜字符串＞')'
                            // |printf'('＜表达式＞')'
{
    if(isReservedWord(0, "PRINTFTK"))
    {
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            if(isReservedWord(0, "STRCON"))
            {
                string();   //  字符串
                if(isReservedWord(0, "COMMA"))
                {
                    if(run == 1 && ret_valid == 0) {
                        fprintf(fp_result, "%s", temp_str);
                    }
                    getsym();
                    expression();   //  表达式
                    if(run == 1 && ret_valid == 0)
                    {
                        if(now_type == 'i') {
                            fprintf(fp_result, "%d\n", temp_val);
                        }
                        else if(now_type == 'c') {
                            fprintf(fp_result, "%c\n", temp_val);
                        }
                    }
                    if(!isReservedWord(0, "RPARENT")) {
                        error_l();  //  错误l
                    }
                    else {
                        getsym();
                    }
                }
                else if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  错误l
                }
                else
                {
                    if(run == 1 && ret_valid == 0) {
                        fprintf(fp_result, "%s\n", temp_str);
                    }
                    getsym();
                }
            }
            else
            {
                expression();   //  表达式
                if(run == 1 && ret_valid == 0)
                {
                    if(now_type == 'i') {
                        fprintf(fp_result, "%d\n", temp_val);
                    }
                    else if(now_type == 'c') {
                        fprintf(fp_result, "%c\n", temp_val);
                    }
                }
                if(strcmp(store[now].type,"RPARENT") != 0) {
                    error_l();  //  错误l
                }
                else {
                    getsym();
                }
            }
        }
    }
    fprintf(fp_out,"<写语句>\n");
}
void case_statement()   //  ＜情况语句＞::=switch'('＜表达式＞')''{'＜情况表＞＜缺省＞'}'
{
    int aim = 0;
    int sign = 0;
    if(isReservedWord(0, "SWITCHTK"))
    {
        getsym();
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            expression();   //  表达式
            aim = temp_val;
            if(now_type == 'i') {
                switch_type = 1;
            }
            else if(now_type == 'c') {
                switch_type = 2;
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
            if(!isReservedWord(0, "LBRACE")) {
                error_report();
            }
            else
            {
                getsym();
                sign = situation_table(aim);    //  情况表
                default_sentence(sign); //  缺省
                if(!isReservedWord(0, "RBRACE")) {
                    error_report();
                }
                else {
                    getsym();
                }
            }
        }
    }
    fprintf(fp_out,"<情况语句>\n");
}
void return_statement() //  ＜返回语句＞::=return['('＜表达式＞')']
{
    int return_temp = 0;
    if(isReservedWord(0, "RETURNTK"))
    {
        getsym();
        if(void_ret == 1 && def == 1)   //  无返回值
        {
            if(!isReservedWord(0, "SEMICN")) {
                error_g();  //  错误g
            }
        }
        else if(have_ret == 1 && def == 1)  //  有返回值
        {
            if(isReservedWord(0, "SEMICN")) {
                error_h();  //  错误h - 形如return;
            }
        }
        if(isReservedWord(0, "LPARENT"))
        {
            getsym();
            if(have_ret == 1 && isReservedWord(0, "RPARENT") && def == 1) {
                error_h();  //  错误h - 形如return();
            }
            expression();   //  表达式
            if(run == 1) {
                return_temp = temp_val;
            }
            if(have_ret == 1
            && ((now_type == 'i' && func_tab[func_num].type != 5)
            || (now_type == 'c' && func_tab[func_num].type != 6))
            && def == 1) {
                error_h();  //  错误h - return语句中表达式类型与返回值类型不一致
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  错误l
            }
            else {
                getsym();
            }
        }
    }
    if(run == 1)
    {
        ret_value = return_temp;
        run = 0;
        ret_valid = 1;
    }
    fprintf(fp_out,"<返回语句>\n");
}
int situation_table(int aim)    //  ＜情况表＞::=＜情况子语句＞{＜情况子语句＞}
{
    int flag = 0;
    int sign = 0;
    do
    {
        flag = case_sub_statement(aim); //  情况子语句
        if(flag == 1) {
            sign = 1;
        }
    }   while(isReservedWord(0, "CASETK"));
    fprintf(fp_out,"<情况表>\n");
    return sign;
}
int case_sub_statement(int aim) //  ＜情况子语句＞::=case＜常量＞：＜语句＞
{
    int state = run;
    int sign = 0;
    int type;
    if(isReservedWord(0, "CASETK"))
    {
        getsym();
        constant(switch_type);  //  常量
        if(aim == temp_val) {
            sign = 1;
        }        
        if(!isReservedWord(0, "COLON")) {
            error_report();
        }       
        else
        {
            getsym();
            if(sign != 1) {
                run = 0;
            }            
            sentence(); //  语句
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }        
        }
    }
    fprintf(fp_out,"<情况子语句>\n");
    return sign;
}
void default_sentence(int sign) //  ＜缺省＞::=default:＜语句＞
{
    int state = run;
    if(!isReservedWord(0, "DEFAULTTK")) {
        error_p();
    }
    else
    {
        getsym();
        if(!isReservedWord(0, "COLON")) {
            error_report();
        }        
        else
        {
            getsym();
            if(sign == 1) {
                run = 0;
            }            
            sentence(); //  语句
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }        
        }
    }
    fprintf(fp_out,"<缺省>\n");
}
void string()   //  ＜字符串＞::="｛十进制编码为32,33,35-126的ASCII字符｝"
{
    if(isReservedWord(0, "STRCON"))
    {
        strcpy(temp_str, store[now].word);
        getsym();
    }
    fprintf(fp_out,"<字符串>\n");
}
void value_parameter_table(int value[]) //  ＜值参数表＞::=＜表达式＞{,＜表达式＞}｜＜空＞
{
    int use_num = 0;
    char use_id[MAX] = {};
    int id_num = 0;
    int flag = 0;
    int val_num = 0;
    if(!isReservedWord(0, "RPARENT") && !isReservedWord(0, "SEMICN"))   //＜空＞
    {
        expression();   //  表达式
        value[val_num++] = temp_val;
        use_id[id_num] = now_type;
        id_num++;
        use_num++;
        while(isReservedWord(0, "COMMA"))
        {
            getsym();
            expression();   //  表达式
            value[val_num++] = temp_val;
            use_id[id_num] = now_type;
            id_num++;
            use_num++;
        }
    }
    if(func_label != -1 && use_num != func_tab[func_label].num)
    {
        error_d();  //  错误d
        flag = 1;
    }
    if(func_label != -1     
    && strcmp(use_id, func_tab[func_label].id) != 0 
    && flag == 0) {
        error_e();  //  错误e
    }
    fprintf(fp_out,"<值参数表>\n");
}
void step() //  ＜步长＞::=＜无符号整数＞
{
    unsigned_integer();   //  无符号整数
    fprintf(fp_out,"<步长>\n");
}
int condition() //  ＜条件＞::=＜表达式＞＜关系运算符＞＜表达式＞
{
    int left = 0, right = 0;
    int result = 0;
    int opr = 0;
    expression();   //  表达式
    left = temp_val;
    if(now_type != 'i') {
        error_f();  //  错误f
    }
    if(!isReservedWord(0,"LSS") && !isReservedWord(0, "LEQ")
    &&  !isReservedWord(0, "GRE") && !isReservedWord(0, "GEQ")
    &&  !isReservedWord(0, "EQL") && !isReservedWord(0, "NEQ")) {
        error_report();
    }
    else
    {
        if (isReservedWord(0, "LSS"))    //  <
        {
            opr = 1;
        }
        else if (isReservedWord(0, "LEQ"))   //  <=
        {
            opr = 2;
        }
        else if (isReservedWord(0, "GRE"))   //  >
        {
            opr = 3;
        }
        else if (isReservedWord(0, "GEQ"))  //  >=
        {
            opr = 4;
        }
        else if (isReservedWord(0, "EQL"))  //  ==
        {
            opr = 5;
        }
        else if (isReservedWord(0, "NEQ"))    // !=
        {
            opr = 6;
        }
        getsym();
        expression();   //  表达式
        right = temp_val;
        if (now_type != 'i') {
            error_f();  //  错误f
        }
        if (opr == 1)
        {
            if (left < right) {
                result = 1;
            }
        }
        else if (opr == 2)
        {
            if (left <= right) {
                result = 1;
            }
        }
        else if (opr == 3)
        {
            if (left > right) {
                result = 1;
            }
        }
        else if (opr == 4)
        {
            if (left >= right) {
                result = 1;
            }
        }
        else if (opr == 5)
        {
            if (left == right) {
                result = 1;
            }
        }
        else if (opr == 6)
        {
            if (left != right) {
                result = 1;
            }
        }
    }
    fprintf(fp_out,"<条件>\n");
    return result;
}
void expression()   //  ＜表达式＞::=［＋｜－］＜项＞{＜加法运算符＞＜项＞}
{
    int temp = 0;
    int minu_flag = 0;
    int opr = 0;
    if(isReservedWord(0, "PLUS")) {
        getsym();
    }
    else if(isReservedWord(0, "MINU"))
    {
        minu_flag = 1;
        getsym();
    }
    term(); //  项
    if(minu_flag == 1) {
        temp_val = -temp_val;
    }
    while(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU"))
    {
        if(isReservedWord(0, "PLUS")) {
            opr = 1;
        }
        else {
            opr = 2;
        }
        getsym();
        temp = temp_val;
        term(); //  项
        if(opr == 1)    //  +
        {
            temp_val = temp + temp_val;
        }
        else if(opr == 2)   //  -
        {
            temp_val = temp - temp_val;
        }
        now_type = 'i';
    }
    fprintf(fp_out,"<表达式>\n");
}
void term() //  ＜项＞::=＜因子＞{＜乘法运算符＞＜因子＞}
{
    int temp = 0;
    int opr = 0;
    factor();   //  因子
    while(isReservedWord(0, "MULT") || isReservedWord(0, "DIV"))
    {
        if(isReservedWord(0, "MULT")) {
            opr = 1;
        }
        else {
            opr = 2;
        }
        getsym();
        temp = temp_val;
        factor();   //  因子
        if(run == 1)
        {
            if(opr == 1)    //  *
            {
                temp_val = temp * temp_val;
            }
            else if(opr == 2)   // /
            {
                temp_val = temp / temp_val;
            }
        }
        now_type = 'i';
    }
    fprintf(fp_out,"<项>\n");
}
void factor()   //  ＜因子＞::=＜标识符＞
                // ｜＜标识符＞'['＜表达式＞']'
                // |＜标识符＞'['＜表达式＞']''['＜表达式＞']'
                // |'('＜表达式＞')'
                // ｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
{
    int *index;
    int i = 0, j = 0;
    int temp_ind = 0;
    int temp_tab = 0;
    int temp_label = 0;
    char temp_type = ' ';
    int flag = 0;  //   是否找到相同函数名的标志
    char use_name[MAX] = {};    //  函数名小写形式
    if(isReservedWord(0, "IDENFR") && !isReservedWord(1, "LPARENT"))
    {
        error_c(2); //  错误c分析
        temp_type = search_nametab();   //  获取标识符类型
        index = get_index(store[now].word); //  获取标识符表对应变量下标及表号
        if(index[1] == 1)   //  在局部表中
        {
            temp_val = local_tab[index[0]].value[0];
        }
        else if(index[1] == 2)  //  在全局表中
        {
            temp_val = global_tab[index[0]].value[0];
        }
        getsym();
        if(isReservedWord(0, "LBRACK"))
        {
            getsym();
            temp_ind = index[0];
            temp_tab = index[1];
            expression();   //  表达式
            i = temp_val;
            if(temp_tab == 1)   //  在局部表中
            {
                temp_val = local_tab[temp_ind].value[i];
            }
            else if(temp_tab == 2)  //  在全局表中
            {
                temp_val = global_tab[temp_ind].value[i];
            }
            if(now_type != 'i') {
                error_i();  //  错误i
            }
            //now_type = 'i';
            if(!isReservedWord(0, "RBRACK")) {
                error_m();  //  错误m
            }
            else {
                getsym();
            }
            if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                expression();   //  表达式
                j = temp_val;
                if(temp_tab == 1)   //  在局部表中
                {
                    temp_val = local_tab[temp_ind].value[i * local_tab[temp_ind].j + j];
                }
                else if(temp_tab == 2)  //  在全局表中
                {
                    temp_val = global_tab[temp_ind].value[i * global_tab[temp_ind].j + j];
                }
                if(now_type != 'i') {
                    error_i();  //  错误i
                }
                //now_type = 'i';
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  错误m
                }
                else {
                    getsym();
                }
            }
        }
        now_type = temp_type;
    }
    else if(isReservedWord(0, "LPARENT"))
    {
        getsym();
        expression();   //  表达式
        if(!isReservedWord(0, "RPARENT")) {
            error_l();  //  错误l
        }
        else {
            getsym();
        }
        now_type = 'i';
    }
    else if(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU") || isReservedWord(0, "INTCON"))
    {
        integer();  //  整数
        now_type = 'i';
    }
    else if(isReservedWord(0, "CHARCON"))
    {
        now_type = 'c';
        temp_val = store[now].word[0];
        getsym();
    }
    else if(isReservedWord(0, "IDENFR") && isReservedWord(1, "LPARENT"))
    {
        for(i = 0; i < strlen(store[now].word); i++) {
            use_name[i] = tolower(store[now].word[i]);
        }
        for(i = 0; i < func_num; i++)
        {
            if(strcmp(use_name, func_tab[i].name) == 0)
            {
                func_label = i;
                flag = 1;
                break;
            }
        }
        if(flag == 0) {
            func_label = -1;
        }
        if(func_label != -1)
        {
            temp_label = func_label;
            function_call_statement_with_return_value();    //  有返回值函数调用语句
            func_label = temp_label;
        }
        else {
            function_call_statement_with_return_value();    //  有返回值函数调用语句
        }
        if(func_tab[func_label].type == 5) {
            now_type = 'i';
        }
        else if(func_tab[func_label].type == 6) {
            now_type = 'c';
        }
        temp_val = ret_value;
    }
    fprintf(fp_out,"<因子>\n");
}
void parameter_table(int value[])   //  <参数表>::=<类型标识符><标识符>{,<类型标识符><标识符>}
                                    // |<空>
{
    int val_num = 0;
    int type;
    int id_num = 0;
    if(isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK"))
    {
        if(isReservedWord(0, "INTTK")) {
            type = 1;
        }
        else {
            type = 2;
        }
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            if(type == 1) {
                func_tab[func_num].id[id_num] = 'i';
            }
            else if(type == 2) {
                func_tab[func_num].id[id_num] = 'c';
            }
            func_tab[func_num].num++;
            id_num++;
            error_b(type);  //  错误b分析
            local_tab[lt_num].func = 1;
            if(run == 1 && ret_valid == 0) {
                local_tab[lt_num].value[0] = value[val_num++];
            }
            lt_num++;
            getsym();
            while(isReservedWord(0, "COMMA"))
            {
                getsym();
                if((!isReservedWord(0, "INTTK") && !isReservedWord(0, "CHARTK"))) {
                    error_report();
                }
                else
                {
                    if(isReservedWord(0, "INTTK")) {
                        type = 1;
                    }
                    else {
                        type = 2;
                    }
                    getsym();
                    if(!isReservedWord(0, "IDENFR")) {
                        error_report();
                    }
                    else
                    {
                        if(type == 1) {
                            func_tab[func_num].id[id_num] = 'i';
                        }
                        else if(type == 2) {
                            func_tab[func_num].id[id_num] = 'c';
                        }
                        func_tab[func_num].num++;
                        id_num++;
                        error_b(type);  //  错误b分析
                        local_tab[lt_num].func = 1;
                        if(run == 1 && ret_valid == 0) {
                            local_tab[lt_num].value[0] = value[val_num++];
                        }
                        lt_num++;
                        getsym();
                    }
                }
            }
        }
    }
    else if(strcmp(store[now].type,"RPARENT") == 0);    //  <空>
    fprintf(fp_out,"<参数表>\n");
}
void declaration_head()//<声明头部>::=int＜标识符>|char＜标识符>
{
    int type;
    int i = 0;
    char lower[MAX] = {};
    if(isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK"))
    {
        if(isReservedWord(0, "INTTK")) {
            type = 5;
        }
        else {
            type = 6;
        }
        func_tab[func_num].type = type; //  储存函数返回值类型
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            for(i = 0; i < strlen(store[now].word); i++) {
                lower[i] = tolower(store[now].word[i]);
            }
            strcpy(func_tab[func_num].name, lower); //  储存函数名
            error_b(type);  //  错误b分析
            global_tab[gt_num].func = 1;
            gt_num++;
            strcpy(head[headnum++], lower);
            getsym();
        }
    }
    fprintf(fp_out,"<声明头部>\n");
}
void variable_description() //  <变量说明>::=<变量定义>;{<变量定义>;}
{
    do
    {
        variable_definition();  //  变量定义
        if(!isReservedWord(0, "SEMICN")) {
            error_k();//错误k
        }
        else {
            getsym();
        }
    }   while((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")) && !isReservedWord(2, "LPARENT"));
    fprintf(fp_out,"<变量说明>\n");
}
void variable_definition()//<变量定义>::=<变量定义无初始化>|<变量定义及初始化>
{
    if(isReservedWord(2, "ASSIGN") ||
       (isReservedWord(2, "LBRACK") && isReservedWord(5, "ASSIGN")) ||
       (isReservedWord(2, "LBRACK") && isReservedWord(4, "ASSIGN")) ||
       (isReservedWord(2, "LBRACK") && isReservedWord(7, "ASSIGN")) ||
       (isReservedWord(2, "LBRACK") && isReservedWord(5, "LBRACK") && isReservedWord(8, "ASSIGN")))
    {
        variable_definition_and_initialization();   //  变量定义及初始化
    }
    else
    {
        variable_definition_has_no_initialization();    //  变量定义无初始化
    }
    fprintf(fp_out,"<变量定义>\n");
}
void variable_definition_has_no_initialization()//<变量定义无初始化>::=<类型标识符><标识符>{'['<无符号整数>']'}02{,<标识符>{'['<无符号整数>']'}02}
{
    int type;
    if(!isReservedWord(0, "INTTK") && !isReservedWord(0, "CHARTK")) {
        error_report();
    }
    else
    {
        if(isReservedWord(0, "INTTK")) {
            type = 1;
        }
        else {
            type = 2;
        }
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            error_b(type);  //  错误b分析
            getsym();
            if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                unsigned_integer(); //  无符号整数
                if(global_flag == 1)
                {
                    global_tab[gt_num].i = temp_val;
                    global_tab[gt_num].dim++;
                }
                else if(global_flag == 0)
                {
                    local_tab[lt_num].i = temp_val;
                    local_tab[lt_num].dim++;
                }
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  错误m
                }
                else {
                    getsym();
                }
                if(isReservedWord(0, "LBRACK"))
                {
                    getsym();
                    unsigned_integer(); //  无符号整数
                    if(global_flag == 1)
                    {
                        global_tab[gt_num].j = temp_val;
                        global_tab[gt_num].dim++;
                    }
                    else if(global_flag == 0)
                    {
                        local_tab[lt_num].j = temp_val;
                        local_tab[lt_num].dim++;
                    }
                    if(!isReservedWord(0, "RBRACK")) {
                        error_m();  //  错误m
                    }
                    else {
                        getsym();
                    }
                }
            }
        }
        if(global_flag == 1) {
            gt_num++;
        }
        else if(global_flag == 0) {
            lt_num++;
        }
    }
    while(isReservedWord(0, "COMMA"))
    {
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            error_b(type);  //  错误b分析
            getsym();
            if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                unsigned_integer(); //  无符号整数
                if(global_flag == 1)
                {
                    global_tab[gt_num].i = temp_val;
                    global_tab[gt_num].dim++;
                }
                else if(global_flag == 0)
                {
                    local_tab[lt_num].i = temp_val;
                    local_tab[lt_num].dim++;
                }
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  错误m
                }
                else {
                    getsym();
                }
                if(isReservedWord(0, "LBRACK"))
                {
                    getsym();
                    unsigned_integer(); //  无符号整数
                    if(global_flag == 1)
                    {
                        global_tab[gt_num].j = temp_val;
                        global_tab[gt_num].dim++;
                    }
                    else if(global_flag == 0)
                    {
                        local_tab[lt_num].j = temp_val;
                        local_tab[lt_num].dim++;
                    }
                    if(!isReservedWord(0, "RBRACK")) {
                        error_m();  //  错误m
                    }
                    else {
                        getsym();
                    }
                }
            }
            if(global_flag == 1) {
                gt_num++;
            }
            else if(global_flag == 0) {
                lt_num++;
            }
        }
    }
    fprintf(fp_out,"<变量定义无初始化>\n");
}
void variable_definition_and_initialization()//<变量定义及初始化>::=<类型标识符><标识符>=<常量>|<类型标识符><标识符>'['<无符号整数>']'='{'<常量>{,<常量>}'}'|<类型标识符><标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>{,<常量>}'}'{,'{'<常量>{,<常量>}'}'}'}'
{
    int type;
    int fir_def=0, sec_def=0;
    int fir_use=0, sec_use=0;
    if(!isReservedWord(0, "INTTK") && !isReservedWord(0, "CHARTK")) {
        error_report();
    }
    else
    {
        if(isReservedWord(0, "INTTK")) {
            type = 1;
        }
        else {
            type = 2;
        }
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            error_b(type);  //  错误b分析
            getsym();
            if(isReservedWord(0, "ASSIGN"))
            {
                getsym();
                constant(type); //  常量
                if(global_flag == 1) {
                    global_tab[gt_num].value[0] = temp_val;
                }
                else if(global_flag == 0) {
                    local_tab[lt_num].value[0] = temp_val;
                }
            }
            else if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                fir_def = atoi(store[now].word);    //  存储第一个数组下标
                unsigned_integer(); //  无符号整数
                if(global_flag == 1)
                {
                    global_tab[gt_num].i = temp_val;
                    global_tab[gt_num].dim++;
                }
                else if(global_flag == 0)
                {
                    local_tab[lt_num].i = temp_val;
                    local_tab[lt_num].dim++;
                }
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  错误m
                }
                else {
                    getsym();
                }
                if(isReservedWord(0, "ASSIGN"))
                {
                    getsym();
                    if(!isReservedWord(0, "LBRACE")) {
                        error_report();
                    }
                    else
                    {
                        getsym();
                        constant(type); //  常量
                        if(global_flag == 1) {
                            global_tab[gt_num].value[fir_use] = temp_val;
                        }
                        else if(global_flag == 0) {
                            local_tab[lt_num].value[fir_use] = temp_val;
                        }
                        fir_use++;
                        while(isReservedWord(0, "COMMA"))
                        {
                            getsym();
                            constant(type); //  常量
                            if(global_flag == 1) {
                                global_tab[gt_num].value[fir_use] = temp_val;
                            }
                            else if(global_flag == 0) {
                                local_tab[lt_num].value[fir_use] = temp_val;
                            }
                            fir_use++;
                        }
                        if(!isReservedWord(0, "RBRACE")) {
                            error_report();
                        }
                        else {
                            getsym();
                        }
                    }
                }
                else if(isReservedWord(0, "LBRACK"))
                {
                    getsym();
                    sec_def = atoi(store[now].word);    //  存储第二个数组下标
                    unsigned_integer(); //  无符号整数
                    if(global_flag == 1)
                    {
                        global_tab[gt_num].j = temp_val;
                        global_tab[gt_num].dim++;
                    }
                    else if(global_flag == 0)
                    {
                        local_tab[lt_num].j = temp_val;
                        local_tab[lt_num].dim++;
                    }
                    if(!isReservedWord(0, "RBRACK")) {
                        error_m();  //  错误m
                    }
                    else {
                        getsym();
                    }
                    if(isReservedWord(0, "ASSIGN"))
                    {
                        getsym();
                        if(!isReservedWord(0, "LBRACE")) {
                            error_report();
                        }
                        else
                        {
                            getsym();
                            if(!isReservedWord(0, "LBRACE")) {
                                error_report();
                            }
                            else
                            {
                                getsym();
                                constant(type); //  常量
                                if(global_flag == 1) {
                                    global_tab[gt_num].value[global_tab[gt_num].j * fir_use + sec_use] = temp_val;
                                }
                                else if(global_flag == 0) {
                                    local_tab[lt_num].value[local_tab[lt_num].j * fir_use + sec_use] = temp_val;
                                }
                                sec_use++;
                                while(isReservedWord(0, "COMMA"))
                                {
                                    getsym();
                                    constant(type); //  常量
                                    if(global_flag == 1) {
                                        global_tab[gt_num].value[global_tab[gt_num].j * fir_use + sec_use] = temp_val;
                                    }
                                    else if(global_flag == 0) {
                                        local_tab[lt_num].value[local_tab[lt_num].j * fir_use + sec_use] = temp_val;
                                    }
                                    sec_use++;
                                }
                                if(!isReservedWord(0, "RBRACE")) {
                                    error_report();
                                }
                                else
                                {
                                    if(sec_def != sec_use) {
                                        error_n();  //  错误n
                                    }
                                    sec_use = 0;
                                    fir_use++;
                                    getsym();
                                }
                                while(isReservedWord(0, "COMMA"))
                                {
                                    getsym();
                                    if(!isReservedWord(0, "LBRACE")) {
                                        error_report();
                                    }
                                    else
                                    {
                                        getsym();
                                        constant(type); //  常量
                                        if(global_flag == 1) {
                                            global_tab[gt_num].value[global_tab[gt_num].j * fir_use +
                                                                     sec_use] = temp_val;
                                        }
                                        else if(global_flag == 0) {
                                            local_tab[lt_num].value[local_tab[lt_num].j * fir_use + sec_use] = temp_val;
                                        }
                                        sec_use++;
                                        while(isReservedWord(0, "COMMA"))
                                        {
                                            getsym();
                                            constant(type); //  常量
                                            if(global_flag == 1) {
                                                global_tab[gt_num].value[global_tab[gt_num].j * fir_use +
                                                                         sec_use] = temp_val;
                                            }
                                            else if(global_flag == 0) {
                                                local_tab[lt_num].value[local_tab[lt_num].j * fir_use +
                                                                        sec_use] = temp_val;
                                            }
                                            sec_use++;
                                        }
                                        if(!isReservedWord(0, "RBRACE")) {
                                            error_report();
                                        }
                                        else
                                        {
                                            if(sec_def != sec_use) {
                                                error_n();  //  错误n
                                            }
                                            sec_use = 0;
                                            fir_use++;
                                            getsym();
                                        }
                                    }
                                }
                                if(!isReservedWord(0, "RBRACE")) {
                                    error_report();
                                }
                                else {
                                    getsym();
                                }
                            }
                        }
                    }
                }
            }
            if(global_flag == 1) {
                gt_num++;
            }
            else if(global_flag == 0) {
                lt_num++;
            }
        }
    }
    if(fir_def != fir_use) {
        error_n();  //  错误n
    }
    fprintf(fp_out,"<变量定义及初始化>\n");
}
void constant_description() //  <常量说明>::=const＜常量定义>;{const＜常量定义>;}
{
    do
    {
        if(!isReservedWord(0, "CONSTTK")) {
            error_report();
        }
        else
        {
            getsym();
            constant_definition();  //  常量定义
            if(!isReservedWord(0, "SEMICN")) {
                error_k();  //  错误k
            }
            else {
                getsym();
            }
        }
    }   while(isReservedWord(0, "CONSTTK"));
    fprintf(fp_out,"<常量说明>\n");
}
void constant_definition()  //  <常量定义>::=int＜标识符>＝<整数>{,<标识符>＝<整数>}
                            // |char＜标识符>＝<字符>{,<标识符>＝<字符>}
{
    if(!isReservedWord(0, "INTTK") && !isReservedWord(0, "CHARTK")) {
        error_report();
    }
    else if(isReservedWord(0, "INTTK"))
    {
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            error_b(3); //  错误b分析 const int
            getsym();
            if(!isReservedWord(0, "ASSIGN")) {
                error_report();
            }
            else
            {
                getsym();
                integer();  //  整数
                if(global_flag == 1)
                {
                    global_tab[gt_num].value[0] = temp_val;
                    gt_num++;
                }
                else if(global_flag == 0)
                {
                    local_tab[lt_num].value[0] = temp_val;
                    lt_num++;
                }
                while(isReservedWord(0, "COMMA"))
                {
                    getsym();
                    if(!isReservedWord(0, "IDENFR")) {
                        error_report();
                    }
                    else
                    {
                        error_b(3); //  错误b分析 const int
                        getsym();
                        if(!isReservedWord(0, "ASSIGN")) {
                            error_report();
                        }
                        else
                        {
                            getsym();
                            integer();  //  整数
                            if(global_flag == 1)
                            {
                                global_tab[gt_num].value[0] = temp_val;
                                gt_num++;
                            }
                            else if(global_flag == 0)
                            {
                                local_tab[lt_num].value[0] = temp_val;
                                lt_num++;
                            }
                        }
                    }
                }
            }
        }
    }
    else if(isReservedWord(0, "CHARTK"))
    {
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            error_b(4); //  错误b分析 const char
            getsym();
            if(!isReservedWord(0, "ASSIGN")) {
                error_report();
            }
            else
            {
                getsym();
                if(!isReservedWord(0, "CHARCON")) {
                    error_report();
                }
                else
                {
                    if(global_flag == 1)
                    {
                        global_tab[gt_num].value[0] = store[now].word[0];
                        gt_num++;
                    }
                    else if(global_flag == 0)
                    {
                        local_tab[lt_num].value[0] = store[now].word[0];
                        lt_num++;
                    }
                    getsym();
                    while(isReservedWord(0, "COMMA"))
                    {
                        getsym();
                        if(!isReservedWord(0, "IDENFR")) {
                            error_report();
                        }
                        else
                        {
                            error_b(4); //  错误b分析 const char
                            getsym();
                            if(!isReservedWord(0, "ASSIGN")) {
                                error_report();
                            }
                            else
                            {
                                getsym();
                                if(!isReservedWord(0, "CHARCON")) {
                                    error_report();
                                }
                                else
                                {
                                    if(global_flag == 1)
                                    {
                                        global_tab[gt_num].value[0] = store[now].word[0];
                                        gt_num++;
                                    }
                                    else if(global_flag == 0)
                                    {
                                        local_tab[lt_num].value[0] = store[now].word[0];
                                        lt_num++;
                                    }
                                    getsym();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    fprintf(fp_out,"<常量定义>\n");
}
void integer()  //  <整数>::=［＋｜－］<无符号整数>
{
    int minu_flag = 0;
    if(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU"))
    {
        if(isReservedWord(0, "MINU")) {
            minu_flag = 1;
        }
        getsym();
        unsigned_integer(); //  无符号整数
        if(minu_flag == 1) {
            temp_val = -temp_val;
        }
    }
    else {
        unsigned_integer(); //  无符号整数
    }
    fprintf(fp_out,"<整数>\n");
}
void unsigned_integer() //  <无符号整数>::=<数字>｛<数字>｝
{
    if(isReservedWord(0, "INTCON"))
    {
        temp_val = atoi(store[now].word);
        getsym();
    }
    fprintf(fp_out,"<无符号整数>\n");
}
void constant(int type) //  <常量>::=<整数>|<字符>
{
    if(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU") || isReservedWord(0, "INTCON"))
    {
        integer();  //  <整数>
        if(type != 1) {
            error_o();  //  错误o
        }
    }
    else if(!isReservedWord(0, "CHARCON")) {
        error_report();
    }
    else
    {
        if(type != 2) {
            error_o();  //  错误o
        }
        temp_val = store[now].word[0];
        getsym();
    }
    fprintf(fp_out,"<常量>\n");
}
