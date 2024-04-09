// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmshparsercpp/MshLexer.h"
#include <iostream>

namespace gmshparsercpp {

MshLexer::MshLexer(std::ifstream * in) : in(in), have_token(false), binary(false) {}

void
MshLexer::set_binary(bool state)
{
    this->binary = state;
}

MshLexer::Token
MshLexer::read()
{
    peek();
    this->have_token = false;
    return this->curr;
}

MshLexer::Token
MshLexer::peek()
{
    if (!this->have_token) {
        this->curr = read_token();
        this->have_token = true;
    }
    return this->curr;
}

MshLexer::Token
MshLexer::read_token()
{
    while (true) {
        if (this->in->peek() == EOF) {
            Token t = { Token::EndOfFile, "", -1 };
            return t;
        }

        std::string str_tok;
        char ch = read_char();
        if (ch == '$') {
            // $SectionName
            str_tok += ch;
            while (true) {
                auto next = peek_char();
                if ((next >= 'A' && next <= 'Z') || (next >= 'a' && next <= 'z')) {
                    str_tok += read_char();
                }
                else {
                    // read the delimiting char
                    read_char();
                    Token token = { Token::Section, str_tok, -1 };
                    return token;
                }
            }
        }
        else if (ch == '"') {
            // "string"
            while (true) {
                auto next = peek_char();
                if (next == '"') {
                    read_char();
                    break;
                }
                else
                    str_tok += read_char();
            }

            Token token = { Token::String, str_tok, -1 };
            return token;
        }
        else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            // skip white spaces
            continue;
        }
        else {
            // numbers
            str_tok += ch;
            while (true) {
                auto next = peek_char();
                if (next == ' ' || next == '\t' || next == '\n' || next == '\r') {
                    // read the delimiting char
                    read_char();
                    Token token = { Token::Number, str_tok, -1 };
                    return token;
                }
                else
                    str_tok += read_char();
            }
        }
    }
}

char
MshLexer::read_char()
{
    char ch;
    this->in->read(&ch, sizeof(ch));
    if (ch == EOF)
        throw Exception("Reached end of file");
    return ch;
}

int
MshLexer::peek_char()
{
    auto ch = this->in->peek();
    if (ch == EOF)
        throw Exception("Reached end of file");
    return ch;
}

} // namespace gmshparsercpp
