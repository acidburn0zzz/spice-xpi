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

#include "redirecthelper.h"

RedirectHelper::RedirectHelper(const Options &o):
    m_input_file(o.inputFilename()),
    m_output_file(o.outputFilename()),
    m_ifb(),
    m_ofb(),
    m_cin_streambuf(NULL),
    m_cout_streambuf(NULL)
{
}

RedirectHelper::~RedirectHelper()
{
    restore();
}

bool RedirectHelper::redirect()
{
    if (!m_cin_streambuf && !m_input_file.empty()) {
        m_ifb.open(m_input_file.c_str(), std::ios::in);
        if (!m_ifb.is_open()) {
            std::cerr << "Unable to open '" << m_input_file << "' for reading!\n";
            return false;
        } else {
            m_cin_streambuf = std::cin.rdbuf();
            std::istream is(&m_ifb);
            std::cin.rdbuf(is.rdbuf());
        }
    }

    if (!m_cout_streambuf && !m_output_file.empty()) {
        m_ofb.open(m_output_file.c_str(), std::ios::out);
        if (!m_ofb.is_open()) {
            std::cerr << "Unable to open '" << m_output_file << "' for writing!\n";
            return false;
        } else {
            m_cout_streambuf = std::cout.rdbuf();
            std::ostream os(&m_ofb);
            std::cout.rdbuf(os.rdbuf());
        }
    }

    return true;
}

void RedirectHelper::restore()
{
    if (m_cin_streambuf) {
        m_ifb.close();
        std::cin.rdbuf(m_cin_streambuf);
        m_cin_streambuf = NULL;
    }

    if (m_cout_streambuf) {
        m_ofb.close();
        std::cout.rdbuf(m_cout_streambuf);
        m_cout_streambuf = NULL;
    }
}
