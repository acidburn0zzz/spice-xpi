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

#ifndef REDIRECTHELPER_H
#define REDIRECTHELPER_H

#include <iostream>
#include <fstream>
#include <string>
#include "options.h"

class RedirectHelper
{
public:
    RedirectHelper(const Options &o);
    ~RedirectHelper();

    bool redirect();
    void restore();

private:
    std::string m_input_file;
    std::string m_output_file;
    std::filebuf m_ifb;
    std::filebuf m_ofb;
    std::streambuf *m_cin_streambuf;
    std::streambuf *m_cout_streambuf;
};

#endif // REDIRECTHELPER_H
