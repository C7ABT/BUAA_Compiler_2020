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


//  语法分析
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
char* Int2String(int num,char *str)//10进制
{
    int i = 0;//指示填充str
    if(num<0)//如果num为负数，将num变正
    {
        num = -num;
        str[i++] = '-';
    }
    //转换
    do
    {
        str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
        num /= 10;//去掉最低位
    }while(num);//num不为0继续循环

    str[i] = '\0';

    //确定开始调整的位置
    int j = 0;
    if(str[0]=='-')//如果有负号，负号不用调整
    {
        j = 1;//从第二位开始调整
        ++i;//由于有负号，所以交换的对称轴也要后移1位
    }
    //对称交换
    for(;j<i/2;j++)
    {
        //对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b;
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    }

    return str;//返回转换后的值
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
    // 符号表
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

char caculator[10000];   // 存储表达式
int ALU (char s[]) {
    int len = strlen(s);
    int j = 0;
    int i = 0;
    for(i=0; i<len-1; i++) { //处理++  +-  -+  --
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
            else if (s[i + 1] == '-') {  //遇到了--的情况
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
// 词法分析

void program(); // 程序
void _string(); // 字符串
int _const(); // 常量
void _const_define(); // 常量定义
void _const_statement(); // 常量说明
void _var_define(); // 变量定义
void _var_statement(); // 变量说明
void _var_define_no_initialization(); // 变量定义无初始化
void _var_define_with_initialization(); // 变量定义及初始化
void _scanf(); // 读语句
void _printf(); // 写语句
void _term(); // 项
void _factor(); // 因子
int _expression(); // 表达式
void _statement(); // 语句
void _default(); // 缺省
void _return(); // 返回语句
int _unsigned_int(); // 无符号整数
void _step(); // 步长
void _main(); // 主函数
void _head_statement(); // 声明头部
int _int(); // 整数
void _assign(); // 赋值语句
void _switch(); // 情况语句
void _case(); // 情况子语句
void _table_cases(); // 情况表
void _function_no_return_define(); // 无返回值函数定义
void _function_with_return_define(); // 有返回值函数定义
void _function_no_return_call(); // 无返回值函数调用语句
void _function_with_return_call(); // 有返回值函数调用语句
void _loop(); // 循环语句
void _table_parameter(); // 参数表
void _table_parameter_value(); // 值参数表
void _condition(); // 条件
void _if(); // 条件语句
void _list_statement(); // 语句列
void _statement_combination(); // 复合语句
int _char(); // 字符

// 老子是分割线 //
SymbolType symbol_pre;
string array_function_with_return[1000]; // 有返回值函数名
string array_function_no_return[1000];    // 无返回值函数名
int index_array_function_with_return;
int index_array_function_no_return;
string token_pre;
void pre_read_Symbol(int n){
    // 预读
    SymbolType symbol_origin = symbol;
    string token_origin = token;
    int index_buffer_origin = index_buffer;
    while (n--) {
        getsym(no);
    }
    token_pre = token;
    symbol_pre = symbol;    // 预读得到的symbol
    symbol = symbol_origin; // 还原symbol
    token = token_origin;   // 还原token
    index_buffer = index_buffer_origin; // 还原指针
} // 预读

// 老子也是分割线 //

void program() {
    /* ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞
     *              | ＜无返回值函数定义＞}＜主函数＞
     */
    if (symbol == CONSTTK) {
        _const_statement(); // 常量说明
    }
    pre_read_Symbol(2);
    if (symbol_pre != LPARENT) {
        _var_statement();   // 变量说明
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
//    fprintf(f_out, "<程序>\n");
//    cout << "<程序>" << endl;
}

void _const_statement() {
    // ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
    if (symbol == CONSTTK) {
        while (symbol == CONSTTK) {
            getsym(yes);
            _const_define(); // 常量定义
            if (symbol == SEMICN) { // ;
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<常量说明>\n");
//    cout << "<常量说明>" << endl;
}

////////////////////
void _var_statement() {
    // ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
    do {
        _var_define();  // 变量定义
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
//    fprintf(f_out, "<变量说明>\n");
//    cout << "<变量说明>" << endl;
}
////////////////////

void _const_define() {
    // ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
    //                  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
    int type, value;
    string name;
    if (symbol == INTTK) { // int
        type = 1;
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            name = token;
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                value = _int(); // 整数
                insert_symbolTable(name, value, type);
                while (symbol == COMMA) { // ,
                    type = 1;
                    getsym(yes);
                    if (symbol == IDENFR) { // 标识符
                        name = token;
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        value = _int(); // 整数
                        insert_symbolTable(name, value, type);
                    }
                }
            }
        }
    }

    else if (symbol == CHARTK) { // char
        getsym(yes);
        type = 2;
        if (symbol == IDENFR) { // 标识符
            name = token;
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                value = _char(); // 字符
                insert_symbolTable(name, value, type);
                while (symbol == COMMA) { // ,
                    type = 2;
                    getsym(yes);
                    if (symbol == IDENFR) { // 标识符
                        name = token;
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        value = _char(); // 字符
                        insert_symbolTable(name, value, type);
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<常量定义>\n");
//    cout << "<常量定义>" << endl;
}

void _function_with_return_define() {
    // ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
    _head_statement();  // 声明头部
    if (symbol == LPARENT) { // (
        getsym(yes);
        _table_parameter(); // 参数表
        if (symbol == RPARENT) { // )
            getsym(yes);
            if (symbol == LBRACE) { // {
                getsym(yes);
                _statement_combination(); // 复合语句
                if (symbol == RBRACE) { // }
                    getsym(yes);
                }
            }
        }
    }
//    fprintf(f_out, "<有返回值函数定义>\n");
//    cout << "<有返回值函数定义>" << endl;
}

void _head_statement() {
    // ＜声明头部＞   ::=  int＜标识符＞
    //                |  char＜标识符＞
    if (symbol == CHARTK || symbol == INTTK) { // int || char
        getsym(yes);
        array_function_with_return[index_array_function_with_return++] = token;
        if (symbol == IDENFR) { // 标识符
            getsym(yes);
        }
    }
//    fprintf(f_out, "<声明头部>\n");
//    cout << "<声明头部>" << endl;
}

void _table_parameter() {
    // ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}
    //               |  ＜空＞
    if (symbol == INTTK || symbol == CHARTK) { // 类型标识符
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            getsym(yes);
        }
        if (symbol == COMMA) { // ,
            while (symbol == COMMA) { // ,
                getsym(yes);
                if (symbol == INTTK || symbol == CHARTK) { // 类型标识符
                    getsym(yes);
                    if (symbol == IDENFR) { // 标识符
                        getsym(yes);
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<参数表>\n");
//    cout << "<参数表>" << endl;
}

void _statement_combination() {
    // ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
    if (symbol == CONSTTK) { // 常量说明
        _const_statement();
    }
    if (symbol == INTTK || symbol == CHARTK) { // 变量说明
        _var_statement();
    }
    _list_statement();  // 语句列
//    fprintf(f_out, "<复合语句>\n");
//    cout << "<复合语句>" << endl;
}

void _function_no_return_define() {
    // ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
    if (symbol == VOIDTK) { // void
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            array_function_no_return[index_array_function_no_return++] = token;
            getsym(yes);
            if (symbol == LPARENT) { // (
                getsym(yes);
                _table_parameter(); // 参数表
                if (symbol == RPARENT) { // )
                    getsym(yes);
                    if (symbol == LBRACE) { // {
                        getsym(yes);
                        _statement_combination(); // 复合语句
                        if (symbol == RBRACE) { // }
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<无返回值函数定义>\n");
//    cout << "<无返回值函数定义>" << endl;
}

void _main() {
    // ＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
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
                        _statement_combination(); // 复合语句
                        if (symbol == RBRACE) { // }
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<主函数>\n");
//    cout << "<主函数>" << endl;
}

void _list_statement() {
    // ＜语句列＞   ::= ｛＜语句＞｝
    while (symbol == IFTK || symbol == WHILETK || symbol == FORTK || symbol == LBRACE
           || symbol == IDENFR || symbol == SCANFTK || symbol == PRINTFTK || symbol == SEMICN
           || symbol == RETURNTK || symbol == SWITCHTK ) { // if while for ( 标识符 scanf printf ; return switch
        _statement(); // 语句
    }
//    fprintf(f_out, "<语句列>\n");
//    cout << "<语句列>" << endl;
}

void _if() {
    // ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
    if (symbol == IFTK) { // if
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _condition(); // 条件
            if (symbol == RPARENT) { // )
                getsym(yes);
                _statement(); // 语句
                if (symbol == ELSETK) { // else
                    getsym(yes);
                    _statement(); // 语句
                }
            }
        }
    }
//    fprintf(f_out, "<条件语句>\n");
//    cout << "<条件语句>" << endl;
}

void _condition() {
    // ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
    _expression(); // 表达式
    if (symbol == LSS || symbol == LEQ
        || symbol == GRE || symbol == GEQ
        || symbol == NEQ || symbol == EQL) {
        // < <= > >= != ==
        getsym(yes);
        _expression(); // 表达式
    }
//    fprintf(f_out, "<条件>\n");
//    cout << "<条件>" << endl;
}

int _expression() {
    //  ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    int i = 0;
    int res = 0;
    int flag = 0;  // 是否来自赋值语句, printf语句需要在开头添加(，同时结尾需要保留)
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
        }   // printf 单个字符
        else if (symbol_pre == SEMICN || symbol_pre == COMMA) {
            res = token[0];
            flag_expression = 2;
            getsym(no);
            return res;
        }   // 赋值 单个字符
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
        }   // printf 数字
        else if (symbol_pre == SEMICN || symbol_pre == COMMA) {
            res = atoi(token.c_str());
            flag_expression = 1;
            getsym(no);
            return res;
        }   // 赋值 数字
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
            }   // printf int型标识符
            else if (symbolTable[pos].type == 2) {
                res = symbolTable[pos].value;
                flag_expression = 2;
                getsym(no);
                return res;
            }   // printf char型标识符
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
            }   // 赋值 int型标识符
            else if (symbolTable[pos].type == 2) {
                res = symbolTable[pos].value;
                flag_expression = 2;
                getsym(no);
                return res;
            }   // 赋值 char型标识符
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
//            }   // printf 标识符 char型
//            else {
//                flag_expression = 1;
//                // printf 标识符 int型
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
                    // 赋值语句 末尾添加)
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
    if (symbol == PLUS || symbol == MINU) { // 加法运算符
        getsym(yes);
    }
    _term(); // 项
    while (symbol == PLUS || symbol == MINU) { // 加法运算符
        getsym(yes);
        _term(); // 项
    }
    return res;
//    fprintf(f_out, "<表达式>\n");
//    cout << "<表达式>" << endl;
}

void _term() {
    // ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
    _factor(); // 因子
    while (symbol == MULT || symbol == DIV) { // 乘法运算符
        getsym(yes);
        _factor();  // 因子
    }
//    fprintf(f_out, "<项>\n");
//    cout << "<项>" << endl;
}

void _loop() {
    // ＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞
    // | for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
    if (symbol == WHILETK) { // while
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _condition();   // 条件
            if (symbol == RPARENT) { // )
                getsym(yes);
                _statement();   // 语句
            }
        }
    }

    else if (symbol == FORTK) { // for
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == IDENFR) { // 标识符
                getsym(yes);
                if (symbol == ASSIGN) { // =
                    getsym(yes);
                    _expression(); // 表达式
                    if (symbol == SEMICN) { // ;
                        getsym(yes);
                        _condition(); // 条件
                        if (symbol == SEMICN) { // ;
                            getsym(yes);
                            if (symbol == IDENFR) { // 标识符
                                getsym(yes);
                                if (symbol == ASSIGN) { // =
                                    getsym(yes);
                                    if (symbol == IDENFR) { // 标识符
                                        getsym(yes);
                                        if (symbol == PLUS || symbol == MINU) { // + || -
                                            getsym(yes);
                                            _step();    // 步长
                                            if (symbol == RPARENT) { // )
                                                getsym(yes);
                                                _statement(); // 语句
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
//    fprintf(f_out, "<循环语句>\n");
//    cout << "<循环语句>" << endl;
}

void _string() {
    // ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
    if (symbol == STRCON) { // 字符串
        getsym(yes);
    }
//    fprintf(f_out, "<字符串>\n");
//    cout << "<字符串>" << endl;
}

void _step() {
    // ＜步长＞::= ＜无符号整数＞
    _unsigned_int(); // 无符号整数
//    fprintf(f_out, "<步长>\n");
//    cout << "<步长>" << endl;
}

int _unsigned_int() {
    // ＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
    if (symbol == INTCON) {
//        int res = stoi(token);
        int res = atoi(token.c_str());
        getsym(yes);
        return res;
    }
//    fprintf(f_out, "<无符号整数>\n");
//    cout << "<无符号整数>" << endl;
    return 0;
}

int _int() {
    // ＜整数＞        ::= ［＋｜－］＜无符号整数＞
    int flag = 0;   // 1: 负数
    if (symbol == PLUS || symbol == MINU) {
        flag = (symbol == MINU) ? 1 : 0;
        getsym(yes);
    }
    if (flag == 1) {
        return _unsigned_int() * (-1); // 负数
    }
    return _unsigned_int(); // 非负数
//    fprintf(f_out, "<整数>\n");
//    cout << "<整数>" << endl;
}

int _char() {
    int res;
    if (symbol == CHARCON) { // 字符
        res = (char)token[0];
        getsym(yes);
        return res;
    }
    return 0;
}

void _scanf() {
    // ＜读语句＞    ::=  scanf '('＜标识符＞')'
    if (symbol == SCANFTK) { // scanf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == IDENFR) { // 标识符
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
//    fprintf(f_out, "<读语句>\n");
//    cout << "<读语句>" << endl;
}

void _printf() {
    // ＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'
    //              |  printf '('＜字符串＞ ')'
    //              | printf '('＜表达式＞')'
    int value;
    if (symbol == PRINTFTK) { // printf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == STRCON) { // 字符串
                fprintf(f_out, "%s", token.c_str());
                printf("%s", token.c_str());
                _string();  // 字符串
                if (symbol == COMMA) { // ,
                    getsym(yes);
                    value = _expression();  // 表达式
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
                value = _expression();  // 表达式
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
//    fprintf(f_out, "<写语句>\n");
//    cout << "<写语句>" << endl;
}

void _return() {
    // ＜返回语句＞   ::=  return['('＜表达式＞')']
    if (symbol == RETURNTK) { // return
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _expression();  // 表达式
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<返回语句>\n");
//    cout << "<返回语句>" << endl;
}

void _function_with_return_call() {
    // ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
    if (symbol == IDENFR) { // 标识符
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _table_parameter_value(); // 值参数表
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<有返回值函数调用语句>\n");
//    cout << "<有返回值函数调用语句>" << endl;
}

void _function_no_return_call() {
    // ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
    if (symbol == IDENFR) { // 标识符
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _table_parameter_value(); // 值参数表
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
//    fprintf(f_out, "<无返回值函数调用语句>\n");
//    cout << "<无返回值函数调用语句>" << endl;
}

void _table_parameter_value() {
    // ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}
    //              ｜   ＜空＞
    if (symbol == RPARENT) { // 空
//        fprintf(f_out, "<值参数表>\n");
//        cout << "<值参数表>" << endl;
    }
    else {
        _expression();  // 表达式
        while (symbol == COMMA) { // ,
            getsym(yes);
            _expression();  // 表达式
        }
//        fprintf(f_out, "<值参数表>\n");
//        cout << "<值参数表>" << endl;
    }
}

void _statement() {
    // ＜语句＞    ::= ＜循环语句＞
    //              ｜＜条件语句＞
    //              | ＜有返回值函数调用语句＞;
    //              | ＜无返回值函数调用语句＞;
    //              ｜＜赋值语句＞;
    //              ｜＜读语句＞;
    //              ｜＜写语句＞;
    //              ｜＜情况语句＞
    //              ｜＜空＞;
    //              |＜返回语句＞;
    //              | '{'＜语句列＞'}'
    if (symbol == SWITCHTK) {   // 情况语句
        _switch();  // 情况语句
    }
    else if (symbol == IFTK) {   // 条件语句
        _if();  // 条件语句
    }
    else if (symbol == WHILETK || symbol == FORTK) {    // 循环语句
        _loop();    // 循环语句
    }
    else if (symbol == LBRACE) { // {语句列}
        getsym(yes);
        _list_statement();  // 语句列
        if (symbol == RBRACE) { // }
            getsym(yes);
        }
    }
    else if (symbol == IDENFR) {    // 赋值语句 有无返回值函数调用语句
        pre_read_Symbol(1);
        if (symbol_pre == LPARENT) { // (
            // 有无返回值函数调用语句
            for (int i = 0; i < index_array_function_with_return; ++i) {
                if (array_function_with_return[i] == token) {
                    _function_with_return_call(); // 有返回值函数调用语句
                    break;
                }
            }
            for (int i = 0; i < index_array_function_no_return; ++i) {
                if (array_function_no_return[i] == token) {
                    _function_no_return_call(); // 无返回值函数调用语句
                    break;
                }
            }
            if (symbol == SEMICN) { // ;
                getsym(yes);
            }
        }
        else {
            // 赋值语句
            _assign();
            if (symbol == SEMICN) { // ;
                getsym(yes);
            }
        }
    }
    else if (symbol == SEMICN) { // ;
        // 空
        getsym(yes);
    }
    else if (symbol == PRINTFTK) {
        // 写语句
        _printf();
        if (symbol == SEMICN) { // ;
            getsym(yes);
        }
    }
    else if (symbol == SCANFTK) {
        // 读语句
        _scanf();
        if (symbol == SEMICN) { // ;
            getsym(yes);
        }
    }
    else if (symbol == RETURNTK) {
        // 返回语句
        _return();
        if (symbol == SEMICN) {
            getsym(yes);
        }
    }
//    fprintf(f_out, "<语句>\n");
//    cout << "<语句>" << endl;
}

void _default() {
    // ＜缺省＞   ::=  default :＜语句＞
    if (symbol == DEFAULTTK) { // default
        getsym(yes);
        if (symbol == COLON) { // :
            getsym(yes);
            _statement();   // 语句
        }
    }
//    fprintf(f_out, "<缺省>\n");
//    cout << "<缺省>" << endl;
}

void _switch() {
    // ＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
    if (symbol == SWITCHTK) { // switch
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            _expression();   // 表达式
            if (symbol == RPARENT) { // )
                getsym(yes);
                {
                    if (symbol == LBRACE) { // {
                        getsym(yes);
                        _table_cases();  // 情况表
                        _default();  // 缺省
                        if (symbol == RBRACE) {
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<情况语句>\n");
//    cout << "<情况语句>" << endl;
}

void _case() {
    // ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
    if (symbol == CASETK) {  // case
        getsym(yes);
        _const();    // 常量
        if (symbol == COLON) {   // :
            getsym(yes);
            _statement();    // 语句
        }
    }
//    fprintf(f_out, "<情况子语句>\n");
//    cout << "<情况子语句>" << endl;
}

void _table_cases() {
    // ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    while (symbol == CASETK) {
        _case();
    }
//    fprintf(f_out, "<情况表>\n");
//    cout << "<情况表>" << endl;
}

int _const() {
    // ＜常量＞   ::=  ＜整数＞
    //             |  ＜字符＞
    if (symbol == INTCON || symbol == PLUS || symbol == MINU) { // 整数
        return _int(); // 整数
    }
    else if (symbol == CHARCON) { // 字符
        return _char();    // 字符
    }
    return 0;
//    fprintf(f_out, "<常量>\n");
//    cout << "<常量>" << endl;
}

void _var_define() {
    // ＜变量定义＞ ::= ＜变量定义无初始化＞
    //              | ＜变量定义及初始化＞

    // ＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞
    //                      | ＜标识符＞'['＜无符号整数＞']'
    //                      | ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'){,(＜标识符＞
    //                      | ＜标识符＞'['＜无符号整数＞']'
    //                      | ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}

    // ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞
    //                      | ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
    //                      | ＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='
    //                      {''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'

    if (symbol == INTTK || symbol == CHARTK) { // 类型标识符
        int i = 1;
        while (symbol_pre != COMMA && symbol_pre != SEMICN && symbol_pre != ASSIGN) {
            pre_read_Symbol(i++) ;
        }
        if (symbol_pre == SEMICN || symbol_pre == COMMA) { // ; ,
            _var_define_no_initialization();    // 变量定义无初始化
        }
        else {
            _var_define_with_initialization();  // 变量定义及初始化
        }
    }
//    fprintf(f_out, "<变量定义>\n");
//    cout << "<变量定义>" << endl;
}

void _assign() {
    // ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞
    //                |  ＜标识符＞'['＜表达式＞']'=＜表达式＞
    //                |  ＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
    int value;
    string name;
    int index;
    if (symbol == IDENFR) { // 标识符
        index = find_symbolTable(token);
        getsym(yes);
//        if (symbol == ASSIGN) { // =
//            getsym(yes);
//            value = _expression();  // 表达式
//            symbolTable[index].value = value;
//        }
        // = 放进表达式中处理，分清表达式来源是赋值语句还是printf
        value = _expression();  // 表达式
        symbolTable[index].value = value;
        if (symbol == LBRACK) {    // [
            getsym(yes);
            _expression();  // 表达式
            if (symbol == RBRACK) { // ]
                getsym(yes);
                if (symbol == ASSIGN) { // =
                    getsym(yes);
                    _expression(); // 表达式
                }
                else if (symbol == LBRACK) {    // [
                    getsym(yes);
                    _expression();  // 表达式
                    if (symbol == RBRACK) { // ]
                        getsym(yes);
                        if (symbol == ASSIGN) { // =
                            getsym(yes);
                            _expression();  // 表达式
                        }
                    }
                }
            }
        }
    }
//    fprintf(f_out, "<赋值语句>\n");
//    cout << "<赋值语句>" << endl;
}

void _factor() {
    // ＜因子＞    ::= ＜标识符＞
    //              ｜＜标识符＞'['＜表达式＞']'
    //              | ＜标识符＞'['＜表达式＞']''['＜表达式＞']'
    //              |'('＜表达式＞')'
    //              ｜＜整数＞
    //              | ＜字符＞
    //              ｜＜有返回值函数调用语句＞
    if (symbol == IDENFR) { // 标识符 有返回值函数调用语句
        pre_read_Symbol(1);
        if (symbol_pre == LPARENT) {    // (
            _function_with_return_call();
        }
        else {
            getsym(yes);
            if (symbol == LBRACK) { // [
                getsym(yes);
                _expression();  // 表达式
                if (symbol == RBRACK) { // ]
                    getsym(yes);
                    if (symbol == LBRACK) { // [
                        getsym(yes);
                        _expression();  // 表达式
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
        _expression();  // 表达式
        if (symbol == RPARENT) {    // )
            getsym(yes);
        }
    }
    else if (symbol == PLUS || symbol == MINU || symbol == INTCON) {
        _int(); // 整数
    }
    else if (symbol == CHARCON) {
        _char();    // 字符
    }
//    fprintf(f_out, "<因子>\n");
//    cout << "<因子>" << endl;
}

void _var_define_no_initialization() {
    // ＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞
    //                      | ＜标识符＞'['＜无符号整数＞']'
    //                      | ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']')
    //                      {,(＜标识符＞
    //                      | ＜标识符＞'['＜无符号整数＞']'
    //                      | ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
    string name;
    int type;
    if (symbol == INTTK || symbol == CHARTK) { // 类型标识符
        type = (symbol == INTTK) ? 1 : 2;
        do {
            getsym(yes);
            if (symbol == IDENFR) { // 标识符
                name = token;
                getsym(yes);
                if (symbol == LBRACK) { // [
                    getsym(yes);
                    _unsigned_int();    // 无符号整数
                    if (symbol == RBRACK) { // ]
                        getsym(yes);
                        if (symbol == LBRACK) { // [
                            getsym(yes);
                            _unsigned_int();    // 无符号整数
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
//    fprintf(f_out, "<变量定义无初始化>\n");
//    cout << "<变量定义无初始化>" << endl;
}

void _var_define_with_initialization() {
    // ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞
    //                      | ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
    //                      |＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'=
    //                      '{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
    int type, value;
    string name;
    if (symbol == INTTK || symbol == CHARTK) {  // 类型标识符
        type = (symbol == INTTK) ? 1 : 2;
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            name = token;
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                value = _const();   // 常量
                insert_symbolTable(name, value, type);
            }
            else if (symbol == LBRACK) {    // [
                getsym(yes);
                _unsigned_int();    // 无符号整数
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
                        _unsigned_int();    // 无符号整数
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
//    fprintf(f_out, "<变量定义及初始化>\n");
//    cout << "<变量定义及初始化>" << endl;
}

// main
int main() {
    getBuffer_debug();
//    getBuffer();
    getsym(no);
    program();
    return 0;
}






