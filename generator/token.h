/* ***** BEGIN LICENSE BLOCK *****
*   Copyright (C) 2012, Peter Hatina <phatina@redhat.com>
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License as
*   published by the Free Software Foundation; either version 2 of
*   the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
* ***** END LICENSE BLOCK ***** */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{
public:
    typedef enum {
        T_UNKNOWN, T_ATTRIBUTE, T_CONST, T_IN, T_OUT, T_INOUT,
        T_INTERFACE, T_NATIVE, T_RAISES, T_TYPEDEF, T_UUID, T_UUIDVAL,
        T_OPEN_BRACKET, T_CLOSE_BRACKET, T_OPEN_BRACE, T_CLOSE_BRACE,
        T_OPEN_PARENTHESES, T_CLOSE_PARENTHESES, T_COMMA, T_COLON,
        T_ASSIGN, T_SEMICOLON, T_INCLUDE, T_READONLY, T_FLOAT, T_DOUBLE,
        T_STRING, T_WSTRING, T_UNSIGNED, T_SHORT, T_LONG, T_CHAR, T_WCHAR,
        T_BOOLEAN, T_VOID, T_OCTET, T_DOUBLECOLON, T_PIPE, T_CARET, T_AMPERSAND,
        T_SHIFT_LEFT, T_SHIFT_RIGHT, T_PLUS, T_MINUS, T_ASTERISK, T_SLASH,
        T_PERCENT, T_TILDE, T_IDENTIFIER, T_EOF, T_NUMBER, T_HASH,
        T_LEX_ERROR, T_UNSIGNED_SHORT, T_UNSIGNED_LONG, T_UNSIGNED_LONG_LONG,
        T_LONG_LONG, T_LESS, T_GREATER, T_QUOTE, T_DOT
    } TokenType;

public:
    Token():
        m_type(T_UNKNOWN),
        m_param()
    {}

    Token(TokenType type, const std::string &parameter = std::string()):
        m_type(type),
        m_param(parameter)
    {}

    Token(const Token &copy):
        m_type(copy.m_type),
        m_param(copy.m_param)
    {}

    ~Token()
    {}

    TokenType getType() const { return m_type; }
    std::string getParameter() const { return m_param; }

    Token &operator= (const Token& rhs)
    {
        m_type = rhs.m_type;
        m_param = rhs.m_param;
        return *this;
    }

    bool operator==(Token::TokenType type) const { return m_type == type; }
    bool operator!=(Token::TokenType type) const { return m_type != type; }

private:
    TokenType m_type;
    std::string m_param;
};

#endif // TOKEN_H
