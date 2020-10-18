//
// Created by Conno on 2020/9/29.
//

#include "iostream"
#include "string"
#include "algorithm"
#include "map"
#include <stdio.h>
#include "fstream"

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

void getBuffer() {
    f_in = fopen("../testfile.txt", "rb");
    f_out = fopen("../output.txt", "wb");
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
    if (symbol != FOUL && output) {
//        fprintf(f_out, "%s %s\n", SymbolType_String[symbol].c_str(), token.c_str());
        printf("%s %s\n", SymbolType_String[symbol].c_str(), token.c_str());

    }
}

// �ʷ�����

void program(); // ����
void _string(); // �ַ���
void _const(); // ����
void _const_define(); // ��������
void _const_statement(); // ����˵��
void _var_define(); // ��������
void _var_statement(); // ����˵��
void _var_define_no_initialization(); // ���������޳�ʼ��
void _var_define_with_initialization(); // �������弰��ʼ��
void scanf(); // �����
void _printf(); // д���
void _term(); // ��
void _factor(); // ����
void _expression(); // ���ʽ
void _statement(); // ���
void _default(); // ȱʡ
void _return(); // �������
void _unsigned_int(); // �޷�������
void _step(); // ����
void _main(); // ������
void _head_statement(); // ����ͷ��
void _int(); // ����
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
void _char(); // �ַ�

// �����Ƿָ��� //
SymbolType symbol_pre;
string array_function_with_return[1000]; // �з���ֵ������
string array_function_no_return[1000];    // �޷���ֵ������
int index_array_function_with_return;
int index_array_function_no_return;

void pre_read_Symbol(int n){
    // Ԥ��
    SymbolType symbol_origin = symbol;
    string token_origin = token;
    int index_buffer_origin = index_buffer;
    while (n--) {
        getsym(no);
    }
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
    cout << "<����>" << endl;
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
    cout << "<����˵��>" << endl;
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
    cout << "<����˵��>" << endl;
}
////////////////////

void _const_define() {
    // ���������壾   ::=   int����ʶ��������������{,����ʶ��������������}
    //                  | char����ʶ���������ַ���{,����ʶ���������ַ���}
    if (symbol == INTTK) { // int
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                _int(); // ����
                while (symbol == COMMA) { // ,
                    getsym(yes);
                    if (symbol == IDENFR) { // ��ʶ��
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        _int(); // ����
                    }
                }
            }
        }
    }

    else if (symbol == CHARTK) { // char
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            getsym(yes);
            if (symbol == ASSIGN) { // =
                getsym(yes);
                _char(); // �ַ�
                while (symbol == COMMA) { // ,
                    getsym(yes);
                    if (symbol == IDENFR) { // ��ʶ��
                        getsym(yes);
                    }
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        _char(); // �ַ�
                    }
                }
            }
        }
    }
    cout << "<��������>" << endl;
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
    cout << "<�з���ֵ��������>" << endl;
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
    cout << "<����ͷ��>" << endl;
}

