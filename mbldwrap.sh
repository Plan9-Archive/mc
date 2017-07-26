#!/bin/sh

# this should be a bourne compatible shell script.
export PATH=`pwd`:`pwd`/6:`pwd`/muse:$PATH
if test `uname` = Plan9; then
	O=6
	export MYR_MUSE=`pwd`/muse/$O.out
	export MYR_MC=`pwd`/6/$O.out
	export MYR_RT=`pwd`/rt/_myrrt.$O
	BOOT="./mk/bootstrap/bootstrap+`uname -s`-`uname -m`.sh"
else
	export MYR_MUSE=muse
	export MYR_MC=6m
	export MYR_RT=`pwd`/rt/_myrrt.o
	BOOT="./mk/bootstrap/bootstrap+`uname -s`-`uname -m`.sh"
fi

if [ -f mbld/mbld ] || [ -f obj/mbld/mbld ]; then
    ./obj/mbld/mbld $@ || ./mbld/mbld $@ || mbld $@ || \
        (echo "Unable to run mbld $@; have you build successfully"; false)
else
    mbld || $BOOT
fi
