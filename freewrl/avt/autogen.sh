#! /bin/sh
aclocal
autoconf
automake --add-missing --copy --gnu > /dev/null 2>&1
automake --add-missing --copy --gnu > /dev/null 2>&1
./configure
make
make test

