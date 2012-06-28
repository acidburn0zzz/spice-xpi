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

#include "generator.h"
#include "options.h"
#include "parser.h"
#include "redirecthelper.h"

int main(int argc, char **argv)
{
    Options o(argc, argv);
    if (!o.good())
        return 1;

    if (o.help()) {
        o.printHelp();
        return 0;
    }

    RedirectHelper rh(o);
    if (!rh.redirect())
        return 1;

    Parser p;
    if (!p.parse())
        return 1;

    Generator g(p.getAttributes(), p.getMethods());
    g.generate();

    rh.restore();
    return 0;
}
