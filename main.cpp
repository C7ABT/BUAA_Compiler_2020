//
// Created by Conno on 2020/9/29.
//

#include "iostream"
#include "string"
#include "algorithm"
#include "map"
#include <stdio.h>
#include "fstream"
#include "set"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

#define yes true
#define no false
using namespace std;


//  �﷨����
enum SymbolType {
    IDENFR, INTCON,   CHARCON,  STRCON,     CONSTTK,
    INTTK,  CHARTK,   VOIDTK,   MAINTK,     IFTK,
    ELSETK, SWITCHTK, CASETK,   DEFAULTTK,  WHILETK,
    FORTK,  SCANFTK,  PRINTFTK, RETURNTK,   PLUS,
    MINU,   MULT,     DIV,      LSS,        LEQ,
    GRE,    GEQ,      EQL,      NEQ,        COLON,
    ASSIGN, SEMICN,   COMMA,    LPARENT,    RPARENT,
    LBRACK, RBRACK,   LBRACE,   RBRACE,
    FOUL // ERROR symbol
};
void error() {

}
map<string, SymbolType> reservedWords = {
        {"const", CONSTTK},
        {"int",   INTTK},
        {"char",  CHARTK},
        {"void",  VOIDTK},
        {"main",  MAINTK},
        {"if", IFTK},
        {"else", ELSETK},
        {"switch", SWITCHTK},
        {"case", CASETK},
        {"default", DEFAULTTK},
        {"while", WHILETK},
        {"for", FORTK},
        {"scanf", SCANFTK},
        {"printf", PRINTFTK},
        {"return", RETURNTK}
};
map<char, SymbolType> reservedSymbol = {
        {'+', PLUS},
        {'-', MINU},
        {'*', MULT},
        {'/', DIV},
        {'<', LSS},
        {'>', GRE},
        {':', COLON},
        {'=', ASSIGN},
        {';', SEMICN},
        {',', COMMA},
        {'(', LPARENT},
        {')', RPARENT},
        {'[', LBRACK},
        {']', RBRACK},
        {'{', LBRACE},
        {'}', RBRACE}
};

const string SymbolType_String[] = {
        "IDENFR", "INTCON",   "CHARCON",  "STRCON",     "CONSTTK",
        "INTTK",  "CHARTK",   "VOIDTK",   "MAINTK",     "IFTK",
        "ELSETK", "SWITCHTK", "CASETK",   "DEFAULTTK",  "WHILETK",
        "FORTK",  "SCANFTK",  "PRINTFTK", "RETURNTK",   "PLUS",
        "MINU",   "MULT",     "DIV",      "LSS",        "LEQ",
        "GRE",    "GEQ",      "EQL",      "NEQ",        "COLON",
        "ASSIGN", "SEMICN",   "COMMA",    "LPARENT",    "RPARENT",
        "LBRACK", "RBRACK",   "LBRACE",   "RBRACE",
        "FOUL" // ERROR symbol
};

string token;
char c;     // The letter read now
char buffer[88888810]; // Everything
SymbolType symbol;
int index_buffer; // index of buffer
int line = 1;
FILE *f_in, *f_out;

void getBuffer_debug() {
    f_in = fopen("../testfile.txt", "rb");
    f_out = fopen("../pcoderesult.txt", "wb");
    fread(buffer,1, 8888888, f_in);
}
void getBuffer() {
    f_in = fopen("testfile.txt", "rb");
    f_out = fopen("pcoderesult.txt", "wb");
    fread(buffer,1, 8888888, f_in);
}
char* Int2String(int num,char *str)//10����
{
    int i = 0;//ָʾ���str
    if(num<0)//���numΪ��������num����
    {
        num = -num;
        str[i++] = '-';
    }
    //ת��
    do
    {
        str[i++] = num%10+48;//ȡnum���λ �ַ�0~9��ASCII����48~57������˵����0+48=48��ASCII���Ӧ�ַ�'0'
        num /= 10;//ȥ�����λ
    }while(num);//num��Ϊ0����ѭ��

    str[i] = '\0';

    //ȷ����ʼ������λ��
    int j = 0;
    if(str[0]=='-')//����и��ţ����Ų��õ���
    {
        j = 1;//�ӵڶ�λ��ʼ����
        ++i;//�����и��ţ����Խ����ĶԳ���ҲҪ����1λ
    }
    //�Գƽ���
    for(;j<i/2;j++)
    {
        //�Գƽ������˵�ֵ ��ʵ����ʡ���м��������a+b��ֵ��a=a+b;b=a-b;a=a-b;
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    }

    return str;//����ת�����ֵ
}
bool isPlus() {
    return (c == '+');
}   // +
bool isMinus() {
    return (c == '-');
}   // -
bool isStar() {
    return (c == '*');
}   // *
bool isDiv() {
    return (c == '/');
}      // /
bool isColon() {
    return (c == ':');
}   // :
bool isComma() {
    return (c == ',');
}   // ,
bool isSemicolon() {
    return (c == ';');
}   // ;
bool isEqual() {
    return (c == '=');
}   // =
bool isLParent() {
    return (c == '(');
}   // (
bool isRParent() {
    return (c == ')');
}   // )
bool isLBrack() {
    return (c == '[');
}   // [
bool isRBrack() {
    return (c == ']');
}   // ]
bool isLBrace() {
    return (c == '{');
}   // {
bool isRBrace() {
    return (c == '}');
}   // }
bool isLess() {
    return (c == '<');
}   // <
bool isGreater() {
    return (c == '>');
}   // >
bool isExclamation() {
    return (c == '!');
}   // !
bool isSpace() {
    return (c == ' ');
}   // " "
bool isTab() {
    return (c == '\t');
}   // \t
bool isInvisibleSymbol() {
    return (c > 0 && c <= 32);
}   // Lee Sin
bool isEOF() {
    return (c == EOF);
}   // EOF
bool isLetter() {
    return isalpha(c);
}
bool isNextline() {
    return (c == '\n' || c == '\r');
}   // \n, \r
bool isDigit() {
    return (isdigit(c));
}   // 0-9
bool isUnderscore() {
    return (c == '_');
}   // _
bool isSingleQuote() {
    return (c == '\'');
}   // \'
bool isDoubleQuote() {
    return (c == '\"');
}   // \"

