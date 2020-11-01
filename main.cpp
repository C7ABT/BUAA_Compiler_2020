//
// Created by Conno on 2020/9/29.
//

#include "iostream"
#include "string"
#include "algorithm"
#include "map"
#include <stdio.h>
#include "fstream"
#include "vector"
#include "cstdio"
#include "cstring"
#include "stdlib.h"
#include "cstdlib"
#include "string.h"
using namespace std;
#define MAX 50

string type[MAX] = {
        "IDENFR", "INTCON", "CHARCON", "STRCON",
        "CONSTTK", "INTTK", "CHARTK", "VOIDTK", "MAINTK",
        "IFTK", "ELSETK", "SWITCHTK", "CASETK", "DEFAULTTK",
        "WHILETK", "FORTK", "SCANFTK", "PRINTFTK", "RETURNTK",
        "PLUS", "MINU", "MULT", "DIV",
        "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ",
        "COLON", "ASSIGN",
        "SEMICN", "COMMA",
        "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"
};


char op[MAX] = {
        '+', '-', '*', '/',
        '<', '>', '=', '!',
        ':', ';', ',',
        '(', ')', '[', ']', '{', '}'
};

map<string, string> reservedWords = {
        {"const", "CONSTTK"},
        {"int",   "INTTK"},
        {"char",  "CHARTK"},
        {"void",  "VOIDTK"},
        {"main",  "MAINTK"},
        {"if", "IFTK"},
        {"else", "ELSETK"},
        {"switch", "SWITCHTK"},
        {"case", "CASETK"},
        {"default", "DEFAULTTK"},
        {"while", "WHILETK"},
        {"for", "FORTK"},
        {"scanf", "SCANFTK"},
        {"printf", "PRINTFTK"},
        {"return", "RETURNTK"}
};
int cnt_word;
int line = 0;   // 行数
FILE *f_in, *f_out;
string token;   // 单词
char single_line[1024]; // 存储单行读入文件数据
int nextsym = 1;    // 语法分析当前单词
int func_with_return[10086];    // 有返回值函数
int func_without_return[10086]; // 无返回值函数

struct Word {
    string val; // 内容
    string type; // 种类
    int line;   // 行数
    int grammar_analysis[MAX];   // 语法分析
    int cnt_grammar_analysis;    // 语法分析个数
};
Word word[10086];

struct Symbol_List {
    string val; // 内容
    int depth; // 层数
    int line;   // 行数
    int function; // 函数索引
    int type;   // 类型
    bool isConst; // 常量
};
Symbol_List symbolList[10086];
int top = 0;

struct Error_list {
    char type;
    int line;
};
Error_list error_list[1024];
int err_cnt = 0;

int cnt_function = 0;
struct Parameter_Table {
    int parameter;  // 参数
    int type[100];  // 类型
};
Parameter_Table parameterTable[10086];

bool isPlus(char c) {
    return (c == '+');
}   // +
bool isMinus(char c) {
    return (c == '-');
}   // -
bool isStar(char c) {
    return (c == '*');
}   // *
bool isDiv(char c) {
    return (c == '/');
}      // /
bool isColon(char c) {
    return (c == ':');
}   // :
bool isComma(char c) {
    return (c == ',');
}   // ,
bool isSemicolon(char c) {
    return (c == ';');
}   // ;
bool isEqual(char c) {
    return (c == '=');
}   // =
bool isLParent(char c) {
    return (c == '(');
}   // (
bool isRParent(char c) {
    return (c == ')');
}   // )
bool isLBrack(char c) {
    return (c == '[');
}   // [
bool isRBrack(char c) {
    return (c == ']');
}   // ]
bool isLBrace(char c) {
    return (c == '{');
}   // {
bool isRBrace(char c) {
    return (c == '}');
}   // }
bool isLess(char c) {
    return (c == '<');
}   // <
bool isGreater(char c) {
    return (c == '>');
}   // >
bool isExclamation(char c) {
    return (c == '!');
}   // !
bool isSpace(char c) {
    return (c == ' ');
}   // " "
bool isTab(char c) {
    return (c == '\t' || c == '\r');
}   // \t \r
bool isInvisibleSymbol(char c) {
    return (c > 0 && c <= 32);
}   // Lee Sin
bool isEOF(char c) {
    return (c == EOF);
}   // EOF
bool isNextline(char c) {
    return (c == '\n');
}   // \n
bool isDigit(char c) {
    return (isdigit(c));
}   // 0-9
bool isUnderscore(char c) {
    return (c == '_');
}   // _
bool isSingleQuote(char c) {
    return (c == '\'');
}   // \'
bool isDoubleQuote(char c) {
    return (c == '\"');
}   // \"
bool isLetter(char c) {
    return (isalpha(c) || isUnderscore(c));
}

