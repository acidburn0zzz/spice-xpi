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

#include <iostream>
#include <list>
#include "parser.h"

Parser::Parser():
    m_scanner(),
    m_token(),
    m_attributes(),
    m_methods()
{
}

Parser::~Parser()
{
}

bool Parser::parse()
{
    m_token = m_scanner.getNextToken();
    while (1) {
        if (m_token == Token::T_INTERFACE || m_token == Token::T_OPEN_BRACKET) {
            if (!parseDefinition())
                return false;
        } else if (m_token == Token::T_HASH) {
            if (!parseInclude())
                return false;
        } else if (m_token == Token::T_EOF) {
            return true;
        } else {
            handleError();
            return false;
        }
    }
}

void Parser::handleError() const
{
    std::cerr << (m_token == Token::T_LEX_ERROR ?
        "Lexical error near line: " : "Syntax error near line: ");
    std::cerr << m_scanner.getLineNo() << std::endl;
}

bool Parser::parseInclude()
{
    if (m_token != Token::T_HASH) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_INCLUDE) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (m_token == Token::T_QUOTE) {
        m_token = m_scanner.getNextToken();
        while (m_token == Token::T_IDENTIFIER || m_token == Token::T_DOT)
            m_token = m_scanner.getNextToken();
        if (m_token != Token::T_QUOTE) {
            handleError();
            return false;
        }
        m_token = m_scanner.getNextToken();
    } else if (m_token == Token::T_LESS) {
        m_token = m_scanner.getNextToken();
        while (m_token == Token::T_IDENTIFIER || m_token == Token::T_DOT)
            m_token = m_scanner.getNextToken();
        if (m_token != Token::T_GREATER) {
            handleError();
            return false;
        }
        m_token = m_scanner.getNextToken();
    } else {
        handleError();
        return false;
    }

    return true;
}

bool Parser::parseDefinitionParams()
{
    if (m_token != Token::T_OPEN_BRACKET) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    while (1) {
        if (m_token == Token::T_UUID) {
            m_token = m_scanner.getNextToken();
            if (m_token != Token::T_OPEN_PARENTHESES) {
                handleError();
                return false;
            }
            m_scanner.setAcceptUuids();
            m_token = m_scanner.getNextToken();
            if (m_token != Token::T_UUIDVAL) {
                handleError();
                return false;
            }
            m_scanner.setAcceptUuids(false);
            m_token = m_scanner.getNextToken();
            if (m_token != Token::T_CLOSE_PARENTHESES) {
                handleError();
                return false;
            }
        } else if (m_token != Token::T_IDENTIFIER) {
            handleError();
            return false;
        }

        m_token = m_scanner.getNextToken();
        if (m_token == Token::T_CLOSE_BRACKET) {
            m_token = m_scanner.getNextToken();
            return true;
        }
        else if (m_token != Token::T_COMMA) {
            handleError();
            return false;
        }

        m_token = m_scanner.getNextToken();
    }
}

bool Parser::parseDefinition()
{
    if (m_token == Token::T_OPEN_BRACKET && !parseDefinitionParams())
        return false;

    if (m_token != Token::T_INTERFACE) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_IDENTIFIER) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (m_token == Token::T_COLON) {
        m_token = m_scanner.getNextToken();
        if (!parseBasicInterfaces())
            return false;
    }

    if (m_token != Token::T_OPEN_BRACE) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (!parseInterfaceBody())
        return false;

    if (m_token != Token::T_CLOSE_BRACE) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_SEMICOLON) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    return true;
}

bool Parser::parseBasicInterfaces()
{
    while (1) {
        if (m_token != Token::T_IDENTIFIER) {
            handleError();
            return false;
        }

        m_token = m_scanner.getNextToken();
        if (m_token == Token::T_OPEN_BRACE)
            return true;

        if (m_token != Token::T_COMMA) {
            handleError();
            return false;
        }

        m_token = m_scanner.getNextToken();
    }
}

bool Parser::parseInterfaceBody()
{
    while (1) {
        switch (m_token.getType()) {
        case Token::T_READONLY:
        case Token::T_ATTRIBUTE:
            if (!parseAttribute())
                return false;
            break;
        case Token::T_FLOAT:
        case Token::T_DOUBLE:
        case Token::T_STRING:
        case Token::T_WSTRING:
        case Token::T_UNSIGNED:
        case Token::T_SHORT:
        case Token::T_LONG:
        case Token::T_CHAR:
        case Token::T_WCHAR:
        case Token::T_BOOLEAN:
        case Token::T_VOID:
        case Token::T_OCTET:
            if (!parseMethod())
                return false;
            break;
        case Token::T_CLOSE_BRACE:
            break;
        default:
            handleError();
            return false;
        }

        if (m_token == Token::T_LEX_ERROR) {
            handleError();
            return false;
        }

        if (m_token == Token::T_CLOSE_BRACE)
            return true;
    }
    return true;
}

