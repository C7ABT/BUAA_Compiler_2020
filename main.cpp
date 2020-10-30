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
void error(char errtype) {

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
int line_prev = 0;  // for ';', the line of last symbol
FILE *f_in, *f_out;

void getBuffer() {
    f_in = fopen("testfile.txt", "rb");
    f_out = fopen("error.txt", "wb");
    fread(buffer,1, 8888888, f_in);
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
    return (c == '\t' || c == '\r');
}   // \t \r
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
    return (c == '\n');
}   // \n
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
bool isDividedLine() {
    return (c == '\\');
}   // \\

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

void getsym(bool checkError = true) {
    do {
        line_prev = line;
        clearToken();
        clearSymbol();
        getChar();
        while (isSpace() || isNextline() || isTab()) {
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
            if (token.length() >= 2 && token[0] == '0') {   // 002
                if (checkError) {
                    error('a');
                }
            }
            symbol = INTCON;
        } else if (isSingleQuote()) {
            getChar();
            catToken();
            if (!(isPlus() || isMinus() || isStar() || isDiv() || isLetter() || isUnderscore() || isDigit())) {
                if (checkError) {
                    error('a');
                }
            }
            getChar();
            if (!isSingleQuote()) {
                if (checkError) {
                    error('a');
                }
                retract();
            }
            symbol = CHARCON;
        } else if (isDoubleQuote()) {
            bool haveError = false;
            getChar();
            while (!isDoubleQuote()) {
                if (!isSpace() && !isExclamation() || !(c >= 35 && c <= 126)) {
                    haveError = true;
                }
                catToken();
                getChar();
            }
            if (haveError && checkError) {
                error('a');
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
            getChar();
            if (isEqual()) {
                catToken();
                symbol = NEQ;
            } else {
                retract();
                error('a');
            }
        }   else if (isDiv()) {
            getChar();
            if (isStar()) {
                do {
                    do {
                        getChar();
                    }   while (!isStar());
                    do {
                        getChar();
                        if (isDiv()) {
                            return;
                        }
                    }   while (isStar());
                }   while (!isStar());
            }
            retract();
            symbol = DIV;
            catToken();
        }   else if (isPlus() || isMinus() || isStar() || isSemicolon() || isComma() || isColon() || \
        isLParent() || isRParent() || isLBrack() || isRBrack() || isLBrace() || isRBrace()) {
            catToken();
            symbol = reservedSymbol[c];
        }   else {
            if (checkError) {
                error('a');
            }
            continue;
        }
        break;
    }   while (true);
    return;
}

// 词法分析

void program(); // 程序
void _string(); // 字符串
void _const(); // 常量
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
void _expression(); // 表达式
void _statement(); // 语句
void _default(); // 缺省
void _return(); // 返回语句
void _unsigned_int(); // 无符号整数
void _step(); // 步长
void _main(); // 主函数
void _head_statement(); // 声明头部
void _int(); // 整数
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
void _char(); // 字符

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
    int line_origin = line;
    while (n--) {
        getsym(no);
    }
    symbol_pre = symbol;    // 预读得到的symbol
    symbol = symbol_origin; // 还原symbol
    token = token_origin;   // 还原token
    line = line_origin;     // 还原line
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

// main
int main() {
    getBuffer();
    getsym(no);
    program();
    return 0;
}






