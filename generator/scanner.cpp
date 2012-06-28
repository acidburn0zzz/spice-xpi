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

#include <fstream>
#include <cctype>
#include "scanner.h"

std::map<std::string, Token::TokenType> Scanner::s_keywords;

Scanner::Scanner():
    m_token(),
    m_line_no_start(1),
    m_line_no_end(m_line_no_start),
    m_accept_uuids(false),
    m_token_queue()
{
    if (s_keywords.empty())
        initKeywords();
}

Scanner::~Scanner()
{
}

void Scanner::initKeywords()
{
    s_keywords["attribute"] = Token::T_ATTRIBUTE;
    s_keywords["const"] = Token::T_CONST;
    s_keywords["in"] = Token::T_IN;
    s_keywords["out"] = Token::T_OUT;
    s_keywords["inout"] = Token::T_INOUT;
    s_keywords["interface"] = Token::T_INTERFACE;
    s_keywords["native"] = Token::T_NATIVE;
    s_keywords["raises"] = Token::T_RAISES;
    s_keywords["readonly"] = Token::T_READONLY;
    s_keywords["typedef"] = Token::T_TYPEDEF;
    s_keywords["uuid"] = Token::T_UUID;
    s_keywords["float"] = Token::T_FLOAT;
    s_keywords["double"] = Token::T_DOUBLE;
    s_keywords["string"] = Token::T_STRING;
    s_keywords["wstring"] = Token::T_WSTRING;
    s_keywords["unsigned"] = Token::T_UNSIGNED;
    s_keywords["short"] = Token::T_SHORT;
    s_keywords["long"] = Token::T_LONG;
    s_keywords["char"] = Token::T_CHAR;
    s_keywords["wchar"] = Token::T_WCHAR;
    s_keywords["boolean"] = Token::T_BOOLEAN;
    s_keywords["void"] = Token::T_VOID;
    s_keywords["octet"] = Token::T_OCTET;
    s_keywords["include"] = Token::T_INCLUDE;
}

Token Scanner::getNextToken()
{
    if (!m_token_queue.empty()) {
        Token t(m_token_queue.front());
        m_token_queue.pop();
        return t;
    }

    enum { S_INITIAL, S_IDENTIFIER,
           S_NUMBER, S_COLON,
           S_SHIFT_LEFT, S_SHIFT_RIGHT,
           S_SLASH, S_BLOCK_COMMENT,
           S_UUID } state = S_INITIAL;
    std::string param;
    int c = 0;
    int old_c;

    m_line_no_start = m_line_no_end;
    while (1) {
        old_c = c;
        c = std::cin.get();

        if (std::cin.eof())
            return Token(state == S_INITIAL ? Token::T_EOF : Token::T_LEX_ERROR);

        if (c == '\n')
            ++m_line_no_end;

        switch (state) {
        case S_INITIAL:
            if (isalpha(c)) {
                param += c;
                state = m_accept_uuids ? S_UUID : S_IDENTIFIER;
            } else if (isdigit(c)) {
                param += c;
                state = m_accept_uuids ? S_UUID : S_NUMBER;
            }
            switch (c) {
            case '[':
                return Token(Token::T_OPEN_BRACKET);
            case ']':
                return Token(Token::T_CLOSE_BRACKET);
            case '{':
                return Token(Token::T_OPEN_BRACE);
            case '}':
                return Token(Token::T_CLOSE_BRACE);
            case '(':
                return Token(Token::T_OPEN_PARENTHESES);
            case ')':
                return Token(Token::T_CLOSE_PARENTHESES);
            case ',':
                return Token(Token::T_COMMA);
            case '=':
                return Token(Token::T_ASSIGN);
            case ';':
                return Token(Token::T_SEMICOLON);
            case '|':
                return Token(Token::T_PIPE);
            case '^':
                return Token(Token::T_CARET);
            case '&':
                return Token(Token::T_AMPERSAND);
            case '+':
                return Token(Token::T_PLUS);
            case '-':
                return Token(Token::T_MINUS);
            case '%':
                return Token(Token::T_PERCENT);
            case '~':
                return Token(Token::T_TILDE);
            case '#':
                return Token(Token::T_HASH);
            case '"':
                return Token(Token::T_QUOTE);
            case '.':
                return Token(Token::T_DOT);
            case ':':
                state = S_COLON;
                break;
            case '<':
                state = S_SHIFT_LEFT;
                break;
            case '>':
                state = S_SHIFT_RIGHT;
                break;
            case '/':
                state = S_SLASH;
                break;
            case ' ':
            case '\t':
                break;
            case '*':
                return Token(Token::T_LEX_ERROR);
            }
            break;

        case S_IDENTIFIER:
            if (!isalnum(c) && c != '-' && c != '_') {
                std::cin.unget();
                std::map<std::string, Token::TokenType>::iterator it;
                it = s_keywords.find(param);
                if (it != s_keywords.end())
                    return Token(it->second);
                return Token(Token::T_IDENTIFIER, param);
            }
            param += c;
            break;

        case S_NUMBER:
            if (!isdigit(c)) {
                std::cin.unget();
                return Token(Token::T_NUMBER, param);
            }
            param += c;
            break;

        case S_UUID: {
            if (!isdigit(c) && c != '-' && (c > 'f' || c < 'a')) {
                std::cin.unget();
                return Token(param.size() != 36 ? Token::T_LEX_ERROR : Token::T_UUIDVAL, param);
            }
            const int len = param.size();
            if (c != '-' && len >= 36 &&
               (len == 8 || len == 13 || len == 18 || len == 23))
            {
                return Token(Token::T_LEX_ERROR);
            }
            param += c;
            break;
        }

        case S_COLON:
            if (c == ':')
                return Token(Token::T_DOUBLECOLON);
            std::cin.unget();
            return Token(Token::T_COLON);

        case S_SHIFT_LEFT:
            if (c == '<')
                return Token(Token::T_SHIFT_LEFT);
            std::cin.unget();
            return Token(Token::T_LESS);

        case S_SHIFT_RIGHT:
            if (c == '>')
                return Token(Token::T_SHIFT_RIGHT);
            std::cin.unget();
            return Token(Token::T_GREATER);

        case S_SLASH:
            if (c == '/') {
                while (c != '\n' && !std::cin.eof())
                    c = std::cin.get();
                state = S_INITIAL;
            } else if (c == '*') {
                state = S_BLOCK_COMMENT;
            }
            break;

        case S_BLOCK_COMMENT:
            if (old_c == '*' && c == '/')
                state = S_INITIAL;
            break;
        }
    }
    return Token(Token::T_LEX_ERROR);
}