void clearToken() {
    token.clear();
}

void catToken() {
    token += c;
}   // Add c to token
void clearSymbol() {
    symbol = FOUL;
}


void getChar() {
    c = buffer[index_buffer++];
    if (isNextline()) {
        ++line;
    }
}

void combo() {
    catToken();
    getChar();
}
void retract() {
    c = buffer[--index_buffer];
    if (isNextline()) {
        --line;
    }
}   // Move the fin pointer 1 step back
bool isReserved_Token() {
    string token_temp(token);
    int token_temp_len = token_temp.size();
    for(int i = 0; i < token_temp_len; ++i){
        token_temp.at(i) = tolower(token_temp.at(i));
    }
    return (reservedWords.find(token_temp) != reservedWords.end());
}   // Whether token is legal
void Reserver_Token() {
    if (isReserved_Token()) {
        string token_temp(token);
        int token_temp_len = token_temp.size();
        for(int i = 0; i < token_temp_len; ++i){
            token_temp.at(i) = tolower(token_temp.at(i));
        }
        symbol = reservedWords[token_temp];
    }   else {
        symbol = IDENFR;
    }
}

void getsym(bool output) {
    clearToken();
    clearSymbol();
    getChar();
    while (isInvisibleSymbol()) {
        getChar();
    }
    if (isEOF()) {
        return;
    }
    if (isLetter() || isUnderscore()) {
        while (isLetter() || isUnderscore() || isDigit()) {
            combo();
        }
        retract();
        Reserver_Token();
    } else if (isDigit()) {
        while (isDigit()) {
            combo();
        }
        retract();
        symbol = INTCON;
    } else if (isSingleQuote()) {
        getChar();
        if (!(isPlus() || isMinus() || isStar() || isDiv() || isLetter() || isUnderscore() || isDigit())) {
            error();
        }
        combo();
        if (!isSingleQuote()) {
            error();
        }
        symbol = CHARCON;
    } else if (isDoubleQuote()) {
        getChar();
        while (!isDoubleQuote()) {
            if (isNextline()) {
                error();
                break;
            }
            catToken();
            getChar();
        }
        symbol = STRCON;
    } else if (isLess()) {
        combo();
        if (isEqual()) {
            catToken();
            symbol = LEQ;
        } else {
            symbol = LSS;
            retract();
        }
    } else if (isGreater()) {
        combo();
        if (isEqual()) {
            catToken();
            symbol = GEQ;
        } else {
            symbol = GRE;
            retract();
        }
    } else if (isEqual()) {
        combo();
        if (isEqual()) {
            catToken();
            symbol = EQL;
        } else {
            symbol = ASSIGN;
            retract();
        }
    } else if (isExclamation()) {
        combo();
        if (isEqual()) {
            catToken();
        } else {
            error();
        }
        symbol = NEQ;
    } else if (isPlus() || isMinus() || isStar() || isDiv() || isSemicolon() || isComma() || isColon() || \
        isLParent() || isRParent() || isLBrack() || isRBrack() || isLBrace() || isRBrace()) {
        catToken();
        symbol = reservedSymbol[c];
    } else if (!isEOF()) {
        error();
    }
}

struct Symbol_Table {
    // ���ű�
    string name;
    int value;
    int type;   // 1: int, 2: char
}   symbolTable[100000];
int pos_symbolTable;

char linn[1006]= {},linl[1006]= {};
struct xhq_shuai {
    int select;
    int zhi;
    char abi;
    int you;
} aa[10006],xxhq[10006];
int chulibds(char ab[]) {
    int i, j, l, p = -1, q = 0, k = 0;
//    cout<<ab<<endl;
//    system("pause");
    l = strlen(ab);
    for (i = 0; i < l; i++) {
        if (ab[i] >= '0' && ab[i] <= '9') {
            linn[q++] = ab[i];
        } else {
            if (q == 0) p++;
            else p += 2;
            aa[p].select = 2;
            aa[p].abi = ab[i];
            if (ab[i] == '+' || ab[i] == '-')aa[p].you = 1;
            else if (ab[i] == '*' || ab[i] == '/')aa[p].you = 2;
            else if (ab[i] == '(' || ab[i] == ')')aa[p].you = 3;
            else if (ab[i] == '%') aa[p].you = 2;
            if (q != 0) {
                aa[p - 1].select = 1;
                aa[p - 1].zhi = atoi(linn);
                memset(linn, '\0', sizeof(linn));
                q = 0;
            }
        }
    }
    if (q != 0) {
        aa[p + 1].select = 1;
        aa[p + 1].zhi = atoi(linn);
    }
    j = 0;
    l = 0;
    q = 0;
    for (i = 0;; i++) {
        if (aa[i].select != 1 && aa[i].select != 2) {
            p = i;
            break;
        }
    }
    for (i = 0; i < p; i++) {
        if (aa[i].select == 1) {
            xxhq[j].select = 1;
            xxhq[j++].zhi = aa[i].zhi;
        } else {
            k = l;
            q = k - 1;
            linn[l] = aa[i].abi;
            linl[l] = aa[i].you;
            if (aa[i].abi == ')') {
                l--;
                while (linn[l] != '(') {
                    xxhq[j].select = 2;
                    xxhq[j++].abi = linn[l];
                    l--;
                }
                continue;
            }
            while (linl[k] <= linl[q] && (linn[q] != '(')) {
                if (q < 0) break;
                xxhq[j].select = 2;
                xxhq[j++].abi = linn[q];
                q--;
            }
            linn[q + 1] = aa[i].abi;
            linl[q + 1] = aa[i].you;
            l = q + 2;


        }
    }

    for (l--; l >= 0; l--) {
        xxhq[j].select = 2;
        xxhq[j++].abi = linn[l];
    }

    for (i = 0; i < j; i++) {
        if (xxhq[i].select == 2) {
            if (xxhq[i].abi == '+') {
                xxhq[i].zhi = xxhq[i - 1].zhi + xxhq[i - 2].zhi;
                for (q = i - 1; (q - 2) >= 0; q--) {
                    xxhq[q].zhi = xxhq[q - 2].zhi;
                }

            } else if (xxhq[i].abi == '-') {
                xxhq[i].zhi = xxhq[i - 2].zhi - xxhq[i - 1].zhi;
                for (q = i - 1; (q - 2) >= 0; q--) {
                    xxhq[q].zhi = xxhq[q - 2].zhi;
                }
            } else if (xxhq[i].abi == '*') {
                xxhq[i].zhi = xxhq[i - 1].zhi * xxhq[i - 2].zhi;
                for (q = i - 1; (q - 2) >= 0; q--) {
                    xxhq[q].zhi = xxhq[q - 2].zhi;
                }
            } else if (xxhq[i].abi == '/') {
                xxhq[i].zhi = xxhq[i - 2].zhi / xxhq[i - 1].zhi;
                for (q = i - 1; (q - 2) >= 0; q--) {
                    xxhq[q].zhi = xxhq[q - 2].zhi;
                }
            } else if (xxhq[i].abi == '%') {
                xxhq[i].zhi = xxhq[i - 2].zhi % xxhq[i - 1].zhi;
                for (q = i - 1; (q - 2) >= 0; q--) {
                    xxhq[q].zhi = xxhq[q - 2].zhi;
                }
            }
        }
    }
    int res = xxhq[j-1].zhi;

    memset(ab,'\0',sizeof(ab));
    memset(aa,0,sizeof(aa));
    memset(xxhq,0,sizeof(xxhq));
    return res;
}