bool Parser::parseAttribute()
{
    bool readonly = false;
    if (m_token == Token::T_READONLY) {
        readonly = true;
        m_token = m_scanner.getNextToken();
    }

    if (m_token != Token::T_ATTRIBUTE) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (!parseType())
        return false;

    Token type = m_token;

    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_IDENTIFIER) {
        handleError();
        return false;
    }

    m_attributes.push_back(Attribute(type.getType(), m_token.getParameter(), readonly));

    m_token = m_scanner.getNextToken();
    if (m_token == Token::T_COMMA) {
        while (1) {
            m_token = m_scanner.getNextToken();
            if (m_token != Token::T_IDENTIFIER) {
                handleError();
                return false;
            }

            m_attributes.push_back(Attribute(type.getType(), m_token.getParameter(), readonly));
            m_token = m_scanner.getNextToken();
            if (m_token == Token::T_SEMICOLON) {
                break;
            } else if (m_token != Token::T_COMMA) {
                handleError();
                return false;
            }
        }
    }
    if (m_token != Token::T_SEMICOLON) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    return true;
}

bool Parser::parseMethod()
{
    if (!parseType())
        return false;

    Token type = m_token;
    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_IDENTIFIER) {
        handleError();
        return false;
    }

    std::string identifier = m_token.getParameter();

    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_OPEN_PARENTHESES) {
        handleError();
        return false;
    }

    std::list<Method::MethodParam> params;
    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_CLOSE_PARENTHESES) {
        while (1) {
            Token dir(Token::T_UNKNOWN);
            if (m_token == Token::T_IN ||
                m_token == Token::T_OUT ||
                m_token == Token::T_INOUT)
            {
                dir = m_token;
                m_token = m_scanner.getNextToken();
            }
            if (!parseType())
                return false;
            Token type = m_token;
            m_token = m_scanner.getNextToken();
            if (m_token != Token::T_IDENTIFIER) {
                handleError();
                return false;
            }

            params.push_back(Method::MethodParam(dir.getType(), type.getType(),
                m_token.getParameter()));

            m_token = m_scanner.getNextToken();
            if (m_token == Token::T_COMMA) {
                m_token = m_scanner.getNextToken();
            } else if (m_token == Token::T_CLOSE_PARENTHESES) {
                break;
            } else {
                handleError();
                return false;
            }
        }
    }

    if (m_token != Token::T_CLOSE_PARENTHESES) {
        handleError();
        return false;
    }

    m_token = m_scanner.getNextToken();
    if (m_token != Token::T_SEMICOLON) {
        handleError();
        return false;
    }

    m_methods.push_back(Method(type.getType(), identifier, params));

    m_token = m_scanner.getNextToken();
    return true;
}

bool Parser::parseType()
{
    switch (m_token.getType()) {
    case Token::T_UNSIGNED: {
        Token new_token = m_scanner.getNextToken();
        if (new_token == Token::T_SHORT)
            m_token = Token(Token::T_UNSIGNED_SHORT);
        else if (new_token == Token::T_LONG) {
            Token newest_token = m_scanner.getNextToken();
            if (newest_token == Token::T_LONG) {
                m_token = Token(Token::T_UNSIGNED_LONG_LONG);
            } else {
                m_scanner.pushToken(newest_token);
                m_token = Token(Token::T_UNSIGNED_LONG);
            }
        } else {
            m_scanner.pushToken(new_token);
            handleError();
            return false;
        }
        break;
    }
    case Token::T_LONG: {
        Token new_token = m_scanner.getNextToken();
        if (new_token == Token::T_LONG)
            m_token = Token(Token::T_LONG_LONG);
        else
            m_scanner.pushToken(new_token);
        break;
    }
    case Token::T_FLOAT:
    case Token::T_DOUBLE:
    case Token::T_STRING:
    case Token::T_WSTRING:
    case Token::T_SHORT:
    case Token::T_CHAR:
    case Token::T_WCHAR:
    case Token::T_BOOLEAN:
    case Token::T_VOID:
    case Token::T_OCTET:
        return true;
    default:
        handleError();
        return false;
    }
    return true;
}