bool isReserved_Token(string x) {
    string x_temp(x);
    int token_temp_len = x.size();
    for(int i = 0; i < token_temp_len; ++i){
        x.at(i) = tolower(x.at(i));
    }
    return (reservedWords.find(x) != reservedWords.end());
}   // Whether token is legal
void Reserver_Token(int num) {
    if (isReserved_Token(word[num].val)) {
        word[num].type = reservedWords[word[num].val];
    }   else {
        word[num].type = "IDENFR";
    }
}   // which symbol does the token mean
int Reserver_ch(char ch) {
    for (int i = 0; i < 30; ++i) {
        if (ch == op[i]) {
            return i;
        }
    }
    if (isNextline(ch) || isSpace(ch) || ch == '\r') {
        return -7;
    }
    return -1;
}
void error(char c, int line) {
    error_list[++err_cnt].type = c;
    error_list->line = line;
}
void catToken(char ch) {
    token += ch;
}   // Add ch to token
void retract(char ch, int pos) {
    ch = single_line[--pos];
}
void getChar(char ch, int pos) {
    ch = single_line[++pos];
}
int index_single_line = -1;
void clearToken() {
    token.clear();
}
void addword() {
    word[++cnt_word].val = token;
    word[cnt_word].line = line;
}
void getsym() {
    clearToken();
    index_single_line = -1;
    int len_single_line = strlen(single_line);
    char ch = '\0';
    while (index_single_line + 1 < len_single_line) {
        getChar(ch, index_single_line);
        if (isLetter(ch)) {
            while (isDigit(ch) || isLetter(ch)) {
                catToken(ch);
                getChar(ch, index_single_line);
            }
            retract(ch, index_single_line);
            addword();
            Reserver_Token(cnt_word);
        }
        else if (isDigit(ch)) {
            while (isDigit(ch)) {
                catToken(ch);
                getChar(ch, index_single_line);
            }
            retract(ch, index_single_line);
            addword();
            word[cnt_word].type = "INTCON";
        }
        else if (isSingleQuote(ch)) {
            getChar(ch, index_single_line);
            if (!(isLetter(ch) || isDigit(ch)
                || isDiv(ch)) || isStar(ch)
                || isPlus(ch) || isMinus(ch)) {
                error('a', line);   // 非法字符
            }
            catToken(ch);
            addword();
            word[cnt_word].type = "CHARCON";
            if (!isSingleQuote(ch)) {
                getChar(ch, index_single_line);
            }
        }
        else if (isDoubleQuote(ch)) {
            getChar(ch, index_single_line);
            if (isDoubleQuote(ch)) {
                error('a', line);   // 空字符串
            }
            while (!isDoubleQuote(ch)) {
                catToken(ch);
                if (!isSpace(ch) && !isExclamation(ch) && !(ch >= 35 && ch <= 126)) {
                    error('a', line);   // 字符串非法字符
                }
                getChar(ch, line);
            }
            addword();
            word[cnt_word].type = "STRCON";
        }
        else {
            int flag = Reserver_ch(ch);
            if (flag == -7);    // \n \r " "
            else if (flag == -1) {   // 非法字符
                error('a', line);
            }
            else if (isLess(ch)) {  // < <=
                getChar(ch, index_single_line);
                if (isEqual(ch)) {
                    token = "<=";
                    addword();
//                    word[++cnt_word].val = "<=";
//                    word[cnt_word].line = line;
                    word[cnt_word].type = "LEQ";
                }
                else {
                    token = "<";
                    addword();
//                    word[++cnt_word].val = "<";
//                    word[cnt_word].line = line;
                    word[cnt_word].type = "LSS";
                    retract(ch, index_single_line);
                }
            }
            else if (isGreater(ch)) {   // > >=
                getChar(ch, index_single_line);
                if (isEqual(ch)) {
                    token = ">=";
                    addword();
//                    word[++cnt_word].val = ">=";
//                    word[cnt_word].line = line;
                    word[cnt_word].type = "GEQ";
                }
                else {
                    token = ">";
                    addword();
//                    word[++cnt_word].val = ">";
//                    word[cnt_word].line = line;
                    word[cnt_word].type = "GRE";
                    retract(ch, index_single_line);
                }
            }
            else if (isEqual(ch)) { // = ==
                getChar(ch, index_single_line);
                if (isEqual(ch)) {
                    token = "==";
                    addword();
//                    word[++cnt_word].val = "==";
//                    word[cnt_word].line = line;
                    word[cnt_word].type = "EQL";
                }
                else {
                    token = "=";
                    addword();
//                    word[++cnt_word].val = "=";
//                    word[cnt_word].line = line;
                    word[cnt_word].type = "ASSIGN";
                    retract(ch, index_single_line);
                }
            }
            else if (isExclamation(ch)) {
                getChar(ch, index_single_line);
                if (!isExclamation(ch)) {
                    error('a', line);
                }
                token = "!=";
                addword();
//                word[++cnt_word].val = "!=";
//                word[cnt_word].line = line;
                word[cnt_word].type = "NEQ";
            }
            else {
                clearToken();
                catToken(ch);
                addword();
                word[cnt_word].type = type[19 + flag];
            }
        }
    }
}

