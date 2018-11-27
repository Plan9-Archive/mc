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
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/config+plan9-x64.myr
	6a -o obj/mbld/cpufeatures.6 mbld/cpufeatures+plan9-x64.s
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/common.myr
	6a -o obj/lib/thread/atomic-impl.6 lib/thread/atomic-impl+plan9-x64.s
	6a -o obj/lib/std/getbp.6 lib/std/getbp+plan9-x64.s
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/errno+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/option.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/traits.myr
	6a -o obj/lib/std/memops-impl.6 lib/std/memops-impl+plan9-x64.s
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/fltbits.myr
	6a -o obj/lib/std/sjlj-impl.6 lib/std/sjlj-impl+plan9-x64.s
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/endian.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/extremum.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/sjlj+x64.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/swap.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/slfill.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/result.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/resolve+plan9.myr
	6a -o obj/lib/sys/syscall.6 lib/sys/syscall+plan9-x64.s
	$pwd/6/6.out -O obj lib/sys/ifreq+plan9.myr
	$pwd/6/6.out -O obj lib/sys/systypes.myr
	$pwd/6/6.out -O obj lib/sys/sys+plan9-x64.myr
	$pwd/muse/6.out -o obj/lib/sys/libsys.use -p sys obj/lib/sys/sys.use obj/lib/sys/systypes.use obj/lib/sys/ifreq.use
	6a -o obj/lib/sys/util.6 lib/sys/util+plan9-x64.s
	ar u obj/lib/sys/libsys.a obj/lib/sys/sys.6 obj/lib/sys/util.6 obj/lib/sys/systypes.6 obj/lib/sys/ifreq.6 obj/lib/sys/syscall.6
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/types+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/pledge.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/types.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/strfind.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/memops.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/clear.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/sleq.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/hassuffix.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/backtrace+x64.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/units.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/cstrconv.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/syswrap-ss+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/sleep.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/syswrap+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/mkpath.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/now.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/consts.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/die.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/slcp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/chartype.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/utf.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/cmp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/sort.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/search.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/hasprefix.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/chomp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/striter.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/intparse.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/strstrip.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/introspect.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/varargs.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/threadhooks.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/bytealloc.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/alloc.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/slurp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/mk.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/slput.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/htab.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/slpush.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/strsplit.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/strbuf.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/sldup.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/bigint.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/fltparse.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/fltfmt.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/dirname.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/dir+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/diriter.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/fndup.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/strjoin.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/getcwd.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/slpop.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/sljoin.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/strreplace.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/getint.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/hashfuncs.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/bitset.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/putint.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/readall.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/blat.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/writeall.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/fmt.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/listen+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/env+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/execvp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/assert.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/rand.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/wait+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/spork.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/pathjoin.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/dial+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/mktemp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/optparse.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/netaddr.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/ipparse.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/fmtfuncs.myr
	$pwd/6/6.out -O obj -I obj/lib/sys lib/std/try.myr
	ar u obj/lib/std/libstd.a obj/lib/std/resolve.6 obj/lib/std/result.6 obj/lib/std/try.6 obj/lib/std/ipparse.6 obj/lib/std/alloc.6 obj/lib/std/sleq.6 obj/lib/std/putint.6 obj/lib/std/sljoin.6 obj/lib/std/slpop.6 obj/lib/std/syswrap.6 obj/lib/std/getint.6 obj/lib/std/strsplit.6 obj/lib/std/slfill.6 obj/lib/std/writeall.6 obj/lib/std/fltfmt.6 obj/lib/std/hasprefix.6 obj/lib/std/swap.6 obj/lib/std/fmt.6 obj/lib/std/netaddr.6 obj/lib/std/varargs.6 obj/lib/std/diriter.6 obj/lib/std/getcwd.6 obj/lib/std/blat.6 obj/lib/std/optparse.6 obj/lib/std/pathjoin.6 obj/lib/std/readall.6 obj/lib/std/strjoin.6 obj/lib/std/threadhooks.6 obj/lib/std/sjlj.6 obj/lib/std/extremum.6 obj/lib/std/endian.6 obj/lib/std/rand.6 obj/lib/std/sldup.6 obj/lib/std/sleep.6 obj/lib/std/wait.6 obj/lib/std/introspect.6 obj/lib/std/fltparse.6 obj/lib/std/fndup.6 obj/lib/std/strbuf.6 obj/lib/std/strreplace.6 obj/lib/std/assert.6 obj/lib/std/spork.6 obj/lib/std/slpush.6 obj/lib/std/strstrip.6 obj/lib/std/htab.6 obj/lib/std/hashfuncs.6 obj/lib/std/slput.6 obj/lib/std/sjlj-impl.6 obj/lib/std/fltbits.6 obj/lib/std/striter.6 obj/lib/std/types.6 obj/lib/std/cstrconv.6 obj/lib/std/units.6 obj/lib/std/backtrace.6 obj/lib/std/syswrap-ss.6 obj/lib/std/die.6 obj/lib/std/mk.6 obj/lib/std/hassuffix.6 obj/lib/std/memops-impl.6 obj/lib/std/pledge.6 obj/lib/std/utf.6 obj/lib/std/slurp.6 obj/lib/std/bytealloc.6 obj/lib/std/mktemp.6 obj/lib/std/consts.6 obj/lib/std/chomp.6 obj/lib/std/dir.6 obj/lib/std/search.6 obj/lib/std/memops.6 obj/lib/std/fmtfuncs.6 obj/lib/std/strfind.6 obj/lib/std/env.6 obj/lib/std/dirname.6 obj/lib/std/clear.6 obj/lib/std/listen.6 obj/lib/std/sort.6 obj/lib/std/cmp.6 obj/lib/std/now.6 obj/lib/std/intparse.6 obj/lib/std/traits.6 obj/lib/std/mkpath.6 obj/lib/std/option.6 obj/lib/std/dial.6 obj/lib/std/errno.6 obj/lib/std/chartype.6 obj/lib/std/bigint.6 obj/lib/std/bitset.6 obj/lib/std/getbp.6 obj/lib/std/slcp.6 obj/lib/std/execvp.6
	$pwd/muse/6.out -o obj/lib/std/libstd.use -p std obj/lib/std/resolve.use obj/lib/std/result.use obj/lib/std/try.use obj/lib/std/ipparse.use obj/lib/std/alloc.use obj/lib/std/sleq.use obj/lib/std/putint.use obj/lib/std/sljoin.use obj/lib/std/slpop.use obj/lib/std/syswrap.use obj/lib/std/getint.use obj/lib/std/strsplit.use obj/lib/std/slfill.use obj/lib/std/writeall.use obj/lib/std/fltfmt.use obj/lib/std/hasprefix.use obj/lib/std/swap.use obj/lib/std/fmt.use obj/lib/std/netaddr.use obj/lib/std/varargs.use obj/lib/std/diriter.use obj/lib/std/getcwd.use obj/lib/std/blat.use obj/lib/std/optparse.use obj/lib/std/pathjoin.use obj/lib/std/readall.use obj/lib/std/strjoin.use obj/lib/std/threadhooks.use obj/lib/std/sjlj.use obj/lib/std/extremum.use obj/lib/std/endian.use obj/lib/std/rand.use obj/lib/std/sldup.use obj/lib/std/sleep.use obj/lib/std/wait.use obj/lib/std/introspect.use obj/lib/std/fltparse.use obj/lib/std/fndup.use obj/lib/std/strbuf.use obj/lib/std/strreplace.use obj/lib/std/assert.use obj/lib/std/spork.use obj/lib/std/slpush.use obj/lib/std/strstrip.use obj/lib/std/htab.use obj/lib/std/hashfuncs.use obj/lib/std/slput.use obj/lib/std/fltbits.use obj/lib/std/striter.use obj/lib/std/types.use obj/lib/std/cstrconv.use obj/lib/std/units.use obj/lib/std/backtrace.use obj/lib/std/syswrap-ss.use obj/lib/std/die.use obj/lib/std/mk.use obj/lib/std/hassuffix.use obj/lib/std/pledge.use obj/lib/std/utf.use obj/lib/std/slurp.use obj/lib/std/bytealloc.use obj/lib/std/mktemp.use obj/lib/std/consts.use obj/lib/std/chomp.use obj/lib/std/dir.use obj/lib/std/search.use obj/lib/std/memops.use obj/lib/std/fmtfuncs.use obj/lib/std/strfind.use obj/lib/std/env.use obj/lib/std/dirname.use obj/lib/std/clear.use obj/lib/std/listen.use obj/lib/std/sort.use obj/lib/std/cmp.use obj/lib/std/now.use obj/lib/std/intparse.use obj/lib/std/traits.use obj/lib/std/mkpath.use obj/lib/std/option.use obj/lib/std/dial.use obj/lib/std/errno.use obj/lib/std/chartype.use obj/lib/std/bigint.use obj/lib/std/bitset.use obj/lib/std/slcp.use obj/lib/std/execvp.use
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/types.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/regex/types.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/regex/interp.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/types.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/bio.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/iter.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/mem.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/fd.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/geti.myr
	$pwd/6/6.out -O obj -I obj/lib/std -I obj/lib/sys lib/bio/puti.myr
	ar u obj/lib/bio/libbio.a obj/lib/bio/puti.6 obj/lib/bio/geti.6 obj/lib/bio/fd.6 obj/lib/bio/mem.6 obj/lib/bio/bio.6 obj/lib/bio/types.6 obj/lib/bio/iter.6
	$pwd/muse/6.out -o obj/lib/bio/libbio.use -p bio obj/lib/bio/puti.use obj/lib/bio/geti.use obj/lib/bio/fd.use obj/lib/bio/mem.use obj/lib/bio/bio.use obj/lib/bio/types.use obj/lib/bio/iter.use
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/tls+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/spawn+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/ncpu+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/atomic.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/waitgrp.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/sem+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/future.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/do.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/mutex+plan9.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/rwlock.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/condvar.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/queue.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/thread/hookstd.myr
	ar u obj/lib/thread/libthread.a obj/lib/thread/mutex.6 obj/lib/thread/future.6 obj/lib/thread/atomic-impl.6 obj/lib/thread/atomic.6 obj/lib/thread/hookstd.6 obj/lib/thread/sem.6 obj/lib/thread/do.6 obj/lib/thread/condvar.6 obj/lib/thread/rwlock.6 obj/lib/thread/common.6 obj/lib/thread/ncpu.6 obj/lib/thread/waitgrp.6 obj/lib/thread/tls.6 obj/lib/thread/spawn.6 obj/lib/thread/types.6 obj/lib/thread/queue.6
	$pwd/muse/6.out -o obj/lib/thread/libthread.use -p thread obj/lib/thread/mutex.use obj/lib/thread/future.use obj/lib/thread/atomic.use obj/lib/thread/hookstd.use obj/lib/thread/sem.use obj/lib/thread/do.use obj/lib/thread/condvar.use obj/lib/thread/rwlock.use obj/lib/thread/common.use obj/lib/thread/ncpu.use obj/lib/thread/waitgrp.use obj/lib/thread/tls.use obj/lib/thread/spawn.use obj/lib/thread/types.use obj/lib/thread/queue.use
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/opts.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/syssel.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/libs.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/util.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/build.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/install.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/parse.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/regex/ranges.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std lib/regex/compile.myr
	ar u obj/lib/regex/libregex.a obj/lib/regex/interp.6 obj/lib/regex/ranges.6 obj/lib/regex/types.6 obj/lib/regex/compile.6
	$pwd/muse/6.out -o obj/lib/regex/libregex.use -p regex obj/lib/regex/interp.use obj/lib/regex/ranges.use obj/lib/regex/types.use obj/lib/regex/compile.use
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/subtest.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/test.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/deps.myr
	$pwd/6/6.out -O obj -I obj/lib/sys -I obj/lib/std -I obj/lib/bio -I obj/lib/regex -I obj/lib/thread mbld/main.myr
	6l -l -o obj/mbld/mbld $pwd/rt/_myrrt.6 obj/mbld/deps.6 obj/mbld/main.6 obj/mbld/util.6 obj/mbld/cpufeatures.6 obj/mbld/libs.6 obj/mbld/syssel.6 obj/mbld/config.6 obj/mbld/opts.6 obj/mbld/subtest.6 obj/mbld/types.6 obj/mbld/test.6 obj/mbld/install.6 obj/mbld/parse.6 obj/mbld/build.6 obj/lib/thread/libthread.a obj/lib/bio/libbio.a obj/lib/regex/libregex.a obj/lib/std/libstd.a obj/lib/sys/libsys.a
true
