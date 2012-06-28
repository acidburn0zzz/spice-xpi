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

#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <map>
#include <set>
#include "attribute.h"
#include "method.h"
#include "token.h"

class Generator
{
public:
    Generator(const std::list<Attribute> &attributes,
              const std::list<Method> &methods);
    ~Generator();

    void generate();

private:
    void init();
    void generateHeader();
    void generateFooter();
    void generateConnectVars();
    void generateContent();

    static std::string lowerString(const std::string &str);
    static std::string splitIdentifier(const std::string &str);
    static std::string attributeDefaultValue(const Attribute &attr);
    static std::string attributeToHtmlElement(const Attribute &attr);
    static bool attributeEnabled(const Attribute &attr);
    static bool methodEnabled(const Method &method, const Method::MethodParam &param);

private:
    std::list<Attribute> m_attributes;
    std::list<Method> m_methods;
    static std::set<std::string> s_default_attributes;
    static std::set<std::string> s_default_methods;
    static std::map<std::string, std::string> s_default_attribute_values;
};

#endif // GENERATOR_H