char caculator[10000];   // �洢���ʽ
int ALU (char s[]) {
    int len = strlen(s);
    int j = 0;
    int i = 0;
    for(i=0; i<len-1; i++) { //����++  +-  -+  --
        if (s[i] == '+') {
            if (s[i + 1] == '+') {
                for (j = i + 1; j < len - 1; j++) {
                    s[j] = s[j + 1];
                }
                s[len-1]='\0';
                len--;
            }
            if (s[i + 1] == '-') {
                for (j = i; j < len - 1; j++) {
                    s[j] = s[j + 1];
                }
                s[len-1]='\0';
                len--;
            }
        } else if (s[i] == '-') {
            if (s[i + 1]== '+') {
                for (j = i + 1; j < len - 1; j++) {
                    s[j] = s[j + 1];
                }
                s[len-1]='\0';
                len--;
            }
            else if (s[i + 1] == '-') {  //������--�����
                s[i + 1] = '+';
                for (j = i; j < len - 1; j++) {
                    s[j] = s[j + 1];
                }
                s[len-1]='\0';
                len--;
            }
        } else if(s[i]=='(') {
            if(s[i+1]=='+'||s[i+1]=='-') {
                for(j=len; j>i+1; j--) {
                    s[j]=s[j-1];
                }
                s[i + 1] = '0';
                len++;
            }
        }
        if(s[i]=='*'||s[i]=='/'){
            if(s[i+1]=='-'||s[i+1]=='+'){
                for(j=len+1;j>i+2;j--){
                    s[j]=s[j-2];
                }
                s[i+2]='0';
                s[i+1]='(';
                for(j=i+4;isdigit(s[j]);j++);
                int k;
                for(k=len+2;k>j;k--){
                    s[k]=s[k-1];
                }
                s[j]=')';
                len+=3;
                s[len]='\0';
            }
        }
    }
    return chulibds(s);
}

int flag_expression;    // 1: int, 2: char
void insert_symbolTable (string name, int value, int type) {
    symbolTable[pos_symbolTable].name = name;
    symbolTable[pos_symbolTable].value = value;
    symbolTable[pos_symbolTable].type = type;
    pos_symbolTable += 1;
}

int find_symbolTable (string name) {
    for (int i = pos_symbolTable; i >= 0; --i) {
        if (symbolTable[i].name == name) {
            return i;
        }
    }
    return -1;
}
// �ʷ�����

void program(); // ����
void _string(); // �ַ���
int _const(); // ����
void _const_define(); // ��������
void _const_statement(); // ����˵��
void _var_define(); // ��������
void _var_statement(); // ����˵��
void _var_define_no_initialization(); // ���������޳�ʼ��
void _var_define_with_initialization(); // �������弰��ʼ��
void _scanf(); // �����
void _printf(); // д���
void _term(); // ��
void _factor(); // ����
int _expression(); // ���ʽ
void _statement(); // ���
void _default(); // ȱʡ
void _return(); // �������
int _unsigned_int(); // �޷�������
void _step(); // ����
void _main(); // ������
void _head_statement(); // ����ͷ��
int _int(); // ����
void _assign(); // ��ֵ���
void _switch(); // ������
void _case(); // ��������
void _table_cases(); // �����
void _function_no_return_define(); // �޷���ֵ��������
void _function_with_return_define(); // �з���ֵ��������
void _function_no_return_call(); // �޷���ֵ�����������
void _function_with_return_call(); // �з���ֵ�����������
void _loop(); // ѭ�����
void _table_parameter(); // ������
void _table_parameter_value(); // ֵ������
void _condition(); // ����
void _if(); // �������
void _list_statement(); // �����
void _statement_combination(); // �������
int _char(); // �ַ�

// �����Ƿָ��� //
SymbolType symbol_pre;
string array_function_with_return[1000]; // �з���ֵ������
string array_function_no_return[1000];    // �޷���ֵ������
int index_array_function_with_return;
int index_array_function_no_return;
string token_pre;
void pre_read_Symbol(int n){
    // Ԥ��
    SymbolType symbol_origin = symbol;
    string token_origin = token;
    int index_buffer_origin = index_buffer;
    while (n--) {
        getsym(no);
    }
    token_pre = token;
    symbol_pre = symbol;    // Ԥ���õ���symbol
    symbol = symbol_origin; // ��ԭsymbol
    token = token_origin;   // ��ԭtoken
    index_buffer = index_buffer_origin; // ��ԭָ��
} // Ԥ��

