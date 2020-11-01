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

struct Error_list {
    char type;
    int line;
};
Error_list error_list[1024];
int err_cnt = 0;

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