#!/bin/sh
# This script is generated by genbootstrap.sh
# to regenerate, run "make bootstrap"
mkdir -p obj/lib/bio
mkdir -p obj/lib/bld.sub
mkdir -p obj/lib/crypto
mkdir -p obj/lib/date
mkdir -p obj/lib/escfmt
mkdir -p obj/lib/fileutil
mkdir -p obj/lib/flate
mkdir -p obj/lib/http
mkdir -p obj/lib/inifile
mkdir -p obj/lib/iter
mkdir -p obj/lib/json
mkdir -p obj/lib/math
mkdir -p obj/lib/regex
mkdir -p obj/lib/std
mkdir -p obj/lib/sys
mkdir -p obj/lib/testr
mkdir -p obj/lib/thread
mkdir -p obj/mbld
pwd=`pwd`
set -x
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/config.myr
	as -g -o obj/mbld/cpufeatures.o mbld/cpufeatures+posixy-x64.s
	as -g -o obj/lib/thread/tls-impl.o lib/thread/tls-impl+fsbase-x64.s
	as -g -o obj/lib/thread/atomic-impl.o lib/thread/atomic-impl+x64.s
	as -g -o obj/lib/thread/exit.o lib/thread/exit+linux-x64.s
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/common.myr
	as -g -o obj/lib/std/getbp.o lib/std/getbp+posixy-x64.s
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/option.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/traits.myr
	as -g -o obj/lib/std/memops-impl.o lib/std/memops-impl+posixy-x64.s
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/fltbits.myr
	as -g -o obj/lib/std/sjlj-impl.o lib/std/sjlj-impl+posixy-x64.s
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/endian.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/extremum.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/sjlj+x64.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/swap.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/slfill.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/result.myr
	as -g -o obj/lib/sys/syscall.o lib/sys/syscall+linux-x64.s
	$pwd/6/6m -O obj lib/sys/systypes.myr
	as -g -o obj/lib/sys/util.o lib/sys/util+posixy-x64.s
	$pwd/6/6m -O obj lib/sys/syserrno+linux.myr
	$pwd/6/6m -O obj lib/sys/sys+linux-x64.myr
	$pwd/6/6m -O obj lib/sys/ifreq+linux.myr
	ar -rcs obj/lib/sys/libsys.a obj/lib/sys/sys.o obj/lib/sys/syserrno.o obj/lib/sys/util.o obj/lib/sys/systypes.o obj/lib/sys/ifreq.o obj/lib/sys/syscall.o
	$pwd/muse/muse -o obj/lib/sys/libsys.use -p sys obj/lib/sys/sys.use obj/lib/sys/syserrno.use obj/lib/sys/systypes.use obj/lib/sys/ifreq.use
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/types+fsbase.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/errno.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/pledge.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/types.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/strfind.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/memops.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/clear.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/sleq.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/hassuffix.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/syswrap-ss+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/sleep.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/backtrace+x64.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/units.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/cstrconv.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/syswrap+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/mkpath.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/now.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/consts.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/die.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/slcp.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/chartype.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/utf.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/cmp.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/sort.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/search.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/hasprefix.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/chomp.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/strstrip.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/introspect.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/varargs.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/wait+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/threadhooks.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/bytealloc.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/alloc.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/slurp.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/mk.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/slput.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/htab.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/slpush.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/striter.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/intparse.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/strsplit.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/strbuf.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/sldup.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/bigint.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/fltparse.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/fltfmt.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/dirname.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/dir+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/diriter.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/fndup.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/strjoin.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/getcwd.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/slpop.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/sljoin.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/strreplace.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/getint.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/hashfuncs.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/bitset.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/putint.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/readall.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/blat.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/writeall.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/fmt.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/env+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/execvp.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/spork.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/assert.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/rand.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/pathjoin.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/mktemp.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/optparse.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/netaddr.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/ipparse.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/fmtfuncs.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/resolve+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/dialparse+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/dial+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/listen+posixy.myr
	$pwd/6/6m -O obj -I obj/lib/sys lib/std/try.myr
	ar -rcs obj/lib/std/libstd.a obj/lib/std/resolve.o obj/lib/std/result.o obj/lib/std/try.o obj/lib/std/ipparse.o obj/lib/std/alloc.o obj/lib/std/sleq.o obj/lib/std/putint.o obj/lib/std/sljoin.o obj/lib/std/slpop.o obj/lib/std/syswrap.o obj/lib/std/getint.o obj/lib/std/strsplit.o obj/lib/std/slfill.o obj/lib/std/writeall.o obj/lib/std/fltfmt.o obj/lib/std/hasprefix.o obj/lib/std/swap.o obj/lib/std/fmt.o obj/lib/std/netaddr.o obj/lib/std/varargs.o obj/lib/std/diriter.o obj/lib/std/getcwd.o obj/lib/std/blat.o obj/lib/std/optparse.o obj/lib/std/pathjoin.o obj/lib/std/readall.o obj/lib/std/strjoin.o obj/lib/std/threadhooks.o obj/lib/std/sjlj.o obj/lib/std/extremum.o obj/lib/std/endian.o obj/lib/std/rand.o obj/lib/std/sldup.o obj/lib/std/sleep.o obj/lib/std/wait.o obj/lib/std/introspect.o obj/lib/std/fltparse.o obj/lib/std/fndup.o obj/lib/std/strbuf.o obj/lib/std/strreplace.o obj/lib/std/assert.o obj/lib/std/spork.o obj/lib/std/slpush.o obj/lib/std/strstrip.o obj/lib/std/htab.o obj/lib/std/hashfuncs.o obj/lib/std/slput.o obj/lib/std/sjlj-impl.o obj/lib/std/fltbits.o obj/lib/std/striter.o obj/lib/std/types.o obj/lib/std/cstrconv.o obj/lib/std/units.o obj/lib/std/backtrace.o obj/lib/std/syswrap-ss.o obj/lib/std/die.o obj/lib/std/mk.o obj/lib/std/hassuffix.o obj/lib/std/memops-impl.o obj/lib/std/pledge.o obj/lib/std/utf.o obj/lib/std/slurp.o obj/lib/std/dialparse.o obj/lib/std/bytealloc.o obj/lib/std/mktemp.o obj/lib/std/consts.o obj/lib/std/chomp.o obj/lib/std/dir.o obj/lib/std/search.o obj/lib/std/memops.o obj/lib/std/fmtfuncs.o obj/lib/std/strfind.o obj/lib/std/env.o obj/lib/std/dirname.o obj/lib/std/clear.o obj/lib/std/listen.o obj/lib/std/sort.o obj/lib/std/cmp.o obj/lib/std/now.o obj/lib/std/intparse.o obj/lib/std/traits.o obj/lib/std/mkpath.o obj/lib/std/option.o obj/lib/std/dial.o obj/lib/std/errno.o obj/lib/std/chartype.o obj/lib/std/bigint.o obj/lib/std/bitset.o obj/lib/std/getbp.o obj/lib/std/slcp.o obj/lib/std/execvp.o
	$pwd/muse/muse -o obj/lib/std/libstd.use -p std obj/lib/std/resolve.use obj/lib/std/result.use obj/lib/std/try.use obj/lib/std/ipparse.use obj/lib/std/alloc.use obj/lib/std/sleq.use obj/lib/std/putint.use obj/lib/std/sljoin.use obj/lib/std/slpop.use obj/lib/std/syswrap.use obj/lib/std/getint.use obj/lib/std/strsplit.use obj/lib/std/slfill.use obj/lib/std/writeall.use obj/lib/std/fltfmt.use obj/lib/std/hasprefix.use obj/lib/std/swap.use obj/lib/std/fmt.use obj/lib/std/netaddr.use obj/lib/std/varargs.use obj/lib/std/diriter.use obj/lib/std/getcwd.use obj/lib/std/blat.use obj/lib/std/optparse.use obj/lib/std/pathjoin.use obj/lib/std/readall.use obj/lib/std/strjoin.use obj/lib/std/threadhooks.use obj/lib/std/sjlj.use obj/lib/std/extremum.use obj/lib/std/endian.use obj/lib/std/rand.use obj/lib/std/sldup.use obj/lib/std/sleep.use obj/lib/std/wait.use obj/lib/std/introspect.use obj/lib/std/fltparse.use obj/lib/std/fndup.use obj/lib/std/strbuf.use obj/lib/std/strreplace.use obj/lib/std/assert.use obj/lib/std/spork.use obj/lib/std/slpush.use obj/lib/std/strstrip.use obj/lib/std/htab.use obj/lib/std/hashfuncs.use obj/lib/std/slput.use obj/lib/std/fltbits.use obj/lib/std/striter.use obj/lib/std/types.use obj/lib/std/cstrconv.use obj/lib/std/units.use obj/lib/std/backtrace.use obj/lib/std/syswrap-ss.use obj/lib/std/die.use obj/lib/std/mk.use obj/lib/std/hassuffix.use obj/lib/std/pledge.use obj/lib/std/utf.use obj/lib/std/slurp.use obj/lib/std/dialparse.use obj/lib/std/bytealloc.use obj/lib/std/mktemp.use obj/lib/std/consts.use obj/lib/std/chomp.use obj/lib/std/dir.use obj/lib/std/search.use obj/lib/std/memops.use obj/lib/std/fmtfuncs.use obj/lib/std/strfind.use obj/lib/std/env.use obj/lib/std/dirname.use obj/lib/std/clear.use obj/lib/std/listen.use obj/lib/std/sort.use obj/lib/std/cmp.use obj/lib/std/now.use obj/lib/std/intparse.use obj/lib/std/traits.use obj/lib/std/mkpath.use obj/lib/std/option.use obj/lib/std/dial.use obj/lib/std/errno.use obj/lib/std/chartype.use obj/lib/std/bigint.use obj/lib/std/bitset.use obj/lib/std/slcp.use obj/lib/std/execvp.use
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/types.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/regex/types.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/regex/interp.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/types.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/bio.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/iter.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/mem.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/fd.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/geti.myr
	$pwd/6/6m -O obj -I obj/lib/std -I obj/lib/sys lib/bio/puti.myr
	ar -rcs obj/lib/bio/libbio.a obj/lib/bio/puti.o obj/lib/bio/geti.o obj/lib/bio/fd.o obj/lib/bio/mem.o obj/lib/bio/bio.o obj/lib/bio/types.o obj/lib/bio/iter.o
	$pwd/muse/muse -o obj/lib/bio/libbio.use -p bio obj/lib/bio/puti.use obj/lib/bio/geti.use obj/lib/bio/fd.use obj/lib/bio/mem.use obj/lib/bio/bio.use obj/lib/bio/types.use obj/lib/bio/iter.use
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/ncpu+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/fsbase+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/tls+fsbase.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/spawn+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/atomic.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/futex+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/sem+futex.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/future.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/do.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/mutex+futex.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/condvar+linux.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/queue.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/hookstd.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/waitgrp+futex.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/thread/rwlock+futex.myr
	ar -rcs obj/lib/thread/libthread.a obj/lib/thread/atomic.o obj/lib/thread/future.o obj/lib/thread/hookstd.o obj/lib/thread/condvar.o obj/lib/thread/fsbase.o obj/lib/thread/rwlock.o obj/lib/thread/common.o obj/lib/thread/waitgrp.o obj/lib/thread/exit.o obj/lib/thread/spawn.o obj/lib/thread/queue.o obj/lib/thread/mutex.o obj/lib/thread/atomic-impl.o obj/lib/thread/sem.o obj/lib/thread/tls-impl.o obj/lib/thread/do.o obj/lib/thread/ncpu.o obj/lib/thread/futex.o obj/lib/thread/tls.o obj/lib/thread/types.o
	$pwd/muse/muse -o obj/lib/thread/libthread.use -p thread obj/lib/thread/atomic.use obj/lib/thread/future.use obj/lib/thread/hookstd.use obj/lib/thread/condvar.use obj/lib/thread/fsbase.use obj/lib/thread/rwlock.use obj/lib/thread/common.use obj/lib/thread/waitgrp.use obj/lib/thread/spawn.use obj/lib/thread/queue.use obj/lib/thread/mutex.use obj/lib/thread/sem.use obj/lib/thread/do.use obj/lib/thread/ncpu.use obj/lib/thread/futex.use obj/lib/thread/tls.use obj/lib/thread/types.use
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/opts.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/syssel.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/libs.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/util.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/build.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/install.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/parse.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/regex/ranges.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std lib/regex/compile.myr
	ar -rcs obj/lib/regex/libregex.a obj/lib/regex/interp.o obj/lib/regex/ranges.o obj/lib/regex/types.o obj/lib/regex/compile.o
	$pwd/muse/muse -o obj/lib/regex/libregex.use -p regex obj/lib/regex/interp.use obj/lib/regex/ranges.use obj/lib/regex/types.use obj/lib/regex/compile.use
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/subtest.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/test.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/deps.myr
	$pwd/6/6m -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/main.myr
	ld --gc-sections -o obj/mbld/mbld $pwd/rt/_myrrt.o obj/mbld/deps.o obj/mbld/main.o obj/mbld/util.o obj/mbld/cpufeatures.o obj/mbld/libs.o obj/mbld/syssel.o obj/mbld/config.o obj/mbld/opts.o obj/mbld/subtest.o obj/mbld/types.o obj/mbld/test.o obj/mbld/install.o obj/mbld/parse.o obj/mbld/build.o -Lobj/lib/thread -lthread -Lobj/lib/bio -lbio -Lobj/lib/regex -lregex -Lobj/lib/std -lstd -Lobj/lib/sys -lsys
true
