//
// Created by Conno on 2020/9/29.
//

#include "iostream"
#include "string"
#include "algorithm"
#include "map"
#include <stdio.h>
#include "fstream"
#include "ctype.h"

#define yes true
#define no false
#define Max 1024
#define Maximum Max * Max
using namespace std;


//  语法分析
int pos_M_code;
int pos_S_list_analysis;
int pos_S_list;
int pos_S_print;
int pos_S_symbol;
int line = 1;
int row = 0;
int pos_temp_has_return;
int pos_has_return;
int pos_no_return;
int func = 0;   // 函数嵌套
string key[Max] = { "const",
                   "int", "char",
                   "void", "main",
                   "if", "else",
                   "switch", "default", "case",
                   "while", "for",
                   "scanf", "printf",
                   "return",
                   "do" };
string keykind[Max] = { "CONSTTK",
                       "INTTK", "CHARTK",
                       "VOIDTK", "MAINTK",
                       "IFTK", "ELSETK",
                       "SWITCHTK", "CASETK", "DEFAULTTK",
                       "WHILETK", "FORTK",
                       "SCANFTK", "PRINTFTK",
                       "RETURNTK",
                       "DOTK" };
string op[Max] = { "+", "-", "*", "/",
                  "<", "<=", ">", ">=", "==", "!=",
                  "=",
                  ";", ", ",
                  "(", ")", "[", "]", "{", "}" };
string opkind[Max] = { "PLUS", "MINU", "MULT", "DIV",
                      "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ",
                      "ASSIGN",
                      "SEMICN", "COMMA",
                      "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE" };
string constkind[Max] = { "IDENFR", "INTCON", "CHARCON", "STRCON" };

// 记录表
struct Sym_list {
    string name[Maximum];
    string type[Maximum];
    string has_return[Maximum];
    string no_return[Maximum];
    int line[Maximum];
    int row[Maximum];
}   S_list;

// 输出表
struct Sym_print {
    string name[Maximum];
    string type[Maximum];
    int num[Maximum]; // 0: 词法, 1: 语法
    int line[Maximum];
    int row[Maximum];
}   S_print;

// 符号表
struct Sym_symbol {
    string name[Maximum];
    int type[Maximum]; // 1: int, 2: char, 3: array, 4: none
    int kind[Maximum]; // 1: const, 2: variable, 3: function_name, 4: function_parameter
    int ref[Maximum];    // 值
    int level[Maximum]; // 层次
    int addr[Maximum]; // 在运行栈中的位置
}   S_symbol;

// 运行栈
struct Sym_stack {
    string name[Maximum];
    string CHAR[Maximum];
    int type[Maximum]; // 1: int, 2: char, 3: array_int, 4: array_char
}   S_stack;

// PCode
struct Mid_code {
    int code[Maximum];
    int op1[Maximum];
    int op2[Maximum];
}   M_code;

void emit (int code, int op1, int op2) {
    M_code.code[pos_M_code] = code;
    M_code.op1[pos_M_code] = op1;
    M_code.op2[pos_M_code] = op2;
    pos_M_code += 1;
}
void emit_op1 (int op1) {
    M_code.op1[pos_M_code] = op1;
    pos_M_code += 1;
}
void emit_op2 (int code, int op2) {
    M_code.code[pos_M_code] = code;
    M_code.op2[pos_M_code] = op2;
    pos_M_code += 1;
}

void analysis_2_slist (Sym_list & slist, string name, string type, int pos) {
    slist.name[pos_S_list_analysis] = name;
    slist.type[pos_S_list_analysis] = type;
    slist.row[pos_S_list_analysis] = row;
    slist.line[pos_S_list_analysis] = line;
    row += pos;
    pos_S_list_analysis += 1;
}

void slist_0_sprint (Sym_list & slist, Sym_print & sprint) {
    sprint.name[pos_S_print] = slist.name[pos_S_list];
    sprint.type[pos_S_print] = slist.type[pos_S_list];
    sprint.line[pos_S_print] = slist.line[pos_S_list];
    sprint.row[pos_S_print] = slist.row[pos_S_list];
    sprint.num[pos_S_print] = 0;
    pos_S_list += 1;
    pos_S_print += 1;
}

void slist_1_sprint (Sym_list & slist, Sym_print & sprint, string name) {
    sprint.num[pos_S_print] = 1;
    sprint.name[pos_S_print] = name;
    pos_S_print += 1;
}

int search (Sym_list & slist, Sym_symbol & symSymbol) {
    for (int z = pos_S_symbol; z >= 0; --z) {
        if (slist.name[pos_S_list] == symSymbol.name[z]) {
            return z;
        }
    }
    return 0;
}

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
    return (c == '\t');
}   // \t
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
bool isLetter(char c) {
    return isalpha(c) || isUnderscore(c);
}   // '_' included
bool isSingleQuote(char c) {
    return (c == '\'');
}   // \'
bool isDoubleQuote(char c) {
    return (c == '\"');
}   // \"

bool isReservedWord(char buffer[], int addr, char c) {
    return tolower(buffer[row + addr]) == c;
}   // 特定位置是否为保留字中的字母或其大写
void retract(int len) {
    pos_S_list -= len;
    pos_S_print -= len;
}   // 回溯 pos_S_list, pos_S_print
bool isParticular_S_List (Sym_list &symList, string s) {
    return symList.type[pos_S_list] == s;
}   // symList.type[pos] == s

