#!/bin/sh
set -x

if test `uname` = Plan9; then
    export MYR_MUSE=`pwd`/muse/6.out
    export MYR_MC=`pwd`/6/6.out
    export MYR_RT=`pwd`/rt/_myrrt.6
else
    export MYR_MUSE=`pwd`/muse/muse
    export MYR_MC=`pwd`/6/6m
    export MYR_RT=`pwd`/rt/_myrrt.o
fi

# build without an obj/ dir, so we don't need to deal with
# creating the heirarchy for the compiler. mbld usually
# deals with that, but the bootstrap doesn't have it.
./mbldwrap.sh
cp obj/mbld/mbld xmbld
./xmbld -o '' clean

tags(){
	case `uname` in
	*Linux*)	echo -Tposixy -Tlinux -Tfsbase -Tfutex;;
	*Darwin*)	echo -Tposixy -Tosx -Tfutex;;
	*FreeBSD*)	echo -Tposixy -Tfreebsd -Tfsbase -Tfutex;;
	*NetBSD*)	echo -Tposixy -Tnetbsd;;
	*OpenBSD*)	echo -Tposixy -Topenbsd:6.4 -Tfsbase -Tfutex;;
	*Plan9*)	echo -Tplan9;;
	esac
	case `uname -m` in
	*amd64*)	echo -Tx64	;;
	*x86_64*)	echo -Tx64	;;
	esac
}


bootscript=mk/bootstrap/bootstrap+`uname -s`-`uname -m`.sh
echo '#!/bin/sh' > $bootscript
echo '# This script is generated by genbootstrap.sh' >> $bootscript
echo '# to regenerate, run "make bootstrap"' >> $bootscript
echo '#######################################'

# we output into obj; things can fail if we don't create those dirs

echo 'mkdir -p obj/lib/bio' >> $bootscript
echo 'mkdir -p obj/lib/bld.sub' >> $bootscript
echo 'mkdir -p obj/lib/crypto' >> $bootscript
echo 'mkdir -p obj/lib/date' >> $bootscript
echo 'mkdir -p obj/lib/escfmt' >> $bootscript
echo 'mkdir -p obj/lib/fileutil' >> $bootscript
echo 'mkdir -p obj/lib/flate' >> $bootscript
echo 'mkdir -p obj/lib/http' >> $bootscript
echo 'mkdir -p obj/lib/inifile' >> $bootscript
echo 'mkdir -p obj/lib/iter' >> $bootscript
echo 'mkdir -p obj/lib/json' >> $bootscript
echo 'mkdir -p obj/lib/math' >> $bootscript
echo 'mkdir -p obj/lib/regex' >> $bootscript
echo 'mkdir -p obj/lib/std' >> $bootscript
echo 'mkdir -p obj/lib/sys' >> $bootscript
echo 'mkdir -p obj/lib/testr' >> $bootscript
echo 'mkdir -p obj/lib/thread' >> $bootscript
echo 'mkdir -p obj/mbld' >> $bootscript

echo 'pwd=`pwd`' >> $bootscript
echo 'set -x' >> $bootscript
# mbld needs to be run without an output dir so we dont
# run into mkdir issues.
./xmbld -o '' -j1 -Bnone mbld:mbld `tags` | \
    grep '^	' | \
    sed "s:`pwd`:\$pwd:g" | \
    tee -a $bootscript
echo 'true' >> $bootscript
chmod +x $bootscript
rm ./xmbld
