 // lexer for MSH files

%{
#include <iostream>
using namespace std;

enum mshFlexLexerToken : int {
    Number = 1,
    String,
    Section
};

%}

%option c++
%option prefix="msh"
%option batch
%option header-file="MshLexer.h"
%option yylineno

ws        [ \t]+
newline   \r|\n|\r\n

section   \$[A-Za-z]+

string    \"[^\n"]+\"
number    [+-]?([0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+)([eE][+-]?[0-9]+)?

%%

{ws}       /* skip blanks and tabs */

{newline}  /* skip newlines */

{number}   return mshFlexLexerToken::Number;

{string}   return mshFlexLexerToken::String;

{section}  return mshFlexLexerToken::Section;

%%

// comment