// 程序
bool program(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 字符串
bool _string(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 常量
bool _const(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 常量定义
bool _const_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 常量说明
bool _const_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 变量定义
bool _var_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 变量说明
bool _var_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 变量定义无初始化
bool _var_define_no_initialization(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 变量定义及初始化
bool _var_define_with_initialization(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 读语句
bool _scanf(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 写语句
bool _printf(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 项
bool _term(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 因子
bool _factor(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 表达式
bool _expression(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 语句
bool _statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 缺省
bool _default(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 返回语句
bool _return(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 无符号整数
bool _unsigned_int(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 步长
bool _step(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 主函数
bool _main(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 声明头部
int _head_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 整数
bool _int(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 赋值语句
bool _assign(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 情况语句
bool _switch(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 情况子语句
bool _case(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 情况表
bool _table_cases(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 无返回值函数定义
bool _function_no_return_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 有返回值函数定义
bool _function_with_return_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 无返回值函数调用语句
bool _function_no_return_call(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 有返回值函数调用语句
bool _function_with_return_call(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 循环语句
bool _loop(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 参数表
bool _table_parameter(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 值参数表
bool _table_parameter_value(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 条件
bool _condition(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 条件语句
bool _if(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 语句列
bool _list_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 复合语句
bool _statement_combination(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
// 字符
bool _char(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);

void P_code (Sym_stack & symStack, Sym_symbol & symSymbol, Mid_code & midCode);

char buffer[Max];
ifstream f_in;
ofstream f_out;
void getBuffer() {
    f_in.open("../testfile.txt");
    f_out.open("../pcoderesult.txt");
}
void getsym(char buffer[]);
int main() {
    getBuffer();
    while (!f_in.eof()) {
        f_in.getline(buffer, Max);
        row = 0;
        getsym(buffer);
        line += 1;
    }
    program(S_list, S_print, S_stack, S_symbol);
    P_code(S_stack, S_symbol, M_code);

    f_in.close();
    f_out.close();
    return 0;
}
void getsym(char buffer[]) {
    for (row = 0; row < Max;) {
       while (isSpace(buffer[row])) {
           row += 1;
       }
       if (isPlus(buffer[row])) {
           analysis_2_slist(S_list, "+", "PLUS", 1);
       }
       else if (isMinus(buffer[row])) {
           analysis_2_slist(S_list, "-", "MINU", 1);
       }
       else if (isStar(buffer[row])) {
           analysis_2_slist(S_list, "*", "MULT", 1);
       }
       else if (isDiv(buffer[row])) {
           analysis_2_slist(S_list, "/", "DIV", 1);
       }
       else if (isLess(buffer[row]) && !isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, "<", "LSS", 1);
       }
       else if (isLess(buffer[row]) && isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, "<=", "LEQ", 2);
       }
       else if (isGreater(buffer[row]) && !isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, ">", "GRE", 1);
       }
       else if (isGreater(buffer[row]) && isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, ">=", "GEQ", 2);
       }
       else if (isEqual(buffer[row]) && isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, "==", "EQL", 2);
       }
       else if (isExclamation(buffer[row]) && isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, "!=", "NEQ", 2);
       }
       else if (isEqual(buffer[row]) && !isEqual(buffer[row + 1])) {
           analysis_2_slist(S_list, "=", "ASSIGN", 1);
       }
       else if (isSemicolon(buffer[row])) {
           analysis_2_slist(S_list, ";", "SEMICN", 1);
       }
       else if (isComma(buffer[row])) {
           analysis_2_slist(S_list, ",", "COMMA", 1);
       }
       else if (isLParent(buffer[row])) {
           analysis_2_slist(S_list, "(", "LPARENT", 1);
       }
       else if (isRParent(buffer[row])) {
           analysis_2_slist(S_list, ")", "RPARENT", 1);
       }
       else if (isLBrack(buffer[row])) {
           analysis_2_slist(S_list, "[", "LBRACK", 1);
       }
       else if (isRBrack(buffer[row])) {
           analysis_2_slist(S_list, "]", "RBRACK", 1);
       }
       else if (isLBrace(buffer[row])) {
           analysis_2_slist(S_list, "{", "LBRACE", 1);
       }
       else if (isRBrace(buffer[row])) {
           analysis_2_slist(S_list, "}", "RBRACE", 1);
       }
       else if (isReservedWord(buffer, 0, 'c') && isReservedWord(buffer, 1, 'o')
                && isReservedWord(buffer, 2, 'n') && isReservedWord(buffer, 3, 's')
                && isReservedWord(buffer, 4, 't')
                && (isSpace(buffer[row + 5]) || (!isLetter(buffer[row + 5]) && !isDigit(buffer[row + 5])))) {
           analysis_2_slist(S_list, "const", "CONSTTK", 5);
       }
       else if (isReservedWord(buffer, 0, 'i') && isReservedWord(buffer, 1, 'n')
                && isReservedWord(buffer, 2, 't')
                && (isSpace(buffer[row + 3]) || (!isLetter(buffer[row + 3]) && !isDigit(buffer[row + 3])))) {
           analysis_2_slist(S_list, "int", "INTTK", 3);
       }
       else if (isReservedWord(buffer, 0, 'c') && isReservedWord(buffer, 1, 'h')
                && isReservedWord(buffer, 2, 'a') && isReservedWord(buffer, 3, 'r')
                && (isSpace(buffer[row + 4]) || (!isLetter(buffer[row + 4]) && !isDigit(buffer[row + 4])))) {
           analysis_2_slist(S_list, "char", "CHARTK", 4);
       }
       else if (isReservedWord(buffer, 0, 'v') && isReservedWord(buffer, 1, 'o')
                && isReservedWord(buffer, 2, 'i') && isReservedWord(buffer, 3, 'd')
                && (isSpace(buffer[row + 4]) || (!isLetter(buffer[row + 4]) && !isDigit(buffer[row + 4])))) {
           analysis_2_slist(S_list, "void", "VOIDTK", 4);
       }
       else if (isReservedWord(buffer, 0, 'm') && isReservedWord(buffer, 1, 'a')
                && isReservedWord(buffer, 2, 'i') && isReservedWord(buffer, 3, 'n')
                && (isSpace(buffer[row + 4]) || (!isLetter(buffer[row + 4]) && !isDigit(buffer[row + 4])))) {
           analysis_2_slist(S_list, "main", "MAINTK", 4);
       }
       else if (isReservedWord(buffer, 0, 'i') && isReservedWord(buffer, 1, 'f')
                && (isSpace(buffer[row + 2]) || (!isLetter(buffer[row + 2]) && !isDigit(buffer[row + 2])))) {
           analysis_2_slist(S_list, "if", "IFTK", 2);
       }
       else if (isReservedWord(buffer, 0, 'e') && isReservedWord(buffer, 1, 'l')
                && isReservedWord(buffer, 2, 's') && isReservedWord(buffer, 3, 'e')
                && (isSpace(buffer[row + 4]) || (!isLetter(buffer[row + 4]) && !isDigit(buffer[row + 4])))) {
           analysis_2_slist(S_list, "else", "ELSETK", 4);
       }
       else if (isReservedWord(buffer, 0, 's') && isReservedWord(buffer, 1, 'w')
                && isReservedWord(buffer, 2, 'i') && isReservedWord(buffer, 3, 't')
                && isReservedWord(buffer, 4, 'c') && isReservedWord(buffer, 5, 'h')
                && (isSpace(buffer[row + 6]) || (!isLetter(buffer[row + 6]) && !isDigit(buffer[row + 6])))) {
           analysis_2_slist(S_list, "switch", "SWITCHTK", 6);
       }
       else if (isReservedWord(buffer, 0, 'c') && isReservedWord(buffer, 1, 'a')
                && isReservedWord(buffer, 2, 's') && isReservedWord(buffer, 3, 'e')
                && (isSpace(buffer[row + 4]) || (!isLetter(buffer[row + 4]) && !isDigit(buffer[row + 4])))) {
           analysis_2_slist(S_list, "case", "CASETK", 4);
       }
       else if (isReservedWord(buffer, 0, 'd') && isReservedWord(buffer, 1, 'e')
                && isReservedWord(buffer, 2, 'f') && isReservedWord(buffer, 3, 'a')
                && isReservedWord(buffer, 4, 'u') && isReservedWord(buffer, 5, 'l')
                && isReservedWord(buffer, 6, 't')
                && (isSpace(buffer[row + 7]) || (!isLetter(buffer[row + 7]) && !isDigit(buffer[row + 7])))) {
           analysis_2_slist(S_list, "default", "DEFAULTTK", 7);
       }
       else if (isReservedWord(buffer, 0, 'w') && isReservedWord(buffer, 1, 'h')
                && isReservedWord(buffer, 2, 'i') && isReservedWord(buffer, 3, 'l')
                && isReservedWord(buffer, 4, 'e')
                && (isSpace(buffer[row + 5]) || (!isLetter(buffer[row + 5]) && !isDigit(buffer[row + 5])))) {
           analysis_2_slist(S_list, "while", "WHILETK", 5);
       }
       else if (isReservedWord(buffer, 0, 'f') && isReservedWord(buffer, 1, 'o')
                && isReservedWord(buffer, 2, 'r')
                && (isSpace(buffer[row + 3]) || (!isLetter(buffer[row + 3]) && !isDigit(buffer[row + 3])))) {
           analysis_2_slist(S_list, "for", "FORTK", 3);
       }
       else if (isReservedWord(buffer, 0, 's') && isReservedWord(buffer, 1, 'c')
                && isReservedWord(buffer, 2, 'a') && isReservedWord(buffer, 3, 'n')
                && isReservedWord(buffer, 4, 'f')
                && (isSpace(buffer[row + 5]) || (!isLetter(buffer[row + 5]) && !isDigit(buffer[row + 5])))) {
           analysis_2_slist(S_list, "scanf", "SCANFTK", 5);
       }
       else if (isReservedWord(buffer, 0, 'p') && isReservedWord(buffer, 1, 'r')
                && isReservedWord(buffer, 2, 'i') && isReservedWord(buffer, 3, 'n')
                && isReservedWord(buffer, 4, 't') && isReservedWord(buffer, 5, 'f')
                && (isSpace(buffer[row + 6]) || (!isLetter(buffer[row + 6]) && !isDigit(buffer[row + 6])))) {
           analysis_2_slist(S_list, "printf", "PRINTFTK", 6);
       }
       else if (isReservedWord(buffer, 0, 'r') && isReservedWord(buffer, 1, 'e')
                && isReservedWord(buffer, 2, 't') && isReservedWord(buffer, 3, 'u')
                && isReservedWord(buffer, 4, 'r') && isReservedWord(buffer, 5, 'n')
                && (isSpace(buffer[row + 6]) || (!isLetter(buffer[row + 6]) && !isDigit(buffer[row + 6])))) {
           analysis_2_slist(S_list, "return", "RETURNTK", 6);
       }
       //////////////////////// do do do do do do
       else if (isReservedWord(buffer, 0, 'd') && isReservedWord(buffer, 1, 'o')
                && (isSpace(buffer[row + 2]) || (!isLetter(buffer[row + 2]) && !isDigit(buffer[row + 2])))) {
           analysis_2_slist(S_list, "do", "DOTK", 2);
       }
       //////////////////////// do do do do do do
       else if (isDigit(buffer[row])) {
           S_list.type[pos_S_list_analysis] = "INTCON";
           S_list.name[pos_S_list_analysis] = buffer[row];
           S_list.row[pos_S_list_analysis] = row;
           S_list.line[pos_S_list_analysis] = line;
           row += 1;
           while (isDigit(buffer[row])) {
               S_list.name[pos_S_list_analysis] += buffer[row];
               row += 1;
           }
           pos_S_list_analysis += 1;
       }
       else if (isLetter(buffer[row])) {
           S_list.type[pos_S_list_analysis] = "IDENFR";
           S_list.name[pos_S_list_analysis] = buffer[row];
           S_list.row[pos_S_list_analysis] = row;
           S_list.line[pos_S_list_analysis] = line;
           row += 1;
           while (isLetter(buffer[row]) || isDigit(buffer[row])) {
               S_list.name[pos_S_list_analysis] += buffer[row];
               row += 1;
           }
           pos_S_list_analysis += 1;
       }
       else if (isSingleQuote(buffer[row])) {
           row += 1;
           if (isPlus(buffer[row]) || isMinus(buffer[row])
                || isStar(buffer[row]) || isDiv(buffer[row])
                || isDigit(buffer[row]) || isLetter(buffer[row])) {
               row += 1;
               if (isSingleQuote(buffer[row])) {
                   S_list.type[pos_S_list_analysis] = "CHARCON";
                   S_list.name[pos_S_list_analysis] = buffer[row - 1];
                   S_list.row[pos_S_list_analysis] = row;
                   S_list.line[pos_S_list_analysis] = line;
                   row += 1;
                   pos_S_list_analysis += 1;
               }
           }
       }
       else if (isDoubleQuote(buffer[row])) {
           S_list.type[pos_S_list_analysis] = "STRCON";
           row += 1;
           S_list.row[pos_S_list_analysis] = row;
           S_list.line[pos_S_list_analysis] = line;
           while (!isDoubleQuote(buffer[row])) {
               if (isSpace(buffer[row]) || isExclamation(buffer[row])
                    || (buffer[row] >= 35 && buffer[row] <= 126)) {
                   S_list.name[pos_S_list_analysis] += buffer[row];
                   row += 1;
               }
           }
           row += 1;
           pos_S_list_analysis += 1;
       }
       else if (isNextline(buffer[row]) || buffer[row] == '\0' || isTab(buffer[row])
                || (buffer[row] == '\r' && isNextline(buffer[row + 1]))) {
           return;
       }
       else {
           row += 1;
           return;
       }
    }
}

// 老子是分割线 //

bool program(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    /* ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞
     *              | ＜无返回值函数定义＞}＜主函数＞
     */
    int flag = 0;
    if (_const_statement(symList, symPrint, symStack, symSymbol)) {
        flag += 1; // 常量说明
    }
    if (_var_statement(symList, symPrint, symStack, symSymbol)) {
        flag += 1;   // 变量说明
    }
    while (_function_with_return_define(symList, symPrint, symStack, symSymbol)
            || _function_no_return_define(symList, symPrint, symStack, symSymbol)) {
        flag += 1;
    }
    if (_main(symList, symPrint, symStack, symSymbol)) {
        slist_1_sprint(symList, symPrint, "<程序>");
        return true;
    }
    else {
        if (flag) {
            pos_S_list -= flag;
            pos_S_print -= flag;
        }
        return false;
    }
}

bool _const_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
    if (symList.type[pos_S_list] == "CONSTTK") {
        slist_0_sprint(symList, symPrint);
        if (_const_define(symList, symPrint, symStack, symSymbol)) {
            // 常量定义
            if (symList.type[pos_S_list] == ";") { // ;
                slist_0_sprint(symList, symPrint);
                while (symList.type[pos_S_list] == "CONSTTK") {
                    slist_0_sprint(symList, symPrint);
                    if (_const_define(symList, symPrint, symStack, symSymbol)) {
                        if (symList.type[pos_S_list] == ";") {
                            slist_0_sprint(symList, symPrint);
                        }
                        else {
                            retract(2);
                            break;
                        }
                    }
                    else {
                        retract(1);
                        break;
                    }
                }
                slist_1_sprint(symList, symPrint, "<常量说明>");
                return true;
            }
            else {
                retract(2);
                return false;
            }
        }
        else {  // 不是<常量定义>，回退一格，继续判断const
            retract(1);
            return false;
        }
    }
    else {  // 不是const
        return false;
    }
}

bool _var_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
    if (_var_define(symList, symPrint, symStack, symSymbol)) {
        if (symList.type[pos_S_list] == ";") {
            slist_0_sprint(symList, symPrint);
            while (_var_define(symList, symPrint, symStack, symSymbol)) {
                if (symList.type[pos_S_list] == ";") {
                    slist_0_sprint(symList, symPrint);
                }
                else {
                    retract(1);
                    break;
                }
            }
            slist_1_sprint(symList, symPrint, "<变量说明>");
            return true;
        }
        else {
            retract(1);
        }
    }
    else {
        return false;
    }
}

bool _const_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
    //                  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
    if (symList.type[pos_S_list] == "INTTK" && symList.type[pos_S_list + 2] == "ASSIGN") { // int
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "IDENFR") { // 标识符
            symSymbol.name[pos_S_symbol] = symList.name[pos_S_list];
            symSymbol.addr[pos_S_symbol] = 0;
            if (func) {
                symSymbol.level[pos_S_symbol] = 1;
            }
            else {
                symSymbol.level[pos_S_symbol] = 0;
            }
            symSymbol.kind[pos_S_symbol] = 1;
            symSymbol.type[pos_S_symbol] = 1;
            slist_0_sprint(symList, symPrint);
            if (symList.type[pos_S_list] == "ASSIGN") { // =
                slist_0_sprint(symList, symPrint);
                if (_int(symList, symPrint, symStack, symSymbol)) {
                    if (symList.type[pos_S_list - 2] == "PLUS") {
                        symSymbol.ref[pos_S_symbol] = stoi(symList.name[pos_S_list - 1]);
                    }
                    else if (symList.type[pos_S_list - 2] == "MINU") {
                        symSymbol.ref[pos_S_symbol] = -stoi(symList.name[pos_S_list]);
                    }
                    else {
                        symSymbol.ref[pos_S_symbol] = stoi(symList.name[pos_S_list - 1]);
                    }
                    pos_S_symbol += 1;
                    while (symList.type[pos_S_list] == "COMMA") {
                        slist_0_sprint(symList, symPrint);
                        if (symList.type[pos_S_list] == "IDENFR") {
                            symSymbol.name[pos_S_symbol] = symList.name[pos_S_list];
                            symSymbol.addr[pos_S_symbol] = 0;
                            if (func) {
                                symSymbol.level[pos_S_symbol] = 1;
                            }
                            else {
                                symSymbol.level[pos_S_symbol] = 0;
                            }
                            symSymbol.kind[pos_S_symbol] = 1;
                            symSymbol.type[pos_S_symbol] = 1;
                            slist_0_sprint(symList, symPrint);
                            if (symList.type[pos_S_list] == "ASSIGN") {
                                slist_0_sprint(symList, symPrint);
                                if (_int(symList, symPrint, symStack, symSymbol)) {
                                    // 整数
                                    if (symList.type[pos_S_list - 2] == "PLUS") {
                                        symSymbol.ref[pos_S_symbol] = stoi(symList.name[pos_S_list - 1]);
                                    }
                                    else if (symList.type[pos_S_list - 2] == "MINU") {
                                        symSymbol.ref[pos_S_symbol] = -stoi(symList.name[pos_S_list - 1]);
                                    }
                                    else {
                                        symSymbol.ref[pos_S_symbol] = stoi(symList.name[pos_S_list - 1]);
                                    }
                                    pos_S_symbol += 1;
                                }
                                else {
                                    retract(3);
                                    break;
                                }
                            }
                            else {
                                retract(2);
                                break;
                            }
                        }
                        else {
                            retract(1);
                            break;
                        }
                    }
                    slist_1_sprint(symList, symPrint, "<常量定义>");
                    return true;
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    // char
    else if (symList.type[pos_S_list] == "CHARTK" && symList.type[pos_S_list + 2] == "ASSIGN") { // int
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "IDENFR") { // 标识符
            symSymbol.name[pos_S_symbol] = symList.name[pos_S_list];
            symSymbol.addr[pos_S_symbol] = 0;
            if (func) {
                symSymbol.level[pos_S_symbol] = 1;
            }
            else {
                symSymbol.level[pos_S_symbol] = 0;
            }
            symSymbol.kind[pos_S_symbol] = 1;
            symSymbol.type[pos_S_symbol] = 2;
            slist_0_sprint(symList, symPrint);
            if (symList.type[pos_S_list] == "ASSIGN") { // =
                slist_0_sprint(symList, symPrint);
                if (symList.type[pos_S_list] == "CHARTK") {
                    symSymbol.ref[pos_S_symbol] = (int)symList.name[pos_S_list][0];
                    pos_S_symbol += 1;
                    slist_0_sprint(symList, symPrint);
                    while (symList.type[pos_S_list] == "COMMA") {
                        slist_0_sprint(symList, symPrint);
                        if (symList.type[pos_S_list] == "IDENFR") {
                            symSymbol.name[pos_S_symbol] = symList.name[pos_S_list];
                            symSymbol.addr[pos_S_symbol] = 0;
                            if (func) {
                                symSymbol.level[pos_S_symbol] = 1;
                            }
                            else {
                                symSymbol.level[pos_S_symbol] = 0;
                            }
                            symSymbol.kind[pos_S_symbol] = 1;
                            symSymbol.type[pos_S_symbol] = 2;
                            slist_0_sprint(symList, symPrint);
                            if (symList.type[pos_S_list] == "ASSIGN") {
                                slist_0_sprint(symList, symPrint);
                                if (symList.type[pos_S_list] == "CHARTK") {
                                    // 字符
                                    symSymbol.ref[pos_S_symbol] = stoi(symList.name[pos_S_list - 1]);
                                    pos_S_symbol += 1;
                                    slist_0_sprint(symList, symPrint);
                                }
                                else {
                                    retract(3);
                                }
                            }
                            else {
                                retract(2);
                            }
                        }
                        else {
                            retract(1);
                        }
                    }
                    slist_1_sprint(symList, symPrint, "<常量定义>");
                    return true;
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    else {
        return false;
    }
}

bool _function_with_return_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
    if (_head_statement(symList, symPrint, symStack, symSymbol) && symList.type[pos_S_list] == "LPARENT") {
        // 声明头部
        func = 1;
        symList.has_return[pos_has_return] = symList.name[pos_temp_has_return];
        pos_has_return += 1;
        if (symList.type[pos_S_list] == "LPARENT") {
            slist_0_sprint(symList, symPrint);
            if (_table_parameter(symList, symPrint, symStack, symSymbol)) {
                // 参数表
                if (symList.type[pos_S_list] == "RPARENT") {
                    slist_0_sprint(symList, symPrint);
                    if (symList.type[pos_S_list] == "LBRACE") {
                        // {
                        slist_0_sprint(symList, symPrint);
                        if (_statement_combination(symList, symPrint, symStack, symSymbol)) {
                            // 复合语句
                            if (symList.type[pos_S_list] == "RBRACE") {
                                // }
                                slist_0_sprint(symList, symPrint);
                                slist_1_sprint(symList, symPrint, "<有返回值函数定义>");
                                func = 0;
                                return true;
                            }
                            else {
                                retract(6);
                            }
                        }
                        else {
                            retract(5);
                        }
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    else {
        return false;
    }
}

int _head_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜声明头部＞   ::=  int＜标识符＞
    //                |  char＜标识符＞
    if (symList.type[pos_S_list] == "INTTK" && symList.type[pos_S_list + 2] == "LPARENT") { // int || char
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "IDENFR") {
            symPrint.name[pos_S_print] = symList.name[pos_S_list];
            symPrint.type[pos_S_print] = symList.type[pos_S_list];
            symPrint.num[pos_S_print] = 0;
            pos_temp_has_return = pos_S_list;
            pos_S_print += 1;
            pos_S_list += 1;
            slist_1_sprint(symList, symPrint, "<声明头部>");
            return pos_temp_has_return;
        }
        else {
            retract(1);
        }
    }
    else if (symList.type[pos_S_list] == "CHARTK" && symList.type[pos_S_list + 2] == "LPARENT") { // int || char
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "IDENFR") {
            symPrint.name[pos_S_print] = symList.name[pos_S_list];
            symPrint.type[pos_S_print] = symList.type[pos_S_list];
            symPrint.num[pos_S_print] = 0;
            pos_temp_has_return = pos_S_list;
            pos_S_print += 1;
            pos_S_list += 1;
            slist_1_sprint(symList, symPrint, "<声明头部>");
            return pos_temp_has_return;
        }
        else {
            retract(1);
        }
    }
    else {
        return 0;
    }
}

bool _table_parameter(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}
    //               |  ＜空＞
    if (symList.type[pos_S_list] == "INTTK" || symList.type[pos_S_list] == "CHARTK") { // 类型标识符
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "IDENFR") { // 标识符
            slist_0_sprint(symList, symPrint);
            while (symList.type[pos_S_list] == "COMMA") { // ,
                slist_0_sprint(symList, symPrint);
                if (symList.type[pos_S_list] == "INTTK" || symList.type[pos_S_list] == "CHARTK") { // 类型标识符
                    slist_0_sprint(symList, symPrint);
                    if (symList.type[pos_S_list] == "IDENFR") { // 标识符
                        slist_0_sprint(symList, symPrint);
                    }
                    else {
                        retract(2);
                        break;
                    }
                }
                else {
                    retract(1);
                    break;
                }
            }
            slist_1_sprint(symList, symPrint, "<参数表>");
            return true;
        }
        else {
            retract(1);
        }
    }
    else if (symList.type[pos_S_list] == "RPARENT") {   // )
        slist_1_sprint(symList, symPrint, "<参数表>");
        return true;
    }
    else {
        return false;
    }
}

bool _statement_combination(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
    int flag = 0;
    if (_const_statement(symList, symPrint, symStack, symSymbol)) { // 常量说明
        flag += 1;
    }
    if (_var_statement(symList, symPrint, symStack, symSymbol)) { // 变量说明
        flag += 1;
    }
    if (_list_statement(symList, symPrint, symStack, symSymbol)) {
        // 语句列
        slist_1_sprint(symList, symPrint, "<复合语句>");
        return true;
    }
    else {
        if (flag) {
            retract(1);
        }
        return false;
    }
}

bool _function_no_return_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
    int temp;
    if (symList.type[pos_S_list] == "VOIDTK" && symList.type[pos_S_list + 1] != "MAINTK" && symList.type[pos_S_list + 2] == "LPARENT") { // void
        func = 1;
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "IDENFR") { // 标识符
            symPrint.name[pos_S_print] = symList.name[pos_S_list];
            symPrint.type[pos_S_print] = symList.type[pos_S_list];
            symPrint.num[pos_S_print] = 0;
            temp = pos_S_list;
            pos_S_print += 1;
            pos_S_list += 1;
            symList.no_return[pos_no_return] = symList.name[temp];
            pos_no_return += 1;
            if (symList.type[pos_S_list] == "LPARENT") { // (
                slist_0_sprint(symList, symPrint);
                if (_table_parameter(symList, symPrint, symStack, symSymbol)) {
                    // 参数表
                    if (symList.type[pos_S_list] == "RPARENT") {
                        // )
                        slist_0_sprint(symList, symPrint);
                        if (symList.type[pos_S_list] == "LBRACE") {
                            // {
                            slist_0_sprint(symList, symPrint);
                            if (_statement_combination(symList, symPrint, symStack, symSymbol)) {
                                // 复合语句
                                if (symList.type[pos_S_list] == "RBRACE") {
                                    // }
                                    slist_0_sprint(symList, symPrint);
                                    slist_1_sprint(symList, symPrint, "<无返回值函数定义>");
                                    func = 0;
                                    return true;
                                }
                                else {
                                    retract(7);
                                }
                            }
                            else {
                                retract(6);
                            }
                        }
                        else {
                            retract(5);
                        }
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
            return false;
        }
    }
    else {
        return false;
    }
}

bool _main(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
    if (symList.type[pos_S_list] == "VOIDTK") { // void
        slist_0_sprint(symList, symPrint);
        if (symList.type[pos_S_list] == "MAINTK") { // main
            slist_0_sprint(symList, symPrint);
            if (symList.type[pos_S_list] == "LPARENT") { // (
                slist_0_sprint(symList, symPrint);
                if (symList.type[pos_S_list] == "RPARENT") { // )
                    slist_0_sprint(symList, symPrint);
                    if (symList.type[pos_S_list] == "LBRACE") { // {
                        slist_0_sprint(symList, symPrint);
                        if (_statement_combination(symList, symPrint, symStack, symSymbol)) {
                            // 复合语句
                            if (symList.type[pos_S_list] == "RBRACE") { // }
                                slist_0_sprint(symList, symPrint);
                                slist_1_sprint(symList, symPrint, "<主函数>");
                                return true;
                            }
                            else {
                                retract(6);
                            }
                        }
                        else {
                            retract(5);
                        }
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    else {
        return false;
    }
}

bool _list_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜语句列＞   ::= ｛＜语句＞｝
    while (_statement(symList, symPrint, symStack, symSymbol)) {
        // 语句) { // if while for ( 标识符 scanf printf ; return switch
    }
    slist_1_sprint(symList, symPrint, "<语句列>");
    return true;
}

bool _if(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
    if (isParticular_S_List(symList, "IFTK")) { // if
        slist_0_sprint(symList, symPrint);
        if (isParticular_S_List(symList, "LPARENT")) { // (
            slist_0_sprint(symList, symPrint);
            if (_condition(symList, symPrint, symStack, symSymbol)) {
                // 条件
                if (isParticular_S_List(symList, "RPARENT")) {
                    slist_0_sprint(symList, symPrint);
                    if (_statement(symList, symPrint, symStack, symSymbol)) {
                        if (isParticular_S_List(symList, "ELSETK")) {
                            if (_statement(symList, symPrint, symStack, symSymbol)) {
                                slist_1_sprint(symList, symPrint, "<条件语句>");
                                return true;
                            }
                            else {
                                retract(1);
                            }
                        }
                        slist_1_sprint(symList, symPrint, "<条件语句>");
                        return true;
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    else {
        return false;
    }
}

bool _condition(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
    if (_expression(symList, symPrint, symStack, symSymbol)) {
        // 表达式
        if (isParticular_S_List(symList, "LSS") || isParticular_S_List(symList, "LEQ")
            || isParticular_S_List(symList, "GRE") || isParticular_S_List(symList, "GEQ")
            || isParticular_S_List(symList, "EQL") || isParticular_S_List(symList, "NEQ")) {
            slist_0_sprint(symList, symPrint);
            if (_expression(symList, symPrint, symStack, symSymbol)) {
                slist_1_sprint(symList, symPrint, "<条件>");
                return true;
            }
            else {
                retract(1);
            }
        }
        else {
            slist_1_sprint(symList, symPrint, "<条件>");
            return true;
        }
    }
    else {
        return false;
    }
}

bool _expression(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    //  ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    int flag = 0;
    if (isParticular_S_List(symList, "PLUS")) { // +
        slist_0_sprint(symList, symPrint);
        flag = 1;
    }
    if (isParticular_S_List(symList, "MINU")) { // -
        slist_0_sprint(symList, symPrint);
        flag = 2;
    }
    if (_term(symList, symPrint, symStack, symSymbol)) {
        // 项
        if (flag == 2) {
            emit_op1(4);
        }
        if (isParticular_S_List(symList, "PLUS") || isParticular_S_List(symList, "MINU")) {
            do {
                if (isParticular_S_List(symList, "PLUS")) { // +
                    flag = 1;
                }
                else if (isParticular_S_List(symList, "MINU")) { // -
                    flag = 2;
                }
                slist_0_sprint(symList, symPrint);
                if (_term(symList, symPrint, symStack, symSymbol)) {
                    // 项
                }
                if (flag == 1) {
                    emit_op1(5); // stack_top_second += stack_top
                }
                else if (flag == 2) {
                    emit_op1(6); // stack_top_second -= stack_top
                }
            }   while (isParticular_S_List(symList, "PLUS") || isParticular_S_List(symList, "MINU"));
        }
        slist_1_sprint(symList, symPrint, "<表达式>");
        return true;
    }
    else {
        if (flag) {
            retract(1);
        }
        return false;
    }
}

bool _term(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
    if (_factor(symList, symPrint, symStack, symSymbol)) {
        // 因子
        if (isParticular_S_List(symList, "MULT") || isParticular_S_List(symList, "DIV")) {
            do {
                int temp = 0;
                if (isParticular_S_List(symList, "MULT")) {
                    temp = 1;
                }
                else if (isParticular_S_List(symList, "DIV")) {
                    temp = 2;
                }
                slist_0_sprint(symList, symPrint);
                if (_factor(symList, symPrint, symStack, symSymbol)) {
                    // 因子
                }
                else {
                    retract(1);
                    break;
                }
                if (temp == 1) {
                    emit_op1(2);    // stack_top_second *= stack_top
                }
                else if (temp == 2) {
                    emit_op1(3);    // stack_top_second /= stack_top
                }
            }   while (isParticular_S_List(symList, "MULT") || isParticular_S_List(symList, "DIV"));
        }
        slist_1_sprint(symList, symPrint, "<项>");
        return true;
    }
    else {
        return false;
    }
}

bool _loop(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞
    // | for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
    if (isParticular_S_List(symList, "WHILETK")) { // while
        slist_0_sprint(symList, symPrint);
        if (isParticular_S_List(symList, "LPARENT")) { // (
            slist_0_sprint(symList, symPrint);
            if (_condition(symList, symPrint, symStack, symSymbol)) {
                // 条件
                if (isParticular_S_List(symList, "RPARENT")) {
                    // )
                    slist_0_sprint(symList, symPrint);
                    if (_statement(symList, symPrint, symStack, symSymbol)) {
                        // 语句
                        slist_1_sprint(symList, symPrint, "<循环语句>");
                        return true;
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }

    else if (isParticular_S_List(symList, "FORTK")) { // for
        slist_0_sprint(symList, symPrint);
        if (isParticular_S_List(symList, "LPARENT")) {
            // (
            slist_0_sprint(symList, symPrint);
            if (isParticular_S_List(symList, "IDENFR")) {
                slist_0_sprint(symList, symPrint);
                if (isParticular_S_List(symList, "ASSIGN")) {
                    // =
                    slist_0_sprint(symList, symPrint);
                    if (_expression(symList, symPrint, symStack, symSymbol)) {
                        // 表达式
                        if (isParticular_S_List(symList, "SEMICN")) {
                            // ;
                            slist_0_sprint(symList, symPrint);
                            if (_condition(symList, symPrint, symStack, symSymbol)) {
                                // 条件
                                if (isParticular_S_List(symList, "SEMICN")) {
                                    // ;
                                    slist_0_sprint(symList, symPrint);
                                    if (isParticular_S_List(symList, "IDENFR")) {
                                        // 标识符
                                        slist_0_sprint(symList, symPrint);
                                        if (isParticular_S_List(symList, "ASSIGN")) {
                                            slist_0_sprint(symList, symPrint);
                                            if (isParticular_S_List(symList, "IDENFR")) {
                                                // 标识符
                                                slist_0_sprint(symList, symPrint);
                                                if (isParticular_S_List(symList, "PLUS") || isParticular_S_List(symList, "MINU")) {
                                                    // + -
                                                    slist_0_sprint(symList, symPrint);
                                                    if (_step(symList, symPrint, symStack, symSymbol)) {
                                                        if (isParticular_S_List(symList, "LPARENT")) {
                                                            // )
                                                            slist_0_sprint(symList, symPrint);
                                                            if (_statement(symList, symPrint, symStack, symSymbol)) {
                                                                // 语句
                                                                slist_1_sprint(symList, symPrint, "<循环语句>");
                                                                return true;
                                                            }
                                                            else {
                                                                retract(14);
                                                            }
                                                        }
                                                        else {
                                                            retract(13);
                                                        }
                                                    }
                                                    else {
                                                        retract(12);
                                                    }
                                                }
                                                else {
                                                    retract(11);
                                                }
                                            }
                                            else {
                                                retract(10);
                                            }
                                        }
                                        else {
                                            retract(9);
                                        }
                                    }
                                    else {
                                        retract(8);
                                    }
                                }
                                else {
                                    retract(7);
                                }
                            }
                            else {
                                retract(6);
                            }
                        }
                        else {
                            retract(5);
                        }
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    else if (isParticular_S_List(symList, "DOTK")) {    // do
        slist_0_sprint(symList, symPrint);
        if (_statement(symList, symPrint, symStack, symSymbol)) {
            // 语句
            if (isParticular_S_List(symList, "WHILETK")) { // while
                slist_0_sprint(symList, symPrint);
                if (isParticular_S_List(symList, "LPARENT")) { // (
                    slist_0_sprint(symList, symPrint);
                    if (_condition(symList, symPrint, symStack, symSymbol)) {
                        // 条件
                        if (isParticular_S_List(symList, "RPARENT")) {
                            // )
                            slist_0_sprint(symList, symPrint);
                            slist_1_sprint(symList, symPrint, "<循环语句>");
                            return true;
                        }
                        else {
                            retract(5);
                        }
                    }
                    else {
                        retract(4);
                    }
                }
                else {
                    retract(3);
                }
            }
            else {
                retract(2);
            }
        }
        else {
            retract(1);
        }
    }
    else {
        return false;
    }
}

bool _string(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
    if (symList.type[pos_S_list] == "STRCON") { // 字符串
        slist_0_sprint(symList, symPrint);
        slist_1_sprint(symList, symPrint, "<字符串>");
        return true;
    }
    else {
        return false;
    }
}

bool _step(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜步长＞::= ＜无符号整数＞
    if (_unsigned_int(symList, symPrint, symStack, symSymbol)) {
        // 无符号整数
        slist_1_sprint(symList, symPrint, "<步长>");
        return true;
    }
    return false;
}

bool _unsigned_int(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
    if (symList.type[pos_S_list] == "INTCON") {
        slist_0_sprint(symList, symPrint);
        slist_1_sprint(symList, symPrint, "<无符号整数>");
        return true;
    }
    else {
        return false;
    }
}

bool _int(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    int flag = 1;
    if (symList.type[pos_S_list] == "MINU" || symList.type[pos_S_list] == "PLUS") {
        slist_0_sprint(symList, symPrint);
        flag = 1;
    }
    if (_unsigned_int(symList, symPrint, symStack, symSymbol)) {
        // 无符号整数
        slist_1_sprint(symList, symPrint, "<整数>");
        return true;
    }
    else {
        if (flag == 1) {
            retract(1);
        }
        return false;
    }
}

bool _char(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    if (symbol == CHARCON) { // 字符
        getsym(yes);
    }
}

bool _scanf(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜读语句＞    ::=  scanf '('＜标识符＞')'
    if (symbol == SCANFTK) { // scanf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == IDENFR) { // 标识符
                getsym(yes);
            }
            if (symbol == RPARENT) { // }
                getsym(yes);
            }
        }
    }
}

bool _printf(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'
    //              |  printf '('＜字符串＞ ')'
    //              | printf '('＜表达式＞')'
    if (symbol == PRINTFTK) { // printf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == STRCON) { // 字符串
                _string();  // 字符串
                if (symbol == COMMA) { // ,
                    getsym(yes);
                    _expression();  // 表达式
                }
            }
            else {
                _expression(); // 表达式
            }
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
    fprintf(f_out, "<写语句>\n");
    cout << "<写语句>" << endl;
}

bool _return(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
    fprintf(f_out, "<返回语句>\n");
    cout << "<返回语句>" << endl;
}

bool _function_with_return_call(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
    fprintf(f_out, "<有返回值函数调用语句>\n");
    cout << "<有返回值函数调用语句>" << endl;
}

bool _function_no_return_call(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
    fprintf(f_out, "<无返回值函数调用语句>\n");
    cout << "<无返回值函数调用语句>" << endl;
}

bool _table_parameter_value(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}
    //              ｜   ＜空＞
    if (symbol == RPARENT) { // 空
        fprintf(f_out, "<值参数表>\n");
//        cout << "<值参数表>" << endl;
    }
    else {
        _expression();  // 表达式
        while (symbol == COMMA) { // ,
            getsym(yes);
            _expression();  // 表达式
        }
        fprintf(f_out, "<值参数表>\n");
        cout << "<值参数表>" << endl;
    }
}

bool _statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
    fprintf(f_out, "<语句>\n");
    cout << "<语句>" << endl;
}

bool _default(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜缺省＞   ::=  default :＜语句＞
    if (symbol == DEFAULTTK) { // default
        getsym(yes);
        if (symbol == COLON) { // :
            getsym(yes);
            _statement();   // 语句
        }
    }
    fprintf(f_out, "<缺省>\n");
    cout << "<缺省>" << endl;
}

bool _switch(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
    fprintf(f_out, "<情况语句>\n");
    cout << "<情况语句>" << endl;
}

bool _case(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
    if (symbol == CASETK) {  // case
        getsym(yes);
        _const();    // 常量
        if (symbol == COLON) {   // :
            getsym(yes);
            _statement();    // 语句
        }
    }
    fprintf(f_out, "<情况子语句>\n");
    cout << "<情况子语句>" << endl;
}

bool _table_cases(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    while (symbol == CASETK) {
        _case();
    }
    fprintf(f_out, "<情况表>\n");
    cout << "<情况表>" << endl;
}

bool _const(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜常量＞   ::=  ＜整数＞
    //             |  ＜字符＞


    if (symbol == INTCON||symbol == PLUS || symbol == MINU) { // 整数
        _int(); // 整数
    }
    else if (symbol == CHARCON) { // 字符
        _char();    // 字符
    }
    fprintf(f_out, "<常量>\n");
    cout << "<常量>" << endl;
}

bool _var_define(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
            //pre_read_Symbol(1);
//            if (symbol_pre == IDENFR) {    // 标识符
//
//            }
            _var_define_with_initialization();  // 变量定义及初始化
        }
    }
    fprintf(f_out, "<变量定义>\n");
    cout << "<变量定义>" << endl;
}

bool _assign(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞
    //                |  ＜标识符＞'['＜表达式＞']'=＜表达式＞
    //                |  ＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
    if (symbol == IDENFR) { // 标识符
        getsym(yes);
        if (symbol == ASSIGN) { // =
            getsym(yes);
            _expression();  // 表达式
        }
        else if (symbol == LBRACK) {    // [
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
    fprintf(f_out, "<赋值语句>\n");
    cout << "<赋值语句>" << endl;
}

bool _factor(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
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
    fprintf(f_out, "<因子>\n");
    cout << "<因子>" << endl;
}

bool _var_define_no_initialization(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞
    //                      | ＜标识符＞'['＜无符号整数＞']'
    //                      | ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']')
    //                      {,(＜标识符＞
    //                      | ＜标识符＞'['＜无符号整数＞']'
    //                      | ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
    if (symbol == INTTK || symbol == CHARTK) { // 类型标识符
        do {
            getsym(yes);
            if (symbol == IDENFR) { // 标识符
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
            }
        }   while (symbol == COMMA);    // ;
    }
    fprintf(f_out, "<变量定义无初始化>\n");
    cout << "<变量定义无初始化>" << endl;
}

bool _var_define_with_initialization(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol) {
    // ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞
    //                      | ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
    //                      |＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'=
    //                      '{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
    if (symbol == INTTK || symbol == CHARTK) {  // 类型标识符
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                _const();   // 常量
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
    fprintf(f_out, "<变量定义及初始化>\n");
    cout << "<变量定义及初始化>" << endl;
}
