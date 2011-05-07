#!/bin/sh

case "$0" in
  /*) srcdir=`dirname "$0"`;;
  */*) srcdir=`pwd`/`dirname "$0"`;;
  *) srcdir=".";;
esac

LDFLAGS="-L$PSL1GHT/ppu/lib -L$PS3DEV/portlibs/ppu/lib -lrt -llv2"
CFLAGS="-I$PSL1GHT/ppu/include/SDL"
PATH="$PS3DEV/bin":"$PS3DEV/portlibs/ppu/bin":"$PATH"

export LDFLAGS
export CFLAGS
export PATH

"$srcdir"/configure --host=ppu --without-x --datarootdir=/dev_hdd0/game/FDROID001/USRDIR

