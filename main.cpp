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
    int line;   //  �к�
}   user;

typedef struct node2
{
    int line;   //  �к�
    char type;  //  ��������
}   user2;

typedef struct node3
{
    char word[MAX]; //   ���� or ������
    int type;   // ���� or �������ͱ��
    int func;   //  ��ʾ�����ںδ�����
    int dim;    //  ֵ��ά��
    int i, j;   //  �洢������±�
    int value[30];  //    ���� or ����ֵ
}   user3;

typedef struct func
{
    char name[MAX]; //  �����ĺ�����
    int type;   //  �����ķ�������
    int num;    //  ��������
    char id[MAX];   //  �������ͱ��
    int loc;    //  ������ʼλ��
}   user4;

user store[10000];
user2 err_store[100];
int addr = 0;   //  �ṹ�峤��
int now = 0;    //  Ŀǰ�����±�
int headnum = 0;    //  �з���ֵ����������(���������鳤��)
int errnum = 0; //  ���ش������(����洢���鳤��)
char err_type;  //  ��������

int global_flag = 1;    //  ȫ��ָʾ��
user3 global_tab[100];  //  ȫ�ַ��ű�
user3 local_tab[100];   //  �ֲ����ű�
int gt_num = 0, lt_num = 0; //  ��¼ȫ�ֱ�;ֲ���ĳ���
int temp_val = 0;   //  �ݴ泣�������ֵ
char temp_str[MAX]; //  �ݴ��ַ���

user4 func_tab[100];    //  ������
int func_num = 0;   //  �����ĺ����ܸ���
int func_label = -1;    //  �洢��Ӧ�±�
char now_type = ' ';    //  �洢��ǰʵ������

int void_ret = 0;   //  �����޷���ֵ���������ʶ��
int have_ret = 0;   //  �����з���ֵ���������ʶ��
int ret_valid = 0;  //  ����Ƿ���ڷ�������ʶ��
int ret_value = 0;  //  �洢����ֵ

int switch_type = 0;    //  �洢switch�жϵĳ�������

int run = 1;    //  ��ʾ����Ƿ�ִ��
int def = 1;    //  ��ʾ�Ƿ��ں�������׶�

FILE *fp_in = NULL;
FILE *fp_out = NULL;
FILE *fp_err = NULL;
FILE *fp_result = NULL;

int isletter(char letter)//�ж��Ƿ�Ϊ�ַ�
{
    if((letter >= 65 && letter <= 90)
    || (letter >= 97 && letter <= 122)
    || letter == 95)
        return 1;
    else
        return 0;
}

void getsym()   //ȡ��
{
    fprintf(fp_out,"%s %s\n", store[now].type, store[now].word);
    now++;
}

int* get_index(char name[]) //  ��ȡ�ڱ�ʶ�����е��±��Լ����ĸ�����(�ֲ�1 ȫ��2)
//���س���Ϊ2������array��array[0]Ϊ�±꣬array[1]Ϊ���
{
    int i = 0, j = 0;
    int *array;
    char sym[MAX] = {};
    char tab[MAX] = {};
    array = (int *)malloc(2*sizeof(int));
    for(i = 0; i < strlen(name); i++) {
        sym[i] = tolower(name[i]);
    }
    for(i = 0; i < lt_num; i++) // 1st: �����ֲ���
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
    for(i = 0; i < gt_num; i++) //  2nd: ����ȫ�ֱ�
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

char search_nametab()   //  ��ȫ�� or �ֲ�������������
{
    int i = 0, j = 0;
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i = 0; i < strlen(store[now].word); i++) {
        sym[i] = tolower(store[now].word[i]);
    }
    for(i = 0; i < lt_num; i++) // 1st: �����ֲ���
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
    for(i = 0; i<gt_num; i++)   // 2nd: ����ȫ�ֱ�
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

char search_const() //  ��const
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
                return 'x'; //  const����
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
                return 'x'; //  const����
            }
        }
        memset(tab, 0, sizeof(tab));
    }
    return 'n';
}

void error_report() //  ���󱨸�
{
    printf("error : %s at line: %d\n",store[now].word,store[now].line);
}
void error_a()  //  ������a
{
    err_store[errnum].line = store[addr].line;
    err_store[errnum].type = 'a';
    errnum++;
}
void error_b(int type)  //  ������b
{
    int i = 0, j = 0;
    int flag = 0;//�ж��Ƿ��ظ������ʶ��
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i=0; i<strlen(store[now].word); i++)
        sym[i] = tolower(store[now].word[i]);
    if(global_flag == 1)//ȫ�ֱ����ж�
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
    else if(global_flag == 0)//�ֲ������ж�
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
void error_c(int diff)  //  ������c
{
    int i = 0, j = 0;
    int flag = 0;//�ж��Ƿ�����
    char sym[MAX] = {};
    char tab[MAX] = {};
    for(i=0; i<strlen(store[now].word); i++)
        sym[i] = tolower(store[now].word[i]);
    if(diff == 1)//������
    {
        for(i=0; i<gt_num; i++)//����ȫ��ջ�еĺ�����
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
    else if(diff == 2)//����\������
    {
        for(i=0; i<gt_num; i++)//����ȫ��ջ�еĳ���\������
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
        for(i=0; i<lt_num && flag!=1; i++)//�����ֲ�ջ�еĳ���\������
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
void error_d()  //  ������d
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'd';
    errnum++;
}
void error_e()  //  ������e
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'e';
    errnum++;
}
void error_f()  //  ������f
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'f';
    errnum++;
}
void error_g()  //  ������g
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'g';
    errnum++;
}
void error_h()  //  ������h
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'h';
    errnum++;
}
void error_i()  //  ������i
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'i';
    errnum++;
}
void error_j()  //  ������j
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'j';
    errnum++;
}
void error_k()  //  ������k
{
    err_store[errnum].line = store[now-1].line;
    err_store[errnum].type = 'k';
    errnum++;
}
void error_l()  //  ������l
{
    err_store[errnum].line = store[now-1].line;
    err_store[errnum].type = 'l';
    errnum++;
}
void error_m()  //  ������m
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'm';
    errnum++;
}
void error_n()  //  ������n
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'n';
    errnum++;
}
void error_o()  //  ������o
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'o';
    errnum++;
}
void error_p()  //  ������p
{
    err_store[errnum].line = store[now].line;
    err_store[errnum].type = 'p';
    errnum++;
}

