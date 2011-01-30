#!/bin/sh

case "$0" in
  /*) srcdir=`dirname "$0"`;;
  */*) srcdir=`pwd`/`dirname "$0"`;;
  *) srcdir=".";;
esac

LDFLAGS="-B$PSL1GHT/target/lib -B$PS3DEV/host/ppu/lib $PS3DEV/host/ppu/ppu/lib/lv2-psl1ght.o -lpsl1ght -llv2 -L$PS3DEV/ppu/lib"
CFLAGS="-I$PS3DEV/ppu/include/SDL"
PATH="$PS3DEV/host/ppu/bin":"$PATH"

export LDFLAGS
export CFLAGS
export PATH

"$srcdir"/configure --host=ppu --without-x --datarootdir=/dev_hdd0/game/FDROID001/USRDIR

