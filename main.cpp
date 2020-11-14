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
    int ref;    // 值
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

void slist_1_sprint (Sym_list & slist, Sym_print & sprint) {
    sprint.num[pos_S_print] = 1;
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
bool _head_statement(Sym_list & symList, Sym_print & symPrint, Sym_stack & symStack, Sym_symbol & symSymbol);
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
SymbolType symbol_pre;
string array_function_with_return[1000]; // 有返回值函数名
string array_function_no_return[1000];    // 无返回值函数名
int index_array_function_with_return;
int index_array_function_no_return;

void pre_read_Symbol(int n){
    // 预读
    SymbolType symbol_origin = symbol;
    string token_origin = token;
    int index_buffer_origin = index_buffer;
    while (n--) {
        getsym(no);
    }
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
    fprintf(f_out, "<程序>\n");
    cout << "<程序>" << endl;
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
    fprintf(f_out, "<常量说明>\n");
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
    fprintf(f_out, "<变量说明>\n");
    cout << "<变量说明>" << endl;
}
////////////////////

void _const_define() {
    // ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
    //                  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
    if (symbol == INTTK) { // int
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                _int(); // 整数
                while (symbol == COMMA) { // ,
                    getsym(yes);
                    if (symbol == IDENFR) { // 标识符
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        _int(); // 整数
                    }
                }
            }
        }
    }

    else if (symbol == CHARTK) { // char
        getsym(yes);
        if (symbol == IDENFR) { // 标识符
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                _char(); // 字符
                while (symbol == COMMA) { // ,
                    getsym(yes);
                    if (symbol == IDENFR) { // 标识符
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        _char(); // 字符
                    }
                }
            }
        }
    }
    fprintf(f_out, "<常量定义>\n");
    cout << "<常量定义>" << endl;
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
    fprintf(f_out, "<有返回值函数定义>\n");
    cout << "<有返回值函数定义>" << endl;
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
    fprintf(f_out, "<声明头部>\n");
    cout << "<声明头部>" << endl;
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
    fprintf(f_out, "<参数表>\n");
    cout << "<参数表>" << endl;
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
    fprintf(f_out, "<复合语句>\n");
    cout << "<复合语句>" << endl;
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
    fprintf(f_out, "<无返回值函数定义>\n");
    cout << "<无返回值函数定义>" << endl;
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
    fprintf(f_out, "<主函数>\n");
    cout << "<主函数>" << endl;
}

void _list_statement() {
    // ＜语句列＞   ::= ｛＜语句＞｝
    while (symbol == IFTK || symbol == WHILETK || symbol == FORTK || symbol == LBRACE
           || symbol == IDENFR || symbol == SCANFTK || symbol == PRINTFTK || symbol == SEMICN
           || symbol == RETURNTK || symbol == SWITCHTK ) { // if while for ( 标识符 scanf printf ; return switch
        _statement(); // 语句
    }
    fprintf(f_out, "<语句列>\n");
    cout << "<语句列>" << endl;
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
    fprintf(f_out, "<条件语句>\n");
    cout << "<条件语句>" << endl;
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
    fprintf(f_out, "<条件>\n");
    cout << "<条件>" << endl;
}

void _expression() {
    //  ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    if (symbol == PLUS || symbol == MINU) { // 加法运算符
        getsym(yes);
    }
    _term(); // 项
    while (symbol == PLUS || symbol == MINU) { // 加法运算符
        getsym(yes);
        _term(); // 项
    }
    fprintf(f_out, "<表达式>\n");
    cout << "<表达式>" << endl;
}

void _term() {
    // ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
    _factor(); // 因子
    while (symbol == MULT || symbol == DIV) { // 乘法运算符
        getsym(yes);
        _factor();  // 因子
    }
    fprintf(f_out, "<项>\n");
    cout << "<项>" << endl;
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
    fprintf(f_out, "<循环语句>\n");
    cout << "<循环语句>" << endl;
}

void _string() {
    // ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
    if (symbol == STRCON) { // 字符串
        getsym(yes);
    }
    fprintf(f_out, "<字符串>\n");
    cout << "<字符串>" << endl;
}

void _step() {
    // ＜步长＞::= ＜无符号整数＞
    _unsigned_int(); // 无符号整数
    fprintf(f_out, "<步长>\n");
    cout << "<步长>" << endl;
}

void _unsigned_int() {
    // ＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
    if (symbol == INTCON) {
        getsym(yes);
    }
    fprintf(f_out, "<无符号整数>\n");
    cout << "<无符号整数>" << endl;
}

void _int() {
    if (symbol == PLUS || symbol == MINU) {
        getsym(yes);
    }
    _unsigned_int(); // 无符号整数
    fprintf(f_out, "<整数>\n");
    cout << "<整数>" << endl;
}

void _char() {
    if (symbol == CHARCON) { // 字符
        getsym(yes);
    }
}

void _scanf() {
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
    fprintf(f_out, "<读语句>\n");
    cout << "<读语句>" << endl;
}

void _printf() {
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
    fprintf(f_out, "<返回语句>\n");
    cout << "<返回语句>" << endl;
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
    fprintf(f_out, "<有返回值函数调用语句>\n");
    cout << "<有返回值函数调用语句>" << endl;
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
    fprintf(f_out, "<无返回值函数调用语句>\n");
    cout << "<无返回值函数调用语句>" << endl;
}

void _table_parameter_value() {
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
    fprintf(f_out, "<语句>\n");
    cout << "<语句>" << endl;
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
    fprintf(f_out, "<缺省>\n");
    cout << "<缺省>" << endl;
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
    fprintf(f_out, "<情况语句>\n");
    cout << "<情况语句>" << endl;
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
    fprintf(f_out, "<情况子语句>\n");
    cout << "<情况子语句>" << endl;
}

void _table_cases() {
    // ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    while (symbol == CASETK) {
        _case();
    }
    fprintf(f_out, "<情况表>\n");
    cout << "<情况表>" << endl;
}

void _const() {
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

void _assign() {
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
    fprintf(f_out, "<因子>\n");
    cout << "<因子>" << endl;
}

void _var_define_no_initialization() {
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

void _var_define_with_initialization() {
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
//                        if (symbol == LBRACE) { // {
//                            do {
//                                getsym(yes);
//                                _const();    // 常量
//                            }   while (symbol == COMMA);    // ,
//                            if (symbol == RBRACE) { // }
//                                getsym(yes);
//                            }
//                        }
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
                                /*
                                if (symbol == LBRACE) { // {
                                    do {
                                        do {
                                            getsym(yes);
                                            _const();   // 常量
                                        }   while (symbol == COMMA);    // ,
                                        if (symbol == RBRACE) { // }
                                            getsym(yes);
                                        }
                                    }   while (symbol == COMMA);    // ,
                                    if (symbol == RBRACE) { // }
                                        getsym(yes);
                                    }
                                }*/

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
