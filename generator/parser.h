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

#ifndef PARSER_H
#define PARSER_H

#include <list>
#include "attribute.h"
#include "method.h"
#include "scanner.h"
#include "token.h"

class Parser
{
public:
    Parser();
    ~Parser();

    bool parse();

    std::list<Attribute> getAttributes() const { return m_attributes; }
    std::list<Method> getMethods() const { return m_methods; }

private:
    void handleError() const;
    bool parseInclude();
    bool parseDefinitionParams();
    bool parseDefinition();
    bool parseBasicInterfaces();
    bool parseInterfaceBody();
    bool parseAttribute();
    bool parseType();
    bool parseMethod();

private:
    Scanner m_scanner;
    Token m_token;
    std::list<Attribute> m_attributes;
    std::list<Method> m_methods;
};

#endif // PARSER_H
