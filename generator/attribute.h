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

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include "token.h"

class Attribute
{
public:
    Attribute(Token::TokenType type, const std::string &identifier, bool readonly = false):
        m_type(type),
        m_identifier(identifier),
        m_readonly(readonly)
    {}

    Attribute(const Attribute &copy):
        m_type(copy.m_type),
        m_identifier(copy.m_identifier),
        m_readonly(copy.m_readonly)
    {}

    ~Attribute()
    {}

    Token::TokenType getType() const { return m_type; }
    std::string getIdentifier() const { return m_identifier; }

    Attribute &operator=(const Attribute &rhs)
    {
        m_type = rhs.m_type;
        m_identifier = rhs.m_identifier;
        m_readonly = rhs.m_readonly;
        return *this;
    }

private:
    Token::TokenType m_type;
    std::string m_identifier;
    bool m_readonly;
};

#endif // ATTRIBUTE_H