void _table_parameter() {
    // ��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}
    //               |  ���գ�
    if (symbol == INTTK || symbol == CHARTK) { // ���ͱ�ʶ��
        getsym(yes);
        if (symbol = IDENFR) { // ��ʶ��
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
    cout << "<������>" << endl;
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
    cout << "<�������>" << endl;
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
    cout << "<�޷���ֵ��������>" << endl;
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
    cout << "<������>" << endl;
}

void _list_statement() {
    // ������У�   ::= ������䣾��
    while (symbol == IFTK || symbol == WHILETK || symbol == FORTK || symbol == LBRACE
           || symbol == IDENFR || symbol == SCANFTK || symbol == PRINTFTK || symbol == SEMICN
           || symbol == RETURNTK || symbol == SWITCHTK ) { // if while for ( ��ʶ�� scanf printf ; return switch
        _statement(); // ���
    }
    cout << "�����>" << endl;
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
    cout << "<�������>" << endl;
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
    cout << "<����>" << endl;
}

void _expression() {
    //  �����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
    if (symbol == PLUS || symbol == MINU) { // �ӷ������
        getsym(yes);
    }
    _term(); // ��
    while (symbol == PLUS || symbol == MINU) { // �ӷ������
        getsym(yes);
        _term(); // ��
    }
    cout << "<���ʽ>" << endl;
}

void _term() {
    // ���     ::= �����ӣ�{���˷�������������ӣ�}
    _factor(); // ����
    while (symbol == MULT || symbol == DIV) { // �˷������
        getsym(yes);
        _factor();  // ����
    }
    cout << "<��>" << endl;
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
        cout << "<ѭ�����>" << endl;
    }
}

void _string() {
    // ���ַ�����   ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
    if (symbol == STRCON) { // �ַ���
        getsym(yes);
    }
    cout << "<�ַ���>" << endl;
}

void _step() {
    // ��������::= ���޷���������
    _unsigned_int(); // �޷�������
    cout << "<����>" << endl;
}

void _unsigned_int() {
    // ���޷���������  ::= �����֣��������֣���
    if (symbol == INTCON) {
        getsym(yes);
    }
    cout << "<�޷�������>" << endl;
}

void _int() {
    if (symbol == PLUS || symbol == MINU) {
        getsym(yes);
    }
    _unsigned_int(); // �޷�������
    cout << "<����>" << endl;
}

void _char() {
    if (symbol == CHARCON) { // �ַ�
        getsym(yes);
    }
}

void _scanf() {
    // ������䣾    ::=  scanf '('����ʶ����')'
    if (symbol == SCANFTK) { // scanf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == IDENFR) { // ��ʶ��
                getsym(yes);
            }
            if (symbol == RPARENT) { // }
                getsym(yes);
            }
        }
    }
    cout << "<�����>" << endl;
}

void _printf() {
    // ��д��䣾    ::= printf '(' ���ַ�����,�����ʽ�� ')'
    //              |  printf '('���ַ����� ')'
    //              | printf '('�����ʽ��')'
    if (symbol == PRINTFTK) { // printf
        getsym(yes);
        if (symbol == LPARENT) { // (
            getsym(yes);
            if (symbol == STRCON) { // �ַ���
                _string();  // �ַ���
                if (symbol == COMMA) { // ,
                    getsym(yes);
                    _expression();  // ���ʽ
                }
            }
            else {
                _expression(); // ���ʽ
            }
            if (symbol == RPARENT) { // )
                getsym(yes);
            }
        }
    }
    cout << "<д���>" << endl;
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
    cout << "<�������>" << endl;
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
    cout << "<�з���ֵ�����������>" << endl;
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
    cout << "<�޷���ֵ�����������>" << endl;
}

void _table_parameter_value() {
    // ��ֵ������   ::= �����ʽ��{,�����ʽ��}
    //              ��   ���գ�
    if (symbol == RPARENT) { // ��
        cout << "<ֵ������>" << endl;
    }
    else {
        _expression();  // ���ʽ
        while (symbol == COMMA) { // ,
            getsym(yes);
            _expression();  // ���ʽ
        }
        cout << "<ֵ������>" << endl;
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
    cout << "<���>" << endl;
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
    cout << "<ȱʡ>" << endl;
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
    cout << "<������>" << endl;
}

void _case() {
    // ���������䣾  ::=  case��������������䣾
    if (symbol == CASETK) {  // case
        getsym(yes);
        _const();    // ����
        if (symbol == COLON) {   // :
            _statement();    // ���
        }
    }
    cout << "<��������>" << endl;
}

void _table_cases() {
    // �������   ::=  ���������䣾{���������䣾}
    while (symbol == CASETK) {
        _case();
    }
    cout << "<�����>" << endl;
}

void _const() {
    // ��������   ::=  ��������
    //             |  ���ַ���
    if (symbol == INTCON) { // ����
        _int(); // ����
    }
    else if (symbol == CHARCON) { // �ַ�
        _char();    // �ַ�
    }
    cout << "<����>" << endl;
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
        pre_read_Symbol(1);
        if (symbol_pre == LPARENT) { // (
            _var_define_no_initialization();    // ���������޳�ʼ��
        }
        else if (symbol_pre == IDENFR) {    // ��ʶ��
            _var_define_with_initialization();  // �������弰��ʼ��
        }
    }
    cout << "<��������>" << endl;
}