void wipe(int begin, int end) {
    for (int i = begin; i <= end; ++i) {
        word[i].cnt_grammar_analysis = 0;
    }
}   // 语法分析回退
int search() {
    for (int i = 1; i < top; ++i) {
        if (symbolList[top].val == symbolList[i].val && symbolList[top].depth == symbolList[i].depth) {
            return 1;
        }
    }
    return 0;
}   // 搜索相同符号
int idenfr_not_desined(int num) {
    string temp(word[num].val);
    int temp_len = temp.size();
    for(int i = 0; i < temp_len; ++i){
        temp.at(i) = tolower(temp.at(i));
    }
    for (int i = 1; i <= top; ++i) {
        if (symbolList[i].val == temp) {
            return 0;
        }
    }
    return 1;
}   // 判定标识符是否被定义过
string tolower_string(string x) {
    string temp(x);
    int temp_len = temp.size();
    for(int i = 0; i < temp_len; ++i){
        temp.at(i) = tolower(temp.at(i));
    }
    return temp;
}
int find_const(int num) {
    string temp = tolower_string(word[num].val);
    for (int i = 1; i < top; ++i) {
        if (temp == symbolList[i].val && symbolList[i].isConst == true) {
            return 1;
        }
    }
    return 0;
}   // 判断是否为常量
int s2i(int num) {
    int ans = 0;
    int len = (word[num].val).size();
    for (int i = 0; i < len; ++i) {
        ans = ans * 10 + (word[num].val[i] - '0');
    }
    return ans;
}   // 字符串->整数
bool isType(string s) {
    return word[nextsym].type == s;
}   // word[nextsym].type == s ? true : false

int _char(); // 字符
int _int(); // 整数
int _unsigned_int(); // 无符号整数
int _const_define(int num); // 常量定义
int _const_statement(int num); // 常量说明
int _var_define_no_initialization(int num); // 变量定义无初始化
int _var_define_with_initialization(int num); // 变量定义及初始化
int _const(); // 常量
int _var_define(int num); // 变量定义
int _var_statement(int num); // 变量说明
int _head_statement(); // 声明头部
int _step(); // 步长
int _table_parameter(); // 参数表

