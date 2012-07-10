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
#include <cstring>
extern "C" {
#  include <getopt.h>
}
#include "options.h"

Options::Options(int argc, char **argv):
    m_help(false),
    m_good(true),
    m_input_filename(),
    m_output_filename(),
    m_bin_name(argv && argv[0] ? basename(argv[0]) : "spice-xpi-generator")
{
    static struct option longopts[] = {
        { "input",  required_argument, NULL, 'i' },
        { "output", required_argument, NULL, 'o' },
        { "help",   no_argument,       NULL, 'h' },
        { NULL,     0,                 NULL,  0  }
    };

    int c;
    while ((c = getopt_long(argc, argv, "i:o:h", longopts, NULL)) != -1) {
        switch (c) {
        case 'i':
            m_input_filename = optarg;
            break;
        case 'o':
            m_output_filename = optarg;
            break;
        case 'h':
            m_help = true;
            break;
        default:
            m_good = false;
            break;
        }
    }
}

Options::~Options()
{
}

void Options::printHelp() const
{
    std::cout << "Spice-xpi test page generator\n\n"
              << "Usage: " << m_bin_name << " [-h] [-i input] [-o output]\n\n"
              << "Application options:\n"
              << "  -i, --input     input filename (stdin used, if not specified)\n"
              << "  -o, --output    output filename (stdout used, if not specified)\n"
              << "  -h, --help      prints this help\n";
}