void _assign() {
    // ����ֵ��䣾   ::=  ����ʶ�����������ʽ��
    //                |  ����ʶ����'['�����ʽ��']'=�����ʽ��
    //                |  ����ʶ����'['�����ʽ��']''['�����ʽ��']' =�����ʽ��
    if (symbol == IDENFR) { // ��ʶ��
        getsym(yes);
        if (symbol == ASSIGN) { // =
            getsym(yes);
            _expression();  // ���ʽ
        }
        else if (symbol == LBRACK) {    // [
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
    cout << "<��ֵ���>" << endl;
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
    cout << "<����>" << endl;
}

void _var_define_no_initialization() {
    // �����������޳�ʼ����  ::= �����ͱ�ʶ����(����ʶ����
    //                      | ����ʶ����'['���޷���������']'
    //                      | ����ʶ����'['���޷���������']''['���޷���������']')
    //                      {,(����ʶ����
    //                      | ����ʶ����'['���޷���������']'
    //                      | ����ʶ����'['���޷���������']''['���޷���������']' )}
    if (symbol == INTTK || symbol == CHARTK) { // ���ͱ�ʶ��
        do {
            getsym(yes);
            if (symbol == IDENFR) { // ��ʶ��
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
            }
        }   while (symbol == COMMA);    // ;
    }
    cout << "<���������޳�ʼ��>" << endl;
}

void _var_define_with_initialization() {
    // ���������弰��ʼ����  ::= �����ͱ�ʶ��������ʶ����=��������
    //                      | �����ͱ�ʶ��������ʶ����'['���޷���������']'='{'��������{,��������}'}'
    //                      |�����ͱ�ʶ��������ʶ����'['���޷���������']''['���޷���������']'=
    //                      '{''{'��������{,��������}'}'{, '{'��������{,��������}'}'}'}'
    if (symbol == INTTK || symbol == CHARTK) {  // ���ͱ�ʶ��
        getsym(yes);
        if (symbol == IDENFR) { // ��ʶ��
            if (symbol == ASSIGN) { // =
                getsym(yes);
                _const();   // ����
            }
            else if (symbol == LBRACK) {    // [
                getsym(yes);
                _unsigned_int();    // �޷�������
                if (symbol == RBRACK) { // ]
                    getsym(yes);
                    if (symbol == ASSIGN) { // =
                        getsym(yes);
                        if (symbol == LBRACE) { // {
                            do {
                                getsym(yes);
                                _const();    // ����
                            }   while (symbol == COMMA);    // ,
                            if (symbol == RBRACE) { // }
                                getsym(yes);
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
                                if (symbol == LBRACE) { // {
                                    do {
                                        do {
                                            getsym(yes);
                                            _const();   // ����
                                        }   while (symbol == COMMA);    // ,
                                        if (symbol == RBRACE) { // }
                                            getsym(yes);
                                        }
                                    }   while (symbol == COMMA);    // ,
                                    if (symbol == RBRACE) { // }
                                        getsym(yes);
                                    }
                                }
                            }
                        }
                        if (symbol = RBRACE) {  // }
                            getsym(yes);
                        }
                    }
                }
            }
        }
    }
    cout << "<�������弰��ʼ��>" << endl;
}

// main
int main() {
    getBuffer();
    getsym(yes);
    program();
    return 0;           
}