void program(); // 程序
void _string(); // 字符串
int _const() {
//    ＜常量＞   ::=  ＜整数＞|＜字符＞
    int start = nextsym;
    if (_int() == 1) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 7;
        return 1;
    }   else if (_char() == 1) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 7;
        return 2;
    }
    wipe(start, nextsym);
    nextsym = start;
    return 0;
} // 常量
int _const_define(int num) {
//    ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
//                     | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
    int start = nextsym;
    int top_origin = top;
    int err_cnt_origin = err_cnt;
    if (word[nextsym].type == "INTTK") {
        nextsym += 1;
        while (word[nextsym].type == "IDENFR") {
            top += 1;
            symbolList[top].val = tolower_string(word[nextsym].val);
            symbolList[top].depth = num;
            symbolList[top].line = word[nextsym].line;
            symbolList[top].type = 1;
            symbolList[top].isConst = true;
            if (search() == 1) {
                error('b', word[nextsym].line);
                symbolList[top].isConst = false;
                top -= 1;
            }
            nextsym += 1;
            if (word[nextsym].type == "ASSIGN") {
                nextsym += 1;
                if (_int() == 1) {
                    if (word[nextsym++].type == "COMMA") { // ,
                        nextsym -= 1;
                        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 3;
                        return 1;
                    }
                }   else {
                    error('0', index_single_line);
                }
            }   else {
                error('0', index_single_line);
            }
        }
    }   else if (word[nextsym].type == "CHARTK") {
        nextsym += 1;
        while (word[nextsym].type == "IDENFR") {
            top += 1;
            symbolList[top].val = tolower_string(word[nextsym].val);
            symbolList[top].depth = num;
            symbolList[top].line = word[nextsym].line;
            symbolList[top].type = 2;
            symbolList[top].isConst = true;
            if (search() == 1) {
                error('b', word[nextsym].line);
                symbolList[top].type = 1;
                symbolList[top].isConst = false;
                top -= 1;
            }
            nextsym += 1;
            if (word[nextsym].type == "ASSIGN") {
                nextsym += 1;
                if (_int() == 1) {
                    if (word[nextsym++].type == "COMMA") { // ,
                        nextsym -= 1;
                        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 3;
                        return 1;
                    }
                }   else {
                    error('0', index_single_line);
                }
            }   else {
                error('0', index_single_line);
            }
        }
    }
    wipe(start, nextsym);
    nextsym = start;
    top = top_origin;
    err_cnt = err_cnt_origin;
    return 0;
} // 常量定义
int _const_statement(int num) {
//    ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
    int start = nextsym;
    int top_origin = top;
    int err_cnt_origin = err_cnt;
    if (word[nextsym].type == "CONSTTK") {
        nextsym += 1;
        while (_const_define(num) == 1) {
            if (word[nextsym].type == "SEMICN") {
                nextsym += 1;
            }   else {
                error('k', word[nextsym - 1].line);
            }
            if (word[nextsym].type != "CONSTTK") {
                word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 2;
                return 1;
            }
            nextsym += 1;
        }
    }
    wipe(start, nextsym);
    nextsym = start;
    top = top_origin;
    err_cnt = err_cnt_origin;
    return 0;
} // 常量说明
int _var_define(int num) {
//    ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
    int start = nextsym;
    if (_var_define_with_initialization(num) == 1) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 9;
        return 1;
    }   else if (_var_define_no_initialization(num) == 1) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 9;
        return 1;
    }
    wipe(start, nextsym);
    nextsym = start;
    return 0;
} // 变量定义
int _var_statement(int num) {
//    ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
    int start = nextsym;
    int start_temp = nextsym;
    int top_origin = top;
    int err_cnt_origin = err_cnt;
    while (_var_define(num) == 1) {
        if (isType("LPARENT")) {    // (
            break;
        }   else if (isType("SEMICN")) {    // ;
            nextsym += 1;
        }   else {
            error('k', word[nextsym - 1].line);
        }
        start = nextsym;
        top_origin = top;
        err_cnt_origin = err_cnt;
    }
    wipe(start, nextsym);
    nextsym = start;
    top = top_origin;
    err_cnt = err_cnt_origin;
    if (nextsym != start_temp) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 8;
        return 1;
    }
    return 0;
} // 变量说明
int _var_define_no_initialization(int num) {
//    ＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞
//                          |＜标识符＞'['＜无符号整数＞']'
//                          |＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']')
//                          {,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
    int start = nextsym;
    int top_origin = top;
    int err_cnt_origin = err_cnt;
    int type_temp = 0;
    if (word[nextsym].type == "INTTK" || word[nextsym].type == "CHARTK") {
        type_temp = (word[nextsym].type == "INTTK") ? 1 : 2;
        nextsym += 1;
        while (word[nextsym].type == "IDENFR") {
            top += 1;
            symbolList[top].val = tolower_string(word[nextsym].val);
            symbolList[top].depth = num;
            symbolList[top].line = word[nextsym].line;
            symbolList[top].type = type_temp;
            if (search() == 1) {
                error('b', word[nextsym].line);
                symbolList[top].type = 1;
                top -= 1;
            }
            nextsym += 1;
            if (word[nextsym].type == "LBRACK") {   // [
                nextsym += 1;
                if (_unsigned_int() == 1) {
                    if (word[nextsym].type == "RBRACK") {   // ]
                        nextsym += 1;
                    }   else {
                        error('m', word[nextsym - 1].line);
                    }
                    if (word[nextsym].type == "LBRACK") {   // [
                        nextsym += 1;
                        if (_unsigned_int() == 1) {
                            if (word[nextsym].type == "RBRACK") {   // ]
                                nextsym += 1;
                            } else {
                                error('m', word[nextsym - 1].line);
                            }
                            if (word[nextsym].type == "COMMA") {
                                nextsym += 1;
                            }   else {
                                word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 10;
                                return 1;
                            }
                        }   else {
                            error('0', index_single_line);
                        }
                    }   else if (word[nextsym].type == "COMMA") {
                        nextsym += 1;
                    }   else {
                        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 10;
                        return 1;
                    }
                }   else {
                    error('0', index_single_line);
                }
            }   else if (word[nextsym].type == "COMMA") {
                nextsym += 1;
            }   else {
                word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 10;
                return 1;
            }
        }
    }
    wipe(start, nextsym);
    nextsym = start;
    err_cnt = err_cnt_origin;
    top = top_origin;
    return 0;
} // 变量定义无初始化
int _var_define_with_initialization(int num) {
//    ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞
//                          |＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
//                          |＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='
//                          {''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
    int start = nextsym;
    int top_origin = top;
    int err_cnt_origin = err_cnt;
    int type_temp = 0;
    int count_1 = 0;    // 一维数组
    int count_2 = 0;    // 二维数组
    int type_const; // 常量类型
    if (word[nextsym].type == "INTTK" || word[nextsym].type == "CHARTK") {
        type_temp = (isType("INTTK")) ? 1 : 2;
        nextsym += 1;
        if (isType("IDENFR")) {
            top += 1;
            symbolList[top].val = tolower_string(word[nextsym].val);
            symbolList[top].depth = num;
            symbolList[top].line = word[nextsym].line;
            symbolList[top].type = type_temp;
            if (search() == 1) {
                error('b', word[nextsym].line);
                symbolList[top].type = 1;
                top -= 1;
            }
            nextsym += 1;
            if (isType("LBRACK")) {   // [
                nextsym += 1;
                if (_unsigned_int() == 1) {
                    count_1 = s2i(nextsym - 1);
                    if (isType("RBRACK")) {   // ]
                        nextsym += 1;
                    }   else {
                        error('m', word[nextsym - 1].line);
                    }
                    if (isType("LBRACK")) {   // [
                        nextsym += 1;
                        if (_unsigned_int() == 1) {
                            count_2 = s2i(nextsym - 1);
                            if (isType("RBRACK")) {   // ]
                                nextsym += 1;
                            }   else {
                                error('m', word[nextsym - 1].line);
                            }
                            if (isType("ASSIGN")) {   // =
                                nextsym += 1;
                                if (isType("LBRACE")) { // {
                                    nextsym += 1;
                                    while (isType("LBRACE")) { // {
                                        count_1 -= 1;
                                        nextsym += 1;
                                        while (type_const = _const()) {
                                            if (type_const != type_temp) {
                                                error('o', word[nextsym - 1].line);
                                            }
                                            count_2 -= 1;
                                            if (isType("COMMA")) {
                                                nextsym += 1;
                                            }   else {
                                                break;
                                            }
                                        }
                                        if (isType("RBRACE")) { // }
                                            if (count_2) {
                                                error('n', word[nextsym].line);
                                            }
                                            nextsym += 1;
                                            if (isType("COMMA")) {
                                                nextsym += 1;
                                            }   else if (isType("RBRACE")) {
                                                if (count_1) {
                                                    error('n', word[nextsym].line);
                                                    nextsym += 1;
                                                    word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 11;
                                                    return 1;
                                                }
                                            }   else {
                                                error('0', index_single_line);
                                            }
                                        }
                                    }
                                }   else {
                                    error('0', index_single_line);
                                }
                            }   else {
                                error('0', index_single_line);
                            }
                        }
                    }
                }
            }   else if (isType("ASSIGN")) {
                nextsym += 1;
                type_const = _const();
                if (type_const != type_temp) {
                    error('o', word[nextsym - 1].line);
                }
                if (type_const) {
                    word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 11;
                    return 1;
                }   else {
                    error('0', index_single_line);
                }
            }   else {
                error('0', index_single_line);
            }
        }   else {
            error('0', index_single_line);
        }
    }
    wipe(start, nextsym);
    nextsym = start;
    err_cnt = err_cnt_origin;
    top = top_origin;
    return 0;
} // 变量定义及初始化
void scanf(); // 读语句
void _printf(); // 写语句
void _term(); // 项
void _factor(); // 因子
void _expression(); // 表达式
void _statement(); // 语句
void _default(); // 缺省
void _return(); // 返回语句
int _unsigned_int() {
//    ＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
    if (word[nextsym].type == "INTCON") {
        word[nextsym].grammar_analysis[++word[nextsym].cnt_grammar_analysis] = 4;
        nextsym += 1;
        return 1;
    }
    return 0;
} // 无符号整数
int _step() {
//    ＜步长＞::= ＜无符号整数＞
    if (_unsigned_int() == 1) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 25;
        return 1;
    }
    return 0;
} // 步长
void _main(); // 主函数
int _head_statement() {
//    ＜声明头部＞   ::=  int＜标识符＞
//                     |char＜标识符＞
    int start = nextsym;
    int top_origin = top;
    int err_cnt_origin = err_cnt;
    int type_temp = 0;
    if (isType("INTTK") || isType("CHARTK")) {
        type_temp = isType("INTTK") ? 1 : 2;
        nextsym += 1;
        if (isType("IDENFR")) {
            top += 1;
            symbolList[top].val = tolower_string(word[nextsym].val);
            symbolList[top].depth = 1;
            symbolList[top].line = word[nextsym].line;
            symbolList[top].function == ++cnt_function;
            symbolList[top].type = type_temp;
            if (search() == 1) {
                error('b', word[nextsym].line);
                symbolList[top].type = 1;
                top -= 1;
                cnt_function -= 1;
            }
            func_with_return[nextsym] = 1;
            word[nextsym].grammar_analysis[++word[nextsym].cnt_grammar_analysis] = 6;
            nextsym += 1;
            return 1;
        }   else {
            error('0', index_single_line);
        }
    }   else {
        error('0', index_single_line);
    }
    wipe(start, nextsym);
    nextsym = start;
    err_cnt = err_cnt_origin;
    top = top_origin;
    return 0;
} // 声明头部
int _int() {
//    ＜整数＞        ::= ［＋｜－］＜无符号整数＞
    int start = nextsym;
    if (word[nextsym].type == "MINU" || word[nextsym].type == "PLUS") {
        nextsym += 1;
    }
    if (_unsigned_int() != 0) {
        word[nextsym - 1].grammar_analysis[++word[nextsym - 1].cnt_grammar_analysis] = 5;
        return 1;
    }
    wipe(start, nextsym);
    nextsym = start;
    return 0;
} // 整数
void _assign(); // 赋值语句
void _switch(); // 情况语句
void _case(); // 情况子语句
void _table_cases(); // 情况表
void _function_no_return_define(); // 无返回值函数定义
void _function_with_return_define(); // 有返回值函数定义
void _function_no_return_call(); // 无返回值函数调用语句
void _function_with_return_call(); // 有返回值函数调用语句
void _loop(); // 循环语句
int _table_parameter() {
//    ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}
//                   | ＜空＞

} // 参数表
void _table_parameter_value(); // 值参数表
void _condition(); // 条件
void _if(); // 条件语句
void _list_statement(); // 语句列
void _statement_combination(); // 复合语句
int _char() {
//    ＜字符＞    ::=  '＜加法运算符＞'
//                  ｜'＜乘法运算符＞'
//                  ｜'＜字母＞'
//                  ｜'＜数字＞'
    if (word[nextsym].type == "CHARCON") {
        nextsym += 1;
        return 1;
    }
    return 0;
} // 字符