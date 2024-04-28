// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <fstream>
#include "gmshparsercpp/Exception.h"

namespace gmshparsercpp {

class MshLexer {
public:
    struct Token {
        enum EType : int { EndOfFile = 0, Number = 1, String, Section };

        ///
        EType type;
        ///
        std::string str;
        /// Line number
        int line_no;

        template <typename T>
        T
        as() const
        {
            throw Exception("Unsupported type");
        }
    };

    explicit MshLexer(std::ifstream * in);

    void set_binary(bool state);

    /// Look at the next token awaiting in the input stream
    Token peek();

    /// Read a token from the input stream
    Token read();

    /// Read binary blob from the input stream
    template <typename T>
    T
    read_blob()
    {
        T val;
        this->in->read((char *) &val, sizeof(T));
        return val;
    }

    template <typename T>
    T
    get()
    {
        if (this->binary)
            return read_blob<T>();
        else
            return read().as<T>();
    }

private:
    /// Read a token from an input stream
    Token read_token();
    /// Look at a character from an input stream without reading it
    int peek_char();
    /// Read a character from an input stream
    char read_char();

    /// Input stream
    std::ifstream * in;
    /// Flag indicating if we have a token cached
    bool have_token;
    /// Cached token
    Token curr;
    ///
    bool binary;
};

template <>
inline int
MshLexer::Token::as() const
{
    if (this->type == Number)
        return std::stoi(this->str);
    else
        throw Exception("Token is not a number");
}

template <>
inline size_t
MshLexer::Token::as() const
{
    if (this->type == Number)
        return std::stol(this->str);
    else
        throw Exception("Token is not a number");
}

template <>
inline double
MshLexer::Token::as() const
{
    if (this->type == Number)
        return std::stod(this->str);
    else
        throw Exception("Token is not a number");
}

template <>
inline std::string
MshLexer::Token::as() const
{
    if (this->type == String)
        return this->str;
    else
        throw Exception("Token is not a string");
}

} // namespace gmshparsercpp
