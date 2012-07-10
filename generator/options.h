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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

class Options
{
public:
    Options(int argc, char **argv);
    ~Options();

    bool help() const { return m_help; }
    bool good() const { return m_good; }
    void printHelp() const;
    std::string inputFilename() const { return m_input_filename; }
    std::string outputFilename() const { return m_output_filename; }

private:
    bool m_help;
    bool m_good;
    std::string m_input_filename;
    std::string m_output_filename;
    const std::string m_bin_name;
};

#endif // OPTIONS_H
