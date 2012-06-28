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

#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <string>
#include <map>
#include <queue>
#include "token.h"

class Scanner
{
public:
    Scanner();
    ~Scanner();

    Token getNextToken();
    int getLineNo() const { return m_line_no_start; }
    void pushToken(Token &token) { m_token_queue.push(token); }
    void setAcceptUuids(bool accept = true) { m_accept_uuids = accept; }

private:
    static void initKeywords();

private:
    Token m_token;
    int m_line_no_start;
    int m_line_no_end;
    bool m_accept_uuids;
    std::queue<Token> m_token_queue;
    static std::map<std::string, Token::TokenType> s_keywords;
};

#endif // SCANNER_H
