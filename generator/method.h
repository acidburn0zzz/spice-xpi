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

#ifndef METHOD_H
#define METHOD_H

#include <string>
#include <list>
#include "token.h"

class Method
{
public:
    class MethodParam;

public:
    Method(Token::TokenType type, std::string &identifier, const std::list<MethodParam> &params):
        m_type(type),
        m_identifier(identifier),
        m_params(params)
    {}

    Method(const Method &copy):
        m_type(copy.m_type),
        m_identifier(copy.m_identifier),
        m_params(copy.m_params)
    {}

    ~Method()
    {}

    Token::TokenType getType() const { return m_type; }
    std::string getIdentifier() const { return m_identifier; }
    std::list<MethodParam> getParams() const { return m_params; }

    Method &operator=(const Method &rhs)
    {
        m_type = rhs.m_type;
        m_identifier = rhs.m_identifier;
        m_params = rhs.m_params;
        return *this;
    }

private:
    Token::TokenType m_type;
    std::string m_identifier;
    std::list<MethodParam> m_params;
};

class Method::MethodParam
{
public:
    MethodParam(Token::TokenType dir, Token::TokenType type, const std::string &identifier):
        m_dir(dir),
        m_type(type),
        m_identifier(identifier)
    {}

    MethodParam(const MethodParam &copy):
        m_dir(copy.m_dir),
        m_type(copy.m_type),
        m_identifier(copy.m_identifier)
    {}

    ~MethodParam()
    {}

    Token::TokenType getType() const { return m_type; }
    Token::TokenType getDir() const { return m_dir; }
    std::string getIdentifier() const { return m_identifier; }

    MethodParam &operator=(const MethodParam &rhs)
    {
        m_dir = rhs.m_dir;
        m_type = rhs.m_type;
        m_identifier = rhs.m_identifier;
        return *this;
    }

private:
    Token::TokenType m_dir;
    Token::TokenType m_type;
    std::string m_identifier;
};

#endif // METHOD_H