// ����Ҳ�Ƿָ��� //

void program() {
    /* ������    ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾
     *              | ���޷���ֵ�������壾}����������
     */
    if (symbol == CONSTTK) {
        _const_statement(); // ����˵��
    }
    pre_read_Symbol(2);
    if (symbol_pre != LPARENT) {
        _var_statement();   // ����˵��
    }
    while (yes) {
        pre_read_Symbol(1);
        if (symbol_pre == MAINTK) {
            break;
        }
        if (symbol == VOIDTK) {
            _function_no_return_define();
        } else {
            _function_with_return_define();
        }
    }
    _main();
//    fprintf(f_out, "<����>\n");
//    cout << "<����>" << endl;
}

void _const_statement() {
    // ������˵���� ::=  const���������壾;{ const���������壾;}
    if (symbol == CONSTTK) {
        while (symbol == CONSTTK) {
            getsym(yes);
            _const_define(); // ��������
            if (symbol == SEMICN) { // ;
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<����˵��>\n");
//    cout << "<����˵��>" << endl;
}

////////////////////
void _var_statement() {
    // ������˵����  ::= ���������壾;{���������壾;}
    do {
        _var_define();  // ��������
        if (symbol == SEMICN) { // ;
            getsym(yes);
        }
        if (symbol != INTTK && symbol != CHARTK) { // int || char
            break;
        }
        pre_read_Symbol(2);
        if (symbol_pre == LPARENT) { // (
            break;
        }
    }   while (yes);
//    fprintf(f_out, "<����˵��>\n");
//    cout << "<����˵��>" << endl;
}
////////////////////

void _const_define() {
    // ���������壾   ::=   int����ʶ��������������{,����ʶ��������������}
    //                  | char����ʶ���������ַ���{,����ʶ���������ַ���}
    int type, value;
    string name;
    if (symbol == INTTK) { // int
        type = 1;
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            name = token;
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                value = _int(); // ����
                insert_symbolTable(name, value, type);
                while (symbol == COMMA) { // ,
                    type = 1;
                    getsym(yes);
                    if (symbol == IDENFR) { // ��ʶ��
                        name = token;
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        value = _int(); // ����
                        insert_symbolTable(name, value, type);
                    }
                }
            }
        }
    }

    else if (symbol == CHARTK) { // char
        getsym(yes);
        type = 2;
        if (symbol == IDENFR) { // ��ʶ��
            name = token;
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                value = _char(); // �ַ�
                insert_symbolTable(name, value, type);
                while (symbol == COMMA) { // ,
                    type = 2;
                    getsym(yes);
                    if (symbol == IDENFR) { // ��ʶ��
                        name = token;
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        value = _char(); // �ַ�
                        insert_symbolTable(name, value, type);
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<��������>\n");
//    cout << "<��������>" << endl;
}

void _function_with_return_define() {
    // ���з���ֵ�������壾  ::=  ������ͷ����'('��������')' '{'��������䣾'}'
    _head_statement();  // ����ͷ��
    if (symbol == LPARENT) { // (
        getsym(yes);
        _table_parameter(); // ������
        if (symbol == RPARENT) { // )
            getsym(yes);
            if (symbol == LBRACE) { // {
                getsym(yes);
                _statement_combination(); // �������
                if (symbol == RBRACE) { // }
                    getsym(yes);
                }
            }
        }
    }
//    fprintf(f_out, "<�з���ֵ��������>\n");
//    cout << "<�з���ֵ��������>" << endl;
}

void _head_statement() {
    // ������ͷ����   ::=  int����ʶ����
    //                |  char����ʶ����
    if (symbol == CHARTK || symbol == INTTK) { // int || char
        getsym(yes);
        array_function_with_return[index_array_function_with_return++] = token;
        if (symbol == IDENFR) { // ��ʶ��
            getsym(yes);
        }
    }
//    fprintf(f_out, "<����ͷ��>\n");
//    cout << "<����ͷ��>" << endl;
}

void _table_parameter() {
    // ��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}
    //               |  ���գ�
    if (symbol == INTTK || symbol == CHARTK) { // ���ͱ�ʶ��
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            getsym(yes);
        }
        if (symbol == COMMA) { // ,
            while (symbol == COMMA) { // ,
                getsym(yes);
                if (symbol == INTTK || symbol == CHARTK) { // ���ͱ�ʶ��
                    getsym(yes);
                    if (symbol == IDENFR) { // ��ʶ��
                        getsym(yes);
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<������>\n");
//    cout << "<������>" << endl;
}

void _statement_combination() {
    // ��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
    if (symbol == CONSTTK) { // ����˵��
        _const_statement();
    }
    if (symbol == INTTK || symbol == CHARTK) { // ����˵��
        _var_statement();
    }
    _list_statement();  // �����
//    fprintf(f_out, "<�������>\n");
//    cout << "<�������>" << endl;
}

void _function_no_return_define() {
    // ���޷���ֵ�������壾  ::= void����ʶ����'('��������')''{'��������䣾'}'
    if (symbol == VOIDTK) { // void
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            array_function_no_return[index_array_function_no_return++] = token;
            getsym(yes);
            if (symbol == LPARENT) { // (
                getsym(yes);
                _table_parameter(); // ������
                if (symbol == RPARENT) { // )
                    getsym(yes);
                    if (symbol == LBRACE) { // {
                        getsym(yes);
                        _statement_combination(); // �������
                        if (symbol == RBRACE) { // }
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<�޷���ֵ��������>\n");
//    cout << "<�޷���ֵ��������>" << endl;
}

void _main() {
    // ����������    ::= void main��(����)�� ��{����������䣾��}��
    if (symbol == VOIDTK) { // void
        getsym(yes);
        if (symbol == MAINTK) { // main
            getsym(yes);
            if (symbol == LPARENT) { // (
                getsym(yes);
                if (symbol == RPARENT) { // )
                    getsym(yes);
                    if (symbol == LBRACE) { // {
                        getsym(yes);
                        _statement_combination(); // �������
                        if (symbol == RBRACE) { // }
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<������>\n");
//    cout << "<������>" << endl;
}

void _list_statement() {
    // ������У�   ::= ������䣾��
    while (symbol == IFTK || symbol == WHILETK || symbol == FORTK || symbol == LBRACE
           || symbol == IDENFR || symbol == SCANFTK || symbol == PRINTFTK || symbol == SEMICN
           || symbol == RETURNTK || symbol == SWITCHTK ) { // if while for ( ��ʶ�� scanf printf ; return switch
        _statement(); // ���
    }
//    fprintf(f_out, "<�����>\n");
//    cout << "<�����>" << endl;
}

void _if() {
    // ��������䣾  ::= if '('��������')'����䣾��else����䣾��
    if (symbol == IFTK) { // if
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _condition(); // ����
            if (symbol == RPARENT) { // )
                getsym(yes);
                _statement(); // ���
                if (symbol == ELSETK) { // else
                    getsym(yes);
                    _statement(); // ���
                }
            }
        }
    }
//    fprintf(f_out, "<�������>\n");
//    cout << "<�������>" << endl;
}

void _condition() {
    // ��������    ::=  �����ʽ������ϵ������������ʽ��
    _expression(); // ���ʽ
    if (symbol == LSS || symbol == LEQ
        || symbol == GRE || symbol == GEQ
        || symbol == NEQ || symbol == EQL) {
        // < <= > >= != ==
        getsym(yes);
        _expression(); // ���ʽ
    }
//    fprintf(f_out, "<����>\n");
//    cout << "<����>" << endl;
}

int _expression() {
    //  �����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
    int i = 0;
    int res = 0;
    int flag = 0;  // �Ƿ����Ը�ֵ���, printf�����Ҫ�ڿ�ͷ���(��ͬʱ��β��Ҫ����)
    if (symbol == ASSIGN) {
        flag = 1;
        getsym(no);
    }

    if (symbol == CHARCON) {
        pre_read_Symbol(1);
        if (symbol_pre == RPARENT) {
            res = token[0];
            flag_expression = 2;
            getsym(no);
            return res;
        }   // printf �����ַ�
        else if (symbol_pre == SEMICN || symbol_pre == COMMA) {
            res = token[0];
            flag_expression = 2;
            getsym(no);
            return res;
        }   // ��ֵ �����ַ�
        else {
            flag_expression = 1;
        }
    }
    else if (symbol == INTCON) {
        pre_read_Symbol(1);
        if (symbol_pre == RPARENT) {
            res = atoi(token.c_str());
            flag_expression = 1;
            getsym(no);
            return res;
        }   // printf ����
        else if (symbol_pre == SEMICN || symbol_pre == COMMA) {
            res = atoi(token.c_str());
            flag_expression = 1;
            getsym(no);
            return res;
        }   // ��ֵ ����
        else {
            flag_expression = 1;
        }
    }
    else if (symbol == IDENFR) {
        pre_read_Symbol(1);
        if (symbol_pre == RPARENT) {
            int pos = find_symbolTable(token);
            if (symbolTable[pos].type == 1) {
                res = symbolTable[pos].value;
                flag_expression = 1;
                getsym(no);
                return res;
            }   // printf int�ͱ�ʶ��
            else if (symbolTable[pos].type == 2) {
                res = symbolTable[pos].value;
                flag_expression = 2;
                getsym(no);
                return res;
            }   // printf char�ͱ�ʶ��
            else {
                flag_expression = 1;
            }
        }
        else if (symbol_pre == SEMICN || symbol_pre == COMMA) {
            int pos = find_symbolTable(token);
            if (symbolTable[pos].type == 1) {
                res = symbolTable[pos].value;
                flag_expression = 1;
                getsym(no);
                return res;
            }   // ��ֵ int�ͱ�ʶ��
            else if (symbolTable[pos].type == 2) {
                res = symbolTable[pos].value;
                flag_expression = 2;
                getsym(no);
                return res;
            }   // ��ֵ char�ͱ�ʶ��
        }
        else {
            flag_expression = 1;
        }
    }
    else {
        flag_expression = 1;
    }
    if (flag_expression == 1) {
//        pre_read_Symbol(2);
//        if (symbol_pre == SEMICN) {
//            int pos = find_symbolTable(token);
//            if (symbolTable[pos].type == 2) {
//                flag_expression = 2;
//            }   // printf ��ʶ�� char��
//            else {
//                flag_expression = 1;
//                // printf ��ʶ�� int��
//            }
//            getsym(no);
//            return symbolTable[pos].value;
//        }
        memset(caculator, '\0', sizeof(caculator));
        caculator[0] = '(';
        symbol_pre = FOUL;
        while (symbol_pre != SEMICN) {
            pre_read_Symbol(i);
            if (symbol_pre == IDENFR) {
                int pos = find_symbolTable(token_pre);
                char value[1000] = {};
                Int2String(symbolTable[pos].value, value);
                strcat(caculator, value);
            }
            else if (symbol_pre == CHARCON) {
                int temp = token_pre[0];
                char value[1000] = {};
                Int2String(temp, value);
                strcat(caculator, value);
            }
            else if (symbol_pre == SEMICN) {
                if (flag == 1) {
                    // ��ֵ��� ĩβ���)
                    int len = strlen(caculator);
                    caculator[len] = ')';
                }
            }
            else {
                strcat(caculator, token_pre.c_str());
            }
            i += 1;
        }
//        cout << caculator << endl;
        res = ALU(caculator);
        while (i--) {
            getsym(no);
        }
//        cout << res << endl;
        return res;
    }
    if (symbol == PLUS || symbol == MINU) { // �ӷ������
        getsym(yes);
    }
    _term(); // ��
    while (symbol == PLUS || symbol == MINU) { // �ӷ������
        getsym(yes);
        _term(); // ��
    }
    return res;
//    fprintf(f_out, "<���ʽ>\n");
//    cout << "<���ʽ>" << endl;
}

void _term() {
    // ���     ::= �����ӣ�{���˷�������������ӣ�}
    _factor(); // ����
    while (symbol == MULT || symbol == DIV) { // �˷������
        getsym(yes);
        _factor();  // ����
    }
//    fprintf(f_out, "<��>\n");
//    cout << "<��>" << endl;
}

void _loop() {
    // ��ѭ����䣾   ::=  while '('��������')'����䣾
    // | for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
    if (symbol == WHILETK) { // while
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _condition();   // ����
            if (symbol == RPARENT) { // )
                getsym(yes);
                _statement();   // ���
            }
        }
    }

    else if (symbol == FORTK) { // for
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == IDENFR) { // ��ʶ��
                getsym(yes);
                if (symbol == ASSIGN) { // =
                    getsym(yes);
                    _expression(); // ���ʽ
                    if (symbol == SEMICN) { // ;
                        getsym(yes);
                        _condition(); // ����
                        if (symbol == SEMICN) { // ;
                            getsym(yes);
                            if (symbol == IDENFR) { // ��ʶ��
                                getsym(yes);
                                if (symbol == ASSIGN) { // =
                                    getsym(yes);
                                    if (symbol == IDENFR) { // ��ʶ��
                                        getsym(yes);
                                        if (symbol == PLUS || symbol == MINU) { // + || -
                                            getsym(yes);
                                            _step();    // ����
                                            if (symbol == RPARENT) { // )
                                                getsym(yes);
                                                _statement(); // ���
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
//    fprintf(f_out, "<ѭ�����>\n");
//    cout << "<ѭ�����>" << endl;
}

void _string() {
    // ���ַ�����   ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
    if (symbol == STRCON) { // �ַ���
        getsym(yes);
    }
//    fprintf(f_out, "<�ַ���>\n");
//    cout << "<�ַ���>" << endl;
}

void _step() {
    // ��������::= ���޷���������
    _unsigned_int(); // �޷�������
//    fprintf(f_out, "<����>\n");
//    cout << "<����>" << endl;
}

int _unsigned_int() {
    // ���޷���������  ::= �����֣��������֣���
    if (symbol == INTCON) {
//        int res = stoi(token);
        int res = atoi(token.c_str());
        getsym(yes);
        return res;
    }
//    fprintf(f_out, "<�޷�������>\n");
//    cout << "<�޷�������>" << endl;
    return 0;
}

int _int() {
    // ��������        ::= �ۣ������ݣ��޷���������
    int flag = 0;   // 1: ����
    if (symbol == PLUS || symbol == MINU) {
        flag = (symbol == MINU) ? 1 : 0;
        getsym(yes);
    }
    if (flag == 1) {
        return _unsigned_int() * (-1); // ����
    }
    return _unsigned_int(); // �Ǹ���
//    fprintf(f_out, "<����>\n");
//    cout << "<����>" << endl;
}

int _char() {
    int res;
    if (symbol == CHARCON) { // �ַ�
        res = (char)token[0];
        getsym(yes);
        return res;
    }
    return 0;
}

void _scanf() {
    // ������䣾    ::=  scanf '('����ʶ����')'
    if (symbol == SCANFTK) { // scanf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == IDENFR) { // ��ʶ��
                int pos = find_symbolTable(token);
                if (symbolTable[pos].type == 1) {
                    cin >> symbolTable[pos].value;
                }
                else {
                    char c;
                    cin >> c;
                    symbolTable[pos].value = (int)c;
                }
                getsym(yes);
            }
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<�����>\n");
//    cout << "<�����>" << endl;
}

void _printf() {
    // ��д��䣾    ::= printf '(' ���ַ�����,�����ʽ�� ')'
    //              |  printf '('���ַ����� ')'
    //              | printf '('�����ʽ��')'
    int value;
    if (symbol == PRINTFTK) { // printf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == STRCON) { // �ַ���
                fprintf(f_out, "%s", token.c_str());
                printf("%s", token.c_str());
                _string();  // �ַ���
                if (symbol == COMMA) { // ,
                    getsym(yes);
                    value = _expression();  // ���ʽ
                    switch (flag_expression) {
                        case 1:
                            fprintf(f_out, "%d\n", value);
                            printf("%d\n", value);
                            break;
                        case 2:
                            fprintf(f_out, "%c\n", value);
                            printf("%c\n", value);
                            break;
                    }
                }
                else {
                    fprintf(f_out, "\n");
                    printf("\n");
                }
            }
            else {
                value = _expression();  // ���ʽ
                switch (flag_expression) {
                    case 1:
                        fprintf(f_out, "%d\n", value);
                        printf("%d\n", value);
                        break;
                    case 2:
                        fprintf(f_out, "%c\n", value);
                        printf("%c\n", value);
                        break;
                }
            }
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<д���>\n");
//    cout << "<д���>" << endl;
}

void _return() {
    // ��������䣾   ::=  return['('�����ʽ��')']
    if (symbol == RETURNTK) { // return
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _expression();  // ���ʽ
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<�������>\n");
//    cout << "<�������>" << endl;
}

void _function_with_return_call() {
    // ���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
    if (symbol == IDENFR) { // ��ʶ��
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _table_parameter_value(); // ֵ������
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<�з���ֵ�����������>\n");
//    cout << "<�з���ֵ�����������>" << endl;
}

void _function_no_return_call() {
    // ���޷���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
    if (symbol == IDENFR) { // ��ʶ��
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _table_parameter_value(); // ֵ������
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<�޷���ֵ�����������>\n");
//    cout << "<�޷���ֵ�����������>" << endl;
}

void _table_parameter_value() {
    // ��ֵ������   ::= �����ʽ��{,�����ʽ��}
    //              ��   ���գ�
    if (symbol == RPARENT) { // ��
//        fprintf(f_out, "<ֵ������>\n");
//        cout << "<ֵ������>" << endl;
    }
    else {
        _expression();  // ���ʽ
        while (symbol == COMMA) { // ,
            getsym(yes);
            _expression();  // ���ʽ
        }
//        fprintf(f_out, "<ֵ������>\n");
//        cout << "<ֵ������>" << endl;
    }
}

void _statement() {
    // ����䣾    ::= ��ѭ����䣾
    //              ����������䣾
    //              | ���з���ֵ����������䣾;
    //              | ���޷���ֵ����������䣾;
    //              ������ֵ��䣾;
    //              ��������䣾;
    //              ����д��䣾;
    //              ���������䣾
    //              �����գ�;
    //              |��������䣾;
    //              | '{'������У�'}'
    if (symbol == SWITCHTK) {   // ������
        _switch();  // ������
    }
    else if (symbol == IFTK) {   // �������
        _if();  // �������
    }
    else if (symbol == WHILETK || symbol == FORTK) {    // ѭ�����
        _loop();    // ѭ�����
    }
    else if (symbol == LBRACE) { // {�����}
        getsym(yes);
        _list_statement();  // �����
        if (symbol == RBRACE) { // }
            getsym(yes);
        }
    }
    else if (symbol == IDENFR) {    // ��ֵ��� ���޷���ֵ�����������
        pre_read_Symbol(1);
        if (symbol_pre == LPARENT) { // (
            // ���޷���ֵ�����������
            for (int i = 0; i < index_array_function_with_return; ++i) {
                if (array_function_with_return[i] == token) {
                    _function_with_return_call(); // �з���ֵ�����������
                    break;
                }
            }
            for (int i = 0; i < index_array_function_no_return; ++i) {
                if (array_function_no_return[i] == token) {
                    _function_no_return_call(); // �޷���ֵ�����������
                    break;
                }
            }
            if (symbol == SEMICN) { // ;
                getsym(yes);
            }
        }
        else {
            // ��ֵ���
            _assign();
            if (symbol == SEMICN) { // ;
                getsym(yes);
            }
        }
    }
    else if (symbol == SEMICN) { // ;
        // ��
        getsym(yes);
    }
    else if (symbol == PRINTFTK) {
        // д���
        _printf();
        if (symbol == SEMICN) { // ;
            getsym(yes);
        }
    }
    else if (symbol == SCANFTK) {
        // �����
        _scanf();
        if (symbol == SEMICN) { // ;
            getsym(yes);
        }
    }
    else if (symbol == RETURNTK) {
        // �������
        _return();
        if (symbol == SEMICN) {
            getsym(yes);
        }
    }
//    fprintf(f_out, "<���>\n");
//    cout << "<���>" << endl;
}

void _default() {
    // ��ȱʡ��   ::=  default :����䣾
    if (symbol == DEFAULTTK) { // default
        getsym(yes);
        if (symbol == COLON) { // :
            getsym(yes);
            _statement();   // ���
        }
    }
//    fprintf(f_out, "<ȱʡ>\n");
//    cout << "<ȱʡ>" << endl;
}

void _switch() {
    // �������䣾  ::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ����}��
    if (symbol == SWITCHTK) { // switch
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _expression();   // ���ʽ
            if (symbol == RPARENT) { // )
                getsym(yes);
                {
                    if (symbol == LBRACE) { // {
                        getsym(yes);
                        _table_cases();  // �����
                        _default();  // ȱʡ
                        if (symbol == RBRACE) {
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<������>\n");
//    cout << "<������>" << endl;
}

void _case() {
    // ���������䣾  ::=  case��������������䣾
    if (symbol == CASETK) {  // case
        getsym(yes);
        _const();    // ����
        if (symbol == COLON) {   // :
            getsym(yes);
            _statement();    // ���
        }
    }
//    fprintf(f_out, "<��������>\n");
//    cout << "<��������>" << endl;
}

void _table_cases() {
    // �������   ::=  ���������䣾{���������䣾}
    while (symbol == CASETK) {
        _case();
    }
//    fprintf(f_out, "<�����>\n");
//    cout << "<�����>" << endl;
}

int _const() {
    // ��������   ::=  ��������
    //             |  ���ַ���
    if (symbol == INTCON || symbol == PLUS || symbol == MINU) { // ����
        return _int(); // ����
    }
    else if (symbol == CHARCON) { // �ַ�
        return _char();    // �ַ�
    }
    return 0;
//    fprintf(f_out, "<����>\n");
//    cout << "<����>" << endl;
}

void _var_define() {
    // ���������壾 ::= �����������޳�ʼ����
    //              | ���������弰��ʼ����

    // �����������޳�ʼ����  ::= �����ͱ�ʶ����(����ʶ����
    //                      | ����ʶ����'['���޷���������']'
    //                      | ����ʶ����'['���޷���������']''['���޷���������']'){,(����ʶ����
    //                      | ����ʶ����'['���޷���������']'
    //                      | ����ʶ����'['���޷���������']''['���޷���������']' )}

    // ���������弰��ʼ����  ::= �����ͱ�ʶ��������ʶ����=��������
    //                      | �����ͱ�ʶ��������ʶ����'['���޷���������']'='{'��������{,��������}'}'
    //                      | �����ͱ�ʶ��������ʶ����'['���޷���������']''['���޷���������']'='
    //                      {''{'��������{,��������}'}'{, '{'��������{,��������}'}'}'}'

    if (symbol == INTTK || symbol == CHARTK) { // ���ͱ�ʶ��
        int i = 1;
        while (symbol_pre != COMMA && symbol_pre != SEMICN && symbol_pre != ASSIGN) {
            pre_read_Symbol(i++) ;
        }
        if (symbol_pre == SEMICN || symbol_pre == COMMA) { // ; ,
            _var_define_no_initialization();    // ���������޳�ʼ��
        }
        else {
            _var_define_with_initialization();  // �������弰��ʼ��
        }
    }
//    fprintf(f_out, "<��������>\n");
//    cout << "<��������>" << endl;
}

void _assign() {
    // ����ֵ��䣾   ::=  ����ʶ�����������ʽ��
    //                |  ����ʶ����'['�����ʽ��']'=�����ʽ��
    //                |  ����ʶ����'['�����ʽ��']''['�����ʽ��']' =�����ʽ��
    int value;
    string name;
    int index;
    if (symbol == IDENFR) { // ��ʶ��
        index = find_symbolTable(token);
        getsym(yes);
//        if (symbol == ASSIGN) { // =
//            getsym(yes);
//            value = _expression();  // ���ʽ
//            symbolTable[index].value = value;
//        }
        // = �Ž����ʽ�д���������ʽ��Դ�Ǹ�ֵ��仹��printf
        value = _expression();  // ���ʽ
        symbolTable[index].value = value;
        if (symbol == LBRACK) {    // [
            getsym(yes);
            _expression();  // ���ʽ
            if (symbol == RBRACK) { // ]
                getsym(yes);
                if (symbol == ASSIGN) { // =
                    getsym(yes);
                    _expression(); // ���ʽ
                }
                else if (symbol == LBRACK) {    // [
                    getsym(yes);
                    _expression();  // ���ʽ
                    if (symbol == RBRACK) { // ]
                        getsym(yes);
                        if (symbol == ASSIGN) { // =
                            getsym(yes);
                            _expression();  // ���ʽ
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<��ֵ���>\n");
//    cout << "<��ֵ���>" << endl;
}

void _factor() {
    // �����ӣ�    ::= ����ʶ����
    //              ������ʶ����'['�����ʽ��']'
    //              | ����ʶ����'['�����ʽ��']''['�����ʽ��']'
    //              |'('�����ʽ��')'
    //              ����������
    //              | ���ַ���
    //              �����з���ֵ����������䣾
    if (symbol == IDENFR) { // ��ʶ�� �з���ֵ�����������
        pre_read_Symbol(1);
        if (symbol_pre == LPARENT) {    // (
            _function_with_return_call();
        }
        else {
            getsym(yes);
            if (symbol == LBRACK) { // [
                getsym(yes);
                _expression();  // ���ʽ
                if (symbol == RBRACK) { // ]
                    getsym(yes);
                    if (symbol == LBRACK) { // [
                        getsym(yes);
                        _expression();  // ���ʽ
                        if (symbol == RBRACK) { // ]
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
    else if (symbol == LPARENT) {   // (
        getsym(yes);
        _expression();  // ���ʽ
        if (symbol == RPARENT) {    // )
            getsym(yes);
        }
    }
    else if (symbol == PLUS || symbol == MINU || symbol == INTCON) {
        _int(); // ����
    }
    else if (symbol == CHARCON) {
        _char();    // �ַ�
    }
//    fprintf(f_out, "<����>\n");
//    cout << "<����>" << endl;
}

void _var_define_no_initialization() {
    // �����������޳�ʼ����  ::= �����ͱ�ʶ����(����ʶ����
    //                      | ����ʶ����'['���޷���������']'
    //                      | ����ʶ����'['���޷���������']''['���޷���������']')
    //                      {,(����ʶ����
    //                      | ����ʶ����'['���޷���������']'
    //                      | ����ʶ����'['���޷���������']''['���޷���������']' )}
    string name;
    int type;
    if (symbol == INTTK || symbol == CHARTK) { // ���ͱ�ʶ��
        type = (symbol == INTTK) ? 1 : 2;
        do {
            getsym(yes);
            if (symbol == IDENFR) { // ��ʶ��
                name = token;
                getsym(yes);
                if (symbol == LBRACK) { // [
                    getsym(yes);
                    _unsigned_int();    // �޷�������
                    if (symbol == RBRACK) { // ]
                        getsym(yes);
                        if (symbol == LBRACK) { // [
                            getsym(yes);
                            _unsigned_int();    // �޷�������
                            if (symbol == RBRACK) { // ]
                                getsym(yes);
                            }
                        }
                    }
                }
                insert_symbolTable(name, 0, type);
            }
        }   while (symbol == COMMA);    // ;
    }
//    fprintf(f_out, "<���������޳�ʼ��>\n");
//    cout << "<���������޳�ʼ��>" << endl;
}

void _var_define_with_initialization() {
    // ���������弰��ʼ����  ::= �����ͱ�ʶ��������ʶ����=��������
    //                      | �����ͱ�ʶ��������ʶ����'['���޷���������']'='{'��������{,��������}'}'
    //                      |�����ͱ�ʶ��������ʶ����'['���޷���������']''['���޷���������']'=
    //                      '{''{'��������{,��������}'}'{, '{'��������{,��������}'}'}'}'
    int type, value;
    string name;
    if (symbol == INTTK || symbol == CHARTK) {  // ���ͱ�ʶ��
        type = (symbol == INTTK) ? 1 : 2;
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            name = token;
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                value = _const();   // ����
                insert_symbolTable(name, value, type);
            }
            else if (symbol == LBRACK) {    // [
                getsym(yes);
                _unsigned_int();    // �޷�������
                if (symbol == RBRACK) { // ]
                    getsym(yes);
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        while (symbol != SEMICN) {
                            if (LBRACE ==  symbol || RBRACE == symbol || COMMA == symbol) {
                                getsym(yes);
                            } else {
                                _const();
                            }
                        }
                    }
                    else if (symbol == LBRACK) {    // [
                        getsym(yes);
                        _unsigned_int();    // �޷�������
                        if (symbol == RBRACK) { // ]
                            getsym(yes);
                            if (symbol == ASSIGN) { // =
                                getsym(yes);
                                while (symbol != SEMICN) {
                                    if (LBRACE ==  symbol || RBRACE == symbol || COMMA == symbol) {
                                        getsym(yes);
                                    } else {
                                        _const();
                                    }
                                }
                            }
                        }
                        if (symbol == RBRACE) {  // }
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<�������弰��ʼ��>\n");
//    cout << "<�������弰��ʼ��>" << endl;
}

// main
int main() {
    getBuffer_debug();
//    getBuffer();
    getsym(no);
    program();
    return 0;
}






