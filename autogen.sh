#! /bin/sh
aclocal
autoheader
automake --add-missing --foreign
autoconf
##./configure
echo "You are now ready to run './configure'"