void addition_operator();   //  �ӷ������
void multiplication_operator(); //  �˷������
void relational_operators();    //  ��ϵ�����
void letter();  //  ��ĸ
void number();  //  ����
void character();   //  �ַ�
void string();  //  �ַ���
void program(); //  ����
void constant_description();    //  ����˵��
void constant_definition(); //  ��������
void unsigned_integer();  //  �޷�������
void integer(); //  ����
void identifier();  //  ��ʶ��
void declaration_head();    //  ����ͷ��
void constant(int type);    //  ����
void variable_description();    //  ����˵��
void variable_definition(); //  ��������
void variable_definition_has_no_initialization();   //  ���������޳�ʼ��
void variable_definition_and_initialization();  //  �������弰��ʼ��
void type_identifier(); //  ���ͱ�ʶ��
void function_definition_with_return_value(int value[]);    //  �з���ֵ��������
void function_definition_without_return_value(int value[]); //  �޷���ֵ��������
void compound_statement();  //  �������
void parameter_table(int value[]);  //  ������
void principal_function();  //  ������
void expression();  //  ���ʽ
void term();    //  ��
void factor();  //  ����
void sentence();    //  ���
void assignment_statement();    //  ��ֵ���
void conditional_statement();   //  �������
int condition();    //  ����
void loop_statement();  //  ѭ�����
void step();    //  ����
void case_statement();  //  ������
int situation_table(int aim);   //  �����
int case_sub_statement(int aim);    //  ��������
void default_sentence(int sign);    //  ȱʡ
void function_call_statement_with_return_value();   //  �з���ֵ�����������
void function_call_statement_without_return_value();    //  �޷���ֵ�����������
void value_parameter_table(int value[]);    //  ֵ������
void statement_column();    //  �����
void read_statement();  //  �����
void writing_statements();  //  д���
void return_statement();    //  �������

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
    char token = ' ';   // ��ǰ��ĸ
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

    for(i = 0; i < errnum - 1; i++) //  �����������
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
    for(i = 0; i < gt_num; i++) // ȫ�ֱ����
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
    for(i = 0; i < lt_num; i++)//�ֲ������
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
void program()  //  <����>::=��<����˵��>�ݣ�<����˵��>��{<�з���ֵ��������>|<�޷���ֵ��������>}<������>
{
    int *p = NULL;
    if(isReservedWord(0, "CONSTTK"))
    {
        constant_description(); // ����˵��
    }
    if((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")) && !isReservedWord(2, "LPARENT"))
    {
        variable_description(); // ����˵��
    }
    run = 0;
    while((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")) || (isReservedWord(0, "VOIDTK")
        && isReservedWord(1, "IDENFR")))
    {
        if((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")))
        {
            function_definition_with_return_value(p);   // �з���ֵ��������
        }
        else if((isReservedWord(0, "VOIDTK") && isReservedWord(1, "IDENFR")))
        {
            function_definition_without_return_value(p);    // �޷���ֵ��������
        }
    }
    run = 1;
    def = 0;
    if ((isReservedWord(0, "VOIDTK") && isReservedWord(1, "MAINTK"))) {
        principal_function();   // ������
    }
    fprintf(fp_out,"<����>\n");
}
void principal_function()   //  ����������::=void main'('')''{'��������䣾'}'
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
                    error_l();  //  ����l
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
                    compound_statement();   //  �������
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
    fprintf(fp_out,"<������>\n");
}
void function_definition_with_return_value(int value[])//<�з���ֵ��������>::=<����ͷ��>'('<������>')''{'<�������>'}'
{
    int i = 0;
    have_ret = 1;
    user3 temp_tab[100];    //  �ݴ���ű�
    int temp_num = 0;
    if(def == 1)
    {
        declaration_head(); //  ����ͷ��
        global_flag = 0;
        if(!isReservedWord(0, "LPARENT")) {
            error_report();
        }
        else
        {
            getsym();
            func_tab[func_num].loc = now;   //  ��¼λ��
            parameter_table(0); //  ������
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
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
                compound_statement();   //  �������
                if(!isReservedWord(0, "RBRACE")) {
                    error_report();
                }
                else
                {
                    if(ret_valid == 0) {
                        error_h();  //  ����h - ȱ��return���
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
            temp_tab[i] = local_tab[i]; //  �ݴ�ֲ���
        }
        temp_num = lt_num;
        memset(&local_tab, 0, sizeof(local_tab));   //  ��վֲ���
        lt_num = 0;
        parameter_table(value); //  ������
        getsym();
        getsym();
        compound_statement();   //  �������
        getsym();
        memset(&local_tab, 0, sizeof(local_tab));//��վֲ���
        for(i = 0; i < temp_num; i++) {
            local_tab[i] = temp_tab[i]; //  ��ԭ�ֲ���
        }
        lt_num = temp_num;
    }
    fprintf(fp_out,"<�з���ֵ��������>\n");
}
void function_definition_without_return_value(int value[])//<�޷���ֵ��������>::=void����ʶ��>'('<������>')''{'<�������>'}'
{
    int i = 0;
    char lower[MAX] = {};
    void_ret = 1;
    user3 temp_tab[100];    //  �ݴ���ű�
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
                strcpy(func_tab[func_num].name, lower);  //  �溯����
                func_tab[func_num].type = 7;    //  �溯����������
                error_b(7); //  ����b����
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
                    func_tab[func_num].loc = now;   //  ��¼λ��
                    parameter_table(p); //  ������
                    if(!isReservedWord(0, "RPARENT")) {
                        error_l();  //  ����l
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
                        compound_statement();   //  �������
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
        fprintf(fp_out,"<�޷���ֵ��������>\n");
    }
    else if (def == 0 && ret_valid == 0)
    {
        for(i = 0; i < lt_num; i++) {
            temp_tab[i] = local_tab[i]; //  �ݴ�ֲ���
        }
        temp_num = lt_num;
        memset(&local_tab, 0, sizeof(local_tab));   //  ��վֲ���
        lt_num = 0;
        parameter_table(value); //  ������
        getsym();
        getsym();
        compound_statement();   //  �������
        getsym();
        memset(&local_tab, 0, sizeof(local_tab));   //  ��վֲ���
        for(i = 0; i < temp_num; i++) {
            local_tab[i] = temp_tab[i]; //  ��ԭ�ֲ���
        }
        lt_num = temp_num;
    }
}
void compound_statement()   //  <�������>::=��<����˵��>�ݣ�<����˵��>��<�����>
{
    if(isReservedWord(0, "CONSTTK"))
    {
        constant_description(); //  ����˵��
    }
    if((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK"))
        && !isReservedWord(0, "LPARENT"))
    {
        variable_description(); //  ����˵��
    }
    statement_column(); //  �����
    fprintf(fp_out,"<�������>\n");
}
void statement_column() //  ������У�::=������䣾��
{
    while(!isReservedWord(0, "RBRACE")) {
        sentence(); //  ���
    }
    fprintf(fp_out,"<�����>\n");
}
void sentence() //  ����䣾::=��ѭ����䣾
                        // ����������䣾
                        // |���з���ֵ����������䣾;|���޷���ֵ����������䣾;
                        // ������ֵ��䣾;
                        // ��������䣾;����д��䣾;
                        // ���������䣾
                        // �����գ�;
                        // |��������䣾;
                        // |'{'������У�'}'
{
    int i = 0, j = 0, sign = 0;
    char cmp[MAX] = {};
    if(isReservedWord(0, "WHILETK") || isReservedWord(0, "FORTK")) {
        loop_statement();   //  ѭ�����
    }
    else if(isReservedWord(0, "IFTK")) {
        conditional_statement();    //  �������
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
            function_call_statement_with_return_value();    //  �з���ֵ�����������
            func_label = -1;
        }
        else {
            function_call_statement_without_return_value(); //  �޷���ֵ�����������
        }
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  ����k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "IDENFR")
    && (isReservedWord(1, "ASSIGN") || isReservedWord(1, "LBRACK")))
    {
        assignment_statement(); //  ��ֵ���
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  ����k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "SCANFTK"))
    {
        read_statement();   //  �����
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  ����k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "PRINTFTK"))
    {
        writing_statements();   //  д���
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  ����k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "SWITCHTK")) {
        case_statement();   //  ������
    }
    else if(isReservedWord(0, "RETURNTK"))
    {
        return_statement(); //  �������
        if(!isReservedWord(0, "SEMICN")) {
            error_k();  //  ����k
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "LBRACE"))
    {
        getsym();
        statement_column(); //  �����
        if(!isReservedWord(0, "RBRACE")) {
            error_report();
        }
        else {
            getsym();
        }
    }
    else if(isReservedWord(0, "SEMICN")) {
        getsym();   //  ���գ�
    }
    else if(!isReservedWord(0, "SEMICN")) {
        error_k();  //  ����k ���գ�
    }
    fprintf(fp_out,"<���>\n");
}
void loop_statement()   //  ��ѭ����䣾::=while'('��������')'����䣾
                        // | for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
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
            con = condition();  //  ����
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
            }
            else {
                getsym();
            }
            if(con == 0) {
                run = 0;
            }
            sentence(); //  ���
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }
            renew = now;
            while(con == 1 && run != 0) //  ���н׶�
            {
                now = back;
                con = condition();  //  ����
                if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  ����l
                }
                else {
                    getsym();
                }
                if(con == 0 || run == 0)
                {
                    now = renew;
                    break;
                }
                sentence(); //  ���
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
                error_c(2); //  ����c����
                type = search_const();  //  ����Ƿ�Ϊconst����
                if(type == 'x') {
                    error_j();  //  ����j
                }
                index1 = get_index(store[now].word);    //  ��ȡ��ʶ�����Ӧ�����±꼰���
                getsym();
                if(!isReservedWord(0, "ASSIGN")) {
                    error_report();
                }
                else
                {
                    getsym();
                    expression();   //  ���ʽ
                    if(run == 1 && ret_valid == 0)
                    {
                        if(index1[1] == 1)  //  �ھֲ�����
                        {
                            local_tab[index1[0]].value[0] = temp_val;
                        }
                        else if(index1[1] == 2) //  ��ȫ�ֱ���
                        {
                            global_tab[index1[0]].value[0] = temp_val;
                        }
                    }
                    if(!isReservedWord(0, "SEMICN")) {
                        error_k();  //  ����k
                    }
                    else {
                        getsym();
                    }
                    back = now;
                    con = condition();  //  ����
                    if(!isReservedWord(0, "SEMICN")) {
                        error_k();  //  ����k
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
                        error_c(2); //  ����c����
                        strcpy(temp_word1, store[now].word); //  �ݴ汻��ֵ��ʶ���ķ�����
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
                                error_c(2); //  ����c����
                                strcpy(temp_word2,  store[now].word); //  �ݴ渳ֵ��ʶ���ķ�����
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
                                    step(); //  ����
                                    step_long = temp_val;   //  �ݴ沽��
                                    if(!isReservedWord(0, "RPARENT")) {
                                        error_l();  //  ����l
                                    }
                                    else {
                                        getsym();
                                    }
                                    sentence(); //  ���
                                    if(run == 1 && ret_valid == 0)
                                    {
                                        index2 = get_index(temp_word1); //  ��ȡ����ֵ��ʶ�����Ӧ�����±꼰���
                                        index3 = get_index(temp_word2); //  ��ȡ��ֵ��ʶ�����Ӧ�����±꼰���
                                        if(index2[1] == 1)  //  �ھֲ�����
                                        {
                                            if(index3[1] == 1)  //  �ھֲ�����
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
                                            else if(index3[1] == 2) //  ��ȫ�ֱ���
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
                                        else if(index2[1] == 2) //  ��ȫ�ֱ���
                                        {
                                            if(index3[1] == 1)  //  �ھֲ�����
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
                                            else if(index3[1] == 2) //  ��ȫ�ֱ���
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
                    while(con == 1 && run != 0) //  ���н׶�
                    {
                        now = back;
                        con = condition();  //  ����
                        if(!isReservedWord(0, "SEMICN")) {
                            error_k();  //  ����k
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
                            error_c(2); //  ����c����
                            strcpy(temp_word1, store[now].word); //  �ݴ汻��ֵ��ʶ���ķ�����
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
                                    error_c(2); //  ����c����
                                    strcpy(temp_word2, store[now].word); //  �ݴ渳ֵ��ʶ���ķ�����
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
                                        step(); //  ����
                                        step_long = temp_val;   //  �ݴ沽��
                                        if(!isReservedWord(0, "RPARENT")) {
                                            error_l();  //  ����l
                                        }
                                        else {
                                            getsym();
                                        }
                                        sentence(); //  ���
                                        if(run == 1 && ret_valid == 0)
                                        {
                                            index2 = get_index(temp_word1); //  ��ȡ����ֵ��ʶ�����Ӧ�����±꼰���
                                            index3 = get_index(temp_word2); //  ��ȡ��ֵ��ʶ�����Ӧ�����±꼰���
                                            if(index2[1] == 1)  //  �ھֲ�����
                                            {
                                                if(index3[1] == 1)  //  �ھֲ�����
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
                                                else if(index3[1] == 2) //  ��ȫ�ֱ���
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
                                            else if(index2[1] == 2) //  ��ȫ�ֱ���
                                            {
                                                if(index3[1] == 1)  //  �ھֲ�����
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
                                                else if(index3[1] == 2) //  ��ȫ�ֱ���
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
    fprintf(fp_out,"<ѭ�����>\n");
}
void conditional_statement()    //  ��������䣾::=if'('��������')'����䣾��else����䣾��
{
    int con=0;  //  ��ʾ������٣�1Ϊ��, 0Ϊ��
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
            con = condition();  //  ����
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
            }
            else {
                getsym();
            }
            if(con == 0) {
                run = 0;
            }
            sentence(); //  ���
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }
            if(isReservedWord(0, "ELSETK"))
            {
                getsym();
                if(con == 1) {
                    run = 0;
                }
                sentence(); //  ���
                if(state == 1 && ret_valid == 0) {
                    run = 1;
                }
            }
        }
    }
    fprintf(fp_out,"<�������>\n");
}
void function_call_statement_with_return_value()    //  ���з���ֵ����������䣾::=����ʶ����'('��ֵ������')'
{
    int i = 0;
    int flag = 0;   //  �Ƿ��ҵ���ͬ�������ı�־
    char use_name[MAX] = {};    //  ������Сд��ʽ
    int value[30] = {};
    int back = 0;
    int to = 0;
    int state = 0;
    state = run;
    if(isReservedWord(0, "IDENFR"))
    {
        error_c(1); //  ����c����
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
            value_parameter_table(value);   //  ֵ������
            if(state == 1 && ret_valid == 0)
            {
                back = now;
                now = to;
                function_definition_with_return_value(value);   //  �з���ֵ��������
                now = back;
                run = 1;
                ret_valid = 0;
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
            }
            else {
                getsym();
            }
        }
    }
    fprintf(fp_out,"<�з���ֵ�����������>\n");
}
void function_call_statement_without_return_value() //  ���޷���ֵ����������䣾::=����ʶ����'('��ֵ������')'
{
    int i = 0;
    int flag = 0;
    int value[30] = {};
    int back = 0;
    int to = 0;
    char use_name[MAX] = {};    //  ������Сд��ʽ
    int state = 0;
    state = run;
    if (isReservedWord(0, "IDENFR"))
    {
        error_c(1); //  ����c����
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
            value_parameter_table(value);   //  ֵ������
            if(state == 1 && ret_valid == 0)
            {
                back = now;
                now = to;
                function_definition_without_return_value(value);    //  �޷���ֵ��������
                now = back;
                run = 1;
                ret_valid = 0;
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
            }
            else {
                getsym();
            }
        }
    }
    func_label = -1;
    fprintf(fp_out,"<�޷���ֵ�����������>\n");
}
void assignment_statement() //  ����ֵ��䣾::=����ʶ����{'['�����ʽ��']'}�������ʽ��
{
    int *index;
    int i = 0, j = 0;
    char type = ' ';
    if(isReservedWord(0, "IDENFR"))
    {
        error_c(2); //  ����c����
        type = search_const();  //  ����Ƿ�Ϊconst����
        if(type == 'x') {
            error_j();  //  ����j
        }
        index = get_index(store[now].word); //  ��ȡ��ʶ�����Ӧ�����±꼰���
        getsym();
        if(isReservedWord(0, "ASSIGN"))
        {
            getsym();
            expression();   //  ���ʽ
            if(run == 1 && ret_valid == 0)
            {
                if(index[1] == 1)   //  �ھֲ�����
                {
                    local_tab[index[0]].value[0] = temp_val;
                }
                else if(index[1] == 2)  //  ��ȫ�ֱ���
                {
                    global_tab[index[0]].value[0] = temp_val;
                }
            }
        }
        else if(isReservedWord(0, "LBRACK"))
        {
            getsym();
            expression();   //  ���ʽ
            i = temp_val;
            if(!isReservedWord(0, "RBRACK")) {
                error_m();  //  ����m
            }
            else {
                getsym();
            }
            if(isReservedWord(0, "ASSIGN"))
            {
                getsym();
                expression();   //  ���ʽ
                if(run == 1 && ret_valid == 0)
                {
                    if(index[1] == 1)   //  �ھֲ�����
                    {
                        local_tab[index[0]].value[i] = temp_val;
                    }
                    else if(index[1] == 2)  //  ��ȫ�ֱ���
                    {
                        global_tab[index[0]].value[i] = temp_val;
                    }
                }
            }
            else if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                expression();   //  ���ʽ
                j = temp_val;
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  ����m
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
                    expression();   //  ���ʽ
                    if(run == 1 && ret_valid == 0)
                    {
                        if(index[1] == 1)   //  �ھֲ�����
                        {
                            local_tab[index[0]].value[i * local_tab[index[0]].j + j] = temp_val;
                        }
                        else if(index[1] == 2)  //  ��ȫ�ֱ���
                        {
                            global_tab[index[0]].value[i * global_tab[index[0]].j + j] = temp_val;
                        }
                    }
                }
            }
        }
    }
    fprintf(fp_out,"<��ֵ���>\n");
}
void read_statement()   //  ������䣾::=scanf'('����ʶ����')'
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
                error_c(2); //  ����c����
                type = search_const();  //  ����Ƿ�Ϊconst����
                if(type == 'x') {
                    error_j();  //  ����j
                }
                index = get_index(store[now].word); //  ��ȡ��ʶ�����Ӧ�����±꼰���
                if(run == 1 && ret_valid == 0)
                {
                    gets(temp);
                    if(index[1] == 1)   //  �ھֲ�����
                    {
                        if(local_tab[index[0]].type == 1)   //  int����
                        {
                            local_tab[index[0]].value[0] = atoi(temp);
                        }
                        else if(local_tab[index[0]].type == 2)  //  char����
                        {
                            local_tab[index[0]].value[0] = temp[0];
                        }
                    }
                    else if(index[1] == 2)  //  ��ȫ�ֱ���
                    {
                        if(global_tab[index[0]].type == 1)  //  int����
                        {
                            global_tab[index[0]].value[0] = atoi(temp);
                        }
                        else if(global_tab[index[0]].type == 2) //  char����
                        {
                            global_tab[index[0]].value[0] = temp[0];
                        }
                    }
                }
                getsym();
                if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  ����l
                }
                else {
                    getsym();
                }
            }
        }
    }
    fprintf(fp_out,"<�����>\n");
}
void writing_statements()   //  ��д��䣾::=printf'('���ַ�����,�����ʽ��')'
                            // |printf'('���ַ�����')'
                            // |printf'('�����ʽ��')'
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
                string();   //  �ַ���
                if(isReservedWord(0, "COMMA"))
                {
                    if(run == 1 && ret_valid == 0) {
                        fprintf(fp_result, "%s", temp_str);
                    }
                    getsym();
                    expression();   //  ���ʽ
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
                        error_l();  //  ����l
                    }
                    else {
                        getsym();
                    }
                }
                else if(!isReservedWord(0, "RPARENT")) {
                    error_l();  //  ����l
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
                expression();   //  ���ʽ
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
                    error_l();  //  ����l
                }
                else {
                    getsym();
                }
            }
        }
    }
    fprintf(fp_out,"<д���>\n");
}
void case_statement()   //  �������䣾::=switch'('�����ʽ��')''{'���������ȱʡ��'}'
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
            expression();   //  ���ʽ
            aim = temp_val;
            if(now_type == 'i') {
                switch_type = 1;
            }
            else if(now_type == 'c') {
                switch_type = 2;
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
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
                sign = situation_table(aim);    //  �����
                default_sentence(sign); //  ȱʡ
                if(!isReservedWord(0, "RBRACE")) {
                    error_report();
                }
                else {
                    getsym();
                }
            }
        }
    }
    fprintf(fp_out,"<������>\n");
}
void return_statement() //  ��������䣾::=return['('�����ʽ��')']
{
    int return_temp = 0;
    if(isReservedWord(0, "RETURNTK"))
    {
        getsym();
        if(void_ret == 1 && def == 1)   //  �޷���ֵ
        {
            if(!isReservedWord(0, "SEMICN")) {
                error_g();  //  ����g
            }
        }
        else if(have_ret == 1 && def == 1)  //  �з���ֵ
        {
            if(isReservedWord(0, "SEMICN")) {
                error_h();  //  ����h - ����return;
            }
        }
        if(isReservedWord(0, "LPARENT"))
        {
            getsym();
            if(have_ret == 1 && isReservedWord(0, "RPARENT") && def == 1) {
                error_h();  //  ����h - ����return();
            }
            expression();   //  ���ʽ
            if(run == 1) {
                return_temp = temp_val;
            }
            if(have_ret == 1
            && ((now_type == 'i' && func_tab[func_num].type != 5)
            || (now_type == 'c' && func_tab[func_num].type != 6))
            && def == 1) {
                error_h();  //  ����h - return����б��ʽ�����뷵��ֵ���Ͳ�һ��
            }
            if(!isReservedWord(0, "RPARENT")) {
                error_l();  //  ����l
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
    fprintf(fp_out,"<�������>\n");
}
int situation_table(int aim)    //  �������::=���������䣾{���������䣾}
{
    int flag = 0;
    int sign = 0;
    do
    {
        flag = case_sub_statement(aim); //  ��������
        if(flag == 1) {
            sign = 1;
        }
    }   while(isReservedWord(0, "CASETK"));
    fprintf(fp_out,"<�����>\n");
    return sign;
}
int case_sub_statement(int aim) //  ���������䣾::=case��������������䣾
{
    int state = run;
    int sign = 0;
    int type;
    if(isReservedWord(0, "CASETK"))
    {
        getsym();
        constant(switch_type);  //  ����
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
            sentence(); //  ���
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }        
        }
    }
    fprintf(fp_out,"<��������>\n");
    return sign;
}
void default_sentence(int sign) //  ��ȱʡ��::=default:����䣾
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
            sentence(); //  ���
            if(state == 1 && ret_valid == 0) {
                run = 1;
            }        
        }
    }
    fprintf(fp_out,"<ȱʡ>\n");
}
void string()   //  ���ַ�����::="��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
{
    if(isReservedWord(0, "STRCON"))
    {
        strcpy(temp_str, store[now].word);
        getsym();
    }
    fprintf(fp_out,"<�ַ���>\n");
}
void value_parameter_table(int value[]) //  ��ֵ������::=�����ʽ��{,�����ʽ��}�����գ�
{
    int use_num = 0;
    char use_id[MAX] = {};
    int id_num = 0;
    int flag = 0;
    int val_num = 0;
    if(!isReservedWord(0, "RPARENT") && !isReservedWord(0, "SEMICN"))   //���գ�
    {
        expression();   //  ���ʽ
        value[val_num++] = temp_val;
        use_id[id_num] = now_type;
        id_num++;
        use_num++;
        while(isReservedWord(0, "COMMA"))
        {
            getsym();
            expression();   //  ���ʽ
            value[val_num++] = temp_val;
            use_id[id_num] = now_type;
            id_num++;
            use_num++;
        }
    }
    if(func_label != -1 && use_num != func_tab[func_label].num)
    {
        error_d();  //  ����d
        flag = 1;
    }
    if(func_label != -1     
    && strcmp(use_id, func_tab[func_label].id) != 0 
    && flag == 0) {
        error_e();  //  ����e
    }
    fprintf(fp_out,"<ֵ������>\n");
}
void step() //  ��������::=���޷���������
{
    unsigned_integer();   //  �޷�������
    fprintf(fp_out,"<����>\n");
}
int condition() //  ��������::=�����ʽ������ϵ������������ʽ��
{
    int left = 0, right = 0;
    int result = 0;
    int opr = 0;
    expression();   //  ���ʽ
    left = temp_val;
    if(now_type != 'i') {
        error_f();  //  ����f
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
        expression();   //  ���ʽ
        right = temp_val;
        if (now_type != 'i') {
            error_f();  //  ����f
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
    fprintf(fp_out,"<����>\n");
    return result;
}
void expression()   //  �����ʽ��::=�ۣ������ݣ��{���ӷ�����������}
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
    term(); //  ��
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
        term(); //  ��
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
    fprintf(fp_out,"<���ʽ>\n");
}
void term() //  ���::=�����ӣ�{���˷�������������ӣ�}
{
    int temp = 0;
    int opr = 0;
    factor();   //  ����
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
        factor();   //  ����
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
    fprintf(fp_out,"<��>\n");
}
void factor()   //  �����ӣ�::=����ʶ����
                // ������ʶ����'['�����ʽ��']'
                // |����ʶ����'['�����ʽ��']''['�����ʽ��']'
                // |'('�����ʽ��')'
                // ����������|���ַ��������з���ֵ����������䣾
{
    int *index;
    int i = 0, j = 0;
    int temp_ind = 0;
    int temp_tab = 0;
    int temp_label = 0;
    char temp_type = ' ';
    int flag = 0;  //   �Ƿ��ҵ���ͬ�������ı�־
    char use_name[MAX] = {};    //  ������Сд��ʽ
    if(isReservedWord(0, "IDENFR") && !isReservedWord(1, "LPARENT"))
    {
        error_c(2); //  ����c����
        temp_type = search_nametab();   //  ��ȡ��ʶ������
        index = get_index(store[now].word); //  ��ȡ��ʶ�����Ӧ�����±꼰���
        if(index[1] == 1)   //  �ھֲ�����
        {
            temp_val = local_tab[index[0]].value[0];
        }
        else if(index[1] == 2)  //  ��ȫ�ֱ���
        {
            temp_val = global_tab[index[0]].value[0];
        }
        getsym();
        if(isReservedWord(0, "LBRACK"))
        {
            getsym();
            temp_ind = index[0];
            temp_tab = index[1];
            expression();   //  ���ʽ
            i = temp_val;
            if(temp_tab == 1)   //  �ھֲ�����
            {
                temp_val = local_tab[temp_ind].value[i];
            }
            else if(temp_tab == 2)  //  ��ȫ�ֱ���
            {
                temp_val = global_tab[temp_ind].value[i];
            }
            if(now_type != 'i') {
                error_i();  //  ����i
            }
            //now_type = 'i';
            if(!isReservedWord(0, "RBRACK")) {
                error_m();  //  ����m
            }
            else {
                getsym();
            }
            if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                expression();   //  ���ʽ
                j = temp_val;
                if(temp_tab == 1)   //  �ھֲ�����
                {
                    temp_val = local_tab[temp_ind].value[i * local_tab[temp_ind].j + j];
                }
                else if(temp_tab == 2)  //  ��ȫ�ֱ���
                {
                    temp_val = global_tab[temp_ind].value[i * global_tab[temp_ind].j + j];
                }
                if(now_type != 'i') {
                    error_i();  //  ����i
                }
                //now_type = 'i';
                if(!isReservedWord(0, "RBRACK")) {
                    error_m();  //  ����m
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
        expression();   //  ���ʽ
        if(!isReservedWord(0, "RPARENT")) {
            error_l();  //  ����l
        }
        else {
            getsym();
        }
        now_type = 'i';
    }
    else if(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU") || isReservedWord(0, "INTCON"))
    {
        integer();  //  ����
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
            function_call_statement_with_return_value();    //  �з���ֵ�����������
            func_label = temp_label;
        }
        else {
            function_call_statement_with_return_value();    //  �з���ֵ�����������
        }
        if(func_tab[func_label].type == 5) {
            now_type = 'i';
        }
        else if(func_tab[func_label].type == 6) {
            now_type = 'c';
        }
        temp_val = ret_value;
    }
    fprintf(fp_out,"<����>\n");
}
void parameter_table(int value[])   //  <������>::=<���ͱ�ʶ��><��ʶ��>{,<���ͱ�ʶ��><��ʶ��>}
                                    // |<��>
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
            error_b(type);  //  ����b����
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
                        error_b(type);  //  ����b����
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
    else if(strcmp(store[now].type,"RPARENT") == 0);    //  <��>
    fprintf(fp_out,"<������>\n");
}
void declaration_head()//<����ͷ��>::=int����ʶ��>|char����ʶ��>
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
        func_tab[func_num].type = type; //  ���溯������ֵ����
        getsym();
        if(!isReservedWord(0, "IDENFR")) {
            error_report();
        }
        else
        {
            for(i = 0; i < strlen(store[now].word); i++) {
                lower[i] = tolower(store[now].word[i]);
            }
            strcpy(func_tab[func_num].name, lower); //  ���溯����
            error_b(type);  //  ����b����
            global_tab[gt_num].func = 1;
            gt_num++;
            strcpy(head[headnum++], lower);
            getsym();
        }
    }
    fprintf(fp_out,"<����ͷ��>\n");
}
void variable_description() //  <����˵��>::=<��������>;{<��������>;}
{
    do
    {
        variable_definition();  //  ��������
        if(!isReservedWord(0, "SEMICN")) {
            error_k();//����k
        }
        else {
            getsym();
        }
    }   while((isReservedWord(0, "INTTK") || isReservedWord(0, "CHARTK")) && !isReservedWord(2, "LPARENT"));
    fprintf(fp_out,"<����˵��>\n");
}
void variable_definition()//<��������>::=<���������޳�ʼ��>|<�������弰��ʼ��>
{
    if(isReservedWord(2, "ASSIGN") ||
       (isReservedWord(2, "LBRACK") && isReservedWord(5, "ASSIGN")) ||
       (isReservedWord(2, "LBRACK") && isReservedWord(4, "ASSIGN")) ||
       (isReservedWord(2, "LBRACK") && isReservedWord(7, "ASSIGN")) ||
       (isReservedWord(2, "LBRACK") && isReservedWord(5, "LBRACK") && isReservedWord(8, "ASSIGN")))
    {
        variable_definition_and_initialization();   //  �������弰��ʼ��
    }
    else
    {
        variable_definition_has_no_initialization();    //  ���������޳�ʼ��
    }
    fprintf(fp_out,"<��������>\n");
}
void variable_definition_has_no_initialization()//<���������޳�ʼ��>::=<���ͱ�ʶ��><��ʶ��>{'['<�޷�������>']'}02{,<��ʶ��>{'['<�޷�������>']'}02}
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
            error_b(type);  //  ����b����
            getsym();
            if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                unsigned_integer(); //  �޷�������
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
                    error_m();  //  ����m
                }
                else {
                    getsym();
                }
                if(isReservedWord(0, "LBRACK"))
                {
                    getsym();
                    unsigned_integer(); //  �޷�������
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
                        error_m();  //  ����m
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
            error_b(type);  //  ����b����
            getsym();
            if(isReservedWord(0, "LBRACK"))
            {
                getsym();
                unsigned_integer(); //  �޷�������
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
                    error_m();  //  ����m
                }
                else {
                    getsym();
                }
                if(isReservedWord(0, "LBRACK"))
                {
                    getsym();
                    unsigned_integer(); //  �޷�������
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
                        error_m();  //  ����m
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
    fprintf(fp_out,"<���������޳�ʼ��>\n");
}
void variable_definition_and_initialization()//<�������弰��ʼ��>::=<���ͱ�ʶ��><��ʶ��>=<����>|<���ͱ�ʶ��><��ʶ��>'['<�޷�������>']'='{'<����>{,<����>}'}'|<���ͱ�ʶ��><��ʶ��>'['<�޷�������>']''['<�޷�������>']'='{''{'<����>{,<����>}'}'{,'{'<����>{,<����>}'}'}'}'
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
            error_b(type);  //  ����b����
            getsym();
            if(isReservedWord(0, "ASSIGN"))
            {
                getsym();
                constant(type); //  ����
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
                fir_def = atoi(store[now].word);    //  �洢��һ�������±�
                unsigned_integer(); //  �޷�������
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
                    error_m();  //  ����m
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
                        constant(type); //  ����
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
                            constant(type); //  ����
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
                    sec_def = atoi(store[now].word);    //  �洢�ڶ��������±�
                    unsigned_integer(); //  �޷�������
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
                        error_m();  //  ����m
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
                                constant(type); //  ����
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
                                    constant(type); //  ����
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
                                        error_n();  //  ����n
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
                                        constant(type); //  ����
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
                                            constant(type); //  ����
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
                                                error_n();  //  ����n
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
        error_n();  //  ����n
    }
    fprintf(fp_out,"<�������弰��ʼ��>\n");
}
void constant_description() //  <����˵��>::=const����������>;{const����������>;}
{
    do
    {
        if(!isReservedWord(0, "CONSTTK")) {
            error_report();
        }
        else
        {
            getsym();
            constant_definition();  //  ��������
            if(!isReservedWord(0, "SEMICN")) {
                error_k();  //  ����k
            }
            else {
                getsym();
            }
        }
    }   while(isReservedWord(0, "CONSTTK"));
    fprintf(fp_out,"<����˵��>\n");
}
void constant_definition()  //  <��������>::=int����ʶ��>��<����>{,<��ʶ��>��<����>}
                            // |char����ʶ��>��<�ַ�>{,<��ʶ��>��<�ַ�>}
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
            error_b(3); //  ����b���� const int
            getsym();
            if(!isReservedWord(0, "ASSIGN")) {
                error_report();
            }
            else
            {
                getsym();
                integer();  //  ����
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
                        error_b(3); //  ����b���� const int
                        getsym();
                        if(!isReservedWord(0, "ASSIGN")) {
                            error_report();
                        }
                        else
                        {
                            getsym();
                            integer();  //  ����
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
            error_b(4); //  ����b���� const char
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
                            error_b(4); //  ����b���� const char
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
    fprintf(fp_out,"<��������>\n");
}
void integer()  //  <����>::=�ۣ�������<�޷�������>
{
    int minu_flag = 0;
    if(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU"))
    {
        if(isReservedWord(0, "MINU")) {
            minu_flag = 1;
        }
        getsym();
        unsigned_integer(); //  �޷�������
        if(minu_flag == 1) {
            temp_val = -temp_val;
        }
    }
    else {
        unsigned_integer(); //  �޷�������
    }
    fprintf(fp_out,"<����>\n");
}
void unsigned_integer() //  <�޷�������>::=<����>��<����>��
{
    if(isReservedWord(0, "INTCON"))
    {
        temp_val = atoi(store[now].word);
        getsym();
    }
    fprintf(fp_out,"<�޷�������>\n");
}
void constant(int type) //  <����>::=<����>|<�ַ�>
{
    if(isReservedWord(0, "PLUS") || isReservedWord(0, "MINU") || isReservedWord(0, "INTCON"))
    {
        integer();  //  <����>
        if(type != 1) {
            error_o();  //  ����o
        }
    }
    else if(!isReservedWord(0, "CHARCON")) {
        error_report();
    }
    else
    {
        if(type != 2) {
            error_o();  //  ����o
        }
        temp_val = store[now].word[0];
        getsym();
    }
    fprintf(fp_out,"<����>\n");
}
