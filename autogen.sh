#!/bin/sh
set -e # exit on errors

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

mkdir -p "$srcdir"/m4

git submodule init
git submodule update

autoreconf -vfi "$srcdir"

if [ -z "$NOCONFIGURE" ]; then
    "$srcdir"/configure --enable-maintainer-mode ${1+"$@"}
fi
