#!/bin/sh
# This script is generated by genbootstrap.sh
# to regenerate, run "make bootstrap"
pwd=`pwd`
set -x
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/config.myr
	as -g -o lib/thread/exit.o lib/thread/exit+linux-x64.s
	as -g -o lib/thread/atomic-impl.o lib/thread/atomic-impl+x64.s
	as -g -o lib/std/getbp.o lib/std/getbp+posixy-x64.s
	$pwd/6/6m -I . -I lib/sys lib/std/option.myr
	$pwd/6/6m -I . -I lib/sys lib/std/traits.myr
	as -g -o lib/std/memops-impl.o lib/std/memops-impl+posixy-x64.s
	$pwd/6/6m -I . -I lib/sys lib/std/fltbits.myr
	as -g -o lib/std/sjlj-impl.o lib/std/sjlj-impl+posixy-x64.s
	$pwd/6/6m -I . -I lib/sys lib/std/endian.myr
	$pwd/6/6m -I . -I lib/sys lib/std/extremum.myr
	$pwd/6/6m -I . -I lib/sys lib/std/sjlj.myr
	$pwd/6/6m -I . -I lib/sys lib/std/swap.myr
	$pwd/6/6m -I . -I lib/sys lib/std/slfill.myr
	$pwd/6/6m -I . -I lib/sys lib/std/clear.myr
	$pwd/6/6m -I . -I lib/sys lib/std/result.myr
	as -g -o lib/sys/syscall.o lib/sys/syscall+linux-x64.s
	$pwd/6/6m lib/sys/systypes.myr
	as -g -o lib/sys/util.o lib/sys/util+posixy-x64.s
	$pwd/6/6m lib/sys/syserrno+linux.myr
	$pwd/6/6m lib/sys/sys+linux-x64.myr
	$pwd/6/6m lib/sys/ifreq+linux.myr
	ar -rcs lib/sys/libsys.a lib/sys/sys.o lib/sys/syserrno.o lib/sys/util.o lib/sys/systypes.o lib/sys/ifreq.o lib/sys/syscall.o
	$pwd/muse/muse -o lib/sys/libsys.use -p sys lib/sys/sys.use lib/sys/syserrno.use lib/sys/systypes.use lib/sys/ifreq.use
	$pwd/6/6m -I . -I lib/sys lib/std/errno.myr
	$pwd/6/6m -I . -I lib/sys lib/std/types.myr
	$pwd/6/6m -I . -I lib/sys lib/std/strfind.myr
	$pwd/6/6m -I . -I lib/sys lib/std/memops.myr
	$pwd/6/6m -I . -I lib/sys lib/std/sleq.myr
	$pwd/6/6m -I . -I lib/sys lib/std/hassuffix.myr
	$pwd/6/6m -I . -I lib/sys lib/std/syswrap-ss+linux.myr
	$pwd/6/6m -I . -I lib/sys lib/std/sleep.myr
	$pwd/6/6m -I . -I lib/sys lib/std/backtrace+x64.myr
	$pwd/6/6m -I . -I lib/sys lib/std/units.myr
	$pwd/6/6m -I . -I lib/sys lib/std/cstrconv.myr
	$pwd/6/6m -I . -I lib/sys lib/std/syswrap+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/mkpath.myr
	$pwd/6/6m -I . -I lib/sys lib/std/now.myr
	$pwd/6/6m -I . -I lib/sys lib/std/consts.myr
	$pwd/6/6m -I . -I lib/sys lib/std/die.myr
	$pwd/6/6m -I . -I lib/sys lib/std/slcp.myr
	$pwd/6/6m -I . -I lib/sys lib/std/chartype.myr
	$pwd/6/6m -I . -I lib/sys lib/std/utf.myr
	$pwd/6/6m -I . -I lib/sys lib/std/cmp.myr
	$pwd/6/6m -I . -I lib/sys lib/std/sort.myr
	$pwd/6/6m -I . -I lib/sys lib/std/search.myr
	$pwd/6/6m -I . -I lib/sys lib/std/hasprefix.myr
	$pwd/6/6m -I . -I lib/sys lib/std/chomp.myr
	$pwd/6/6m -I . -I lib/sys lib/std/striter.myr
	$pwd/6/6m -I . -I lib/sys lib/std/intparse.myr
	$pwd/6/6m -I . -I lib/sys lib/std/strstrip.myr
	$pwd/6/6m -I . -I lib/sys lib/std/introspect.myr
	$pwd/6/6m -I . -I lib/sys lib/std/varargs.myr
	$pwd/6/6m -I . -I lib/sys lib/std/wait+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/threadhooks.myr
	$pwd/6/6m -I . -I lib/sys lib/std/bytealloc.myr
	$pwd/6/6m -I . -I lib/sys lib/std/alloc.myr
	$pwd/6/6m -I . -I lib/sys lib/std/slurp.myr
	$pwd/6/6m -I . -I lib/sys lib/std/mk.myr
	$pwd/6/6m -I . -I lib/sys lib/std/slput.myr
	$pwd/6/6m -I . -I lib/sys lib/std/htab.myr
	$pwd/6/6m -I . -I lib/sys lib/std/slpush.myr
	$pwd/6/6m -I . -I lib/sys lib/std/strsplit.myr
	$pwd/6/6m -I . -I lib/sys lib/std/strbuf.myr
	$pwd/6/6m -I . -I lib/sys lib/std/sldup.myr
	$pwd/6/6m -I . -I lib/sys lib/std/bigint.myr
	$pwd/6/6m -I . -I lib/sys lib/std/fltparse.myr
	$pwd/6/6m -I . -I lib/sys lib/std/fltfmt.myr
	$pwd/6/6m -I . -I lib/sys lib/std/dirname.myr
	$pwd/6/6m -I . -I lib/sys lib/std/dir+linux.myr
	$pwd/6/6m -I . -I lib/sys lib/std/diriter.myr
	$pwd/6/6m -I . -I lib/sys lib/std/fndup.myr
	$pwd/6/6m -I . -I lib/sys lib/std/strjoin.myr
	$pwd/6/6m -I . -I lib/sys lib/std/getcwd.myr
	$pwd/6/6m -I . -I lib/sys lib/std/slpop.myr
	$pwd/6/6m -I . -I lib/sys lib/std/sljoin.myr
	$pwd/6/6m -I . -I lib/sys lib/std/getint.myr
	$pwd/6/6m -I . -I lib/sys lib/std/hashfuncs.myr
	$pwd/6/6m -I . -I lib/sys lib/std/bitset.myr
	$pwd/6/6m -I . -I lib/sys lib/std/putint.myr
	$pwd/6/6m -I . -I lib/sys lib/std/readall.myr
	$pwd/6/6m -I . -I lib/sys lib/std/blat.myr
	$pwd/6/6m -I . -I lib/sys lib/std/writeall.myr
	$pwd/6/6m -I . -I lib/sys lib/std/fmt.myr
	$pwd/6/6m -I . -I lib/sys lib/std/env+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/execvp.myr
	$pwd/6/6m -I . -I lib/sys lib/std/spork.myr
	$pwd/6/6m -I . -I lib/sys lib/std/assert.myr
	$pwd/6/6m -I . -I lib/sys lib/std/rand.myr
	$pwd/6/6m -I . -I lib/sys lib/std/pathjoin.myr
	$pwd/6/6m -I . -I lib/sys lib/std/mktemp.myr
	$pwd/6/6m -I . -I lib/sys lib/std/optparse.myr
	$pwd/6/6m -I . -I lib/sys lib/std/netaddr.myr
	$pwd/6/6m -I . -I lib/sys lib/std/ipparse.myr
	$pwd/6/6m -I . -I lib/sys lib/std/fmtfuncs.myr
	$pwd/6/6m -I . -I lib/sys lib/std/resolve+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/dialparse+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/dial+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/listen+posixy.myr
	$pwd/6/6m -I . -I lib/sys lib/std/try.myr
	ar -rcs lib/std/libstd.a lib/std/resolve.o lib/std/result.o lib/std/try.o lib/std/ipparse.o lib/std/alloc.o lib/std/sleq.o lib/std/putint.o lib/std/sljoin.o lib/std/slpop.o lib/std/syswrap.o lib/std/getint.o lib/std/strsplit.o lib/std/slfill.o lib/std/writeall.o lib/std/fltfmt.o lib/std/hasprefix.o lib/std/swap.o lib/std/fmt.o lib/std/netaddr.o lib/std/varargs.o lib/std/diriter.o lib/std/getcwd.o lib/std/blat.o lib/std/optparse.o lib/std/pathjoin.o lib/std/readall.o lib/std/strjoin.o lib/std/threadhooks.o lib/std/sjlj.o lib/std/extremum.o lib/std/endian.o lib/std/rand.o lib/std/sldup.o lib/std/sleep.o lib/std/wait.o lib/std/introspect.o lib/std/fltparse.o lib/std/fndup.o lib/std/strbuf.o lib/std/assert.o lib/std/spork.o lib/std/slpush.o lib/std/strstrip.o lib/std/htab.o lib/std/hashfuncs.o lib/std/slput.o lib/std/sjlj-impl.o lib/std/fltbits.o lib/std/striter.o lib/std/types.o lib/std/cstrconv.o lib/std/units.o lib/std/backtrace.o lib/std/syswrap-ss.o lib/std/die.o lib/std/mk.o lib/std/hassuffix.o lib/std/memops-impl.o lib/std/utf.o lib/std/slurp.o lib/std/dialparse.o lib/std/bytealloc.o lib/std/mktemp.o lib/std/consts.o lib/std/chomp.o lib/std/dir.o lib/std/search.o lib/std/memops.o lib/std/fmtfuncs.o lib/std/strfind.o lib/std/env.o lib/std/dirname.o lib/std/clear.o lib/std/listen.o lib/std/sort.o lib/std/cmp.o lib/std/now.o lib/std/intparse.o lib/std/traits.o lib/std/mkpath.o lib/std/option.o lib/std/dial.o lib/std/errno.o lib/std/chartype.o lib/std/bigint.o lib/std/bitset.o lib/std/getbp.o lib/std/slcp.o lib/std/execvp.o
	$pwd/muse/muse -o lib/std/libstd.use -p std lib/std/resolve.use lib/std/result.use lib/std/try.use lib/std/ipparse.use lib/std/alloc.use lib/std/sleq.use lib/std/putint.use lib/std/sljoin.use lib/std/slpop.use lib/std/syswrap.use lib/std/getint.use lib/std/strsplit.use lib/std/slfill.use lib/std/writeall.use lib/std/fltfmt.use lib/std/hasprefix.use lib/std/swap.use lib/std/fmt.use lib/std/netaddr.use lib/std/varargs.use lib/std/diriter.use lib/std/getcwd.use lib/std/blat.use lib/std/optparse.use lib/std/pathjoin.use lib/std/readall.use lib/std/strjoin.use lib/std/threadhooks.use lib/std/sjlj.use lib/std/extremum.use lib/std/endian.use lib/std/rand.use lib/std/sldup.use lib/std/sleep.use lib/std/wait.use lib/std/introspect.use lib/std/fltparse.use lib/std/fndup.use lib/std/strbuf.use lib/std/assert.use lib/std/spork.use lib/std/slpush.use lib/std/strstrip.use lib/std/htab.use lib/std/hashfuncs.use lib/std/slput.use lib/std/fltbits.use lib/std/striter.use lib/std/types.use lib/std/cstrconv.use lib/std/units.use lib/std/backtrace.use lib/std/syswrap-ss.use lib/std/die.use lib/std/mk.use lib/std/hassuffix.use lib/std/utf.use lib/std/slurp.use lib/std/dialparse.use lib/std/bytealloc.use lib/std/mktemp.use lib/std/consts.use lib/std/chomp.use lib/std/dir.use lib/std/search.use lib/std/memops.use lib/std/fmtfuncs.use lib/std/strfind.use lib/std/env.use lib/std/dirname.use lib/std/clear.use lib/std/listen.use lib/std/sort.use lib/std/cmp.use lib/std/now.use lib/std/intparse.use lib/std/traits.use lib/std/mkpath.use lib/std/option.use lib/std/dial.use lib/std/errno.use lib/std/chartype.use lib/std/bigint.use lib/std/bitset.use lib/std/slcp.use lib/std/execvp.use
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/types.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/regex/types.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/regex/interp.myr
	$pwd/6/6m -I lib/std -I lib/sys lib/bio/bio.myr
	$pwd/6/6m -I lib/std -I lib/sys lib/bio/iter.myr
	$pwd/6/6m -I lib/std -I lib/sys lib/bio/mem.myr
	$pwd/6/6m -I lib/std -I lib/sys lib/bio/geti.myr
	$pwd/6/6m -I lib/std -I lib/sys lib/bio/puti.myr
	ar -rcs lib/bio/libbio.a lib/bio/puti.o lib/bio/geti.o lib/bio/mem.o lib/bio/bio.o lib/bio/iter.o
	$pwd/muse/muse -o lib/bio/libbio.use -p bio lib/bio/puti.use lib/bio/geti.use lib/bio/mem.use lib/bio/bio.use lib/bio/iter.use
	$pwd/6/6m -I lib/sys -I lib/std lib/thread/spawn+linux.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/thread/ncpu+linux.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/thread/common.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/thread/atomic.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/thread/mutex+linux.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/thread/hookstd.myr
	ar -rcs lib/thread/libthread.a lib/thread/mutex.o lib/thread/atomic.o lib/thread/atomic-impl.o lib/thread/hookstd.o lib/thread/common.o lib/thread/ncpu.o lib/thread/exit.o lib/thread/spawn.o
	$pwd/muse/muse -o lib/thread/libthread.use -p thread lib/thread/mutex.use lib/thread/atomic.use lib/thread/hookstd.use lib/thread/common.use lib/thread/ncpu.use lib/thread/spawn.use
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/opts.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/syssel.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/libs.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/util.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/build.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/install.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/parse.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/regex/ranges.myr
	$pwd/6/6m -I lib/sys -I lib/std lib/regex/compile.myr
	ar -rcs lib/regex/libregex.a lib/regex/interp.o lib/regex/ranges.o lib/regex/types.o lib/regex/compile.o
	$pwd/muse/muse -o lib/regex/libregex.use -p regex lib/regex/interp.use lib/regex/ranges.use lib/regex/types.use lib/regex/compile.use
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/subtest.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/test.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/deps.myr
	$pwd/6/6m -I lib/sys -I lib/std -I lib/bio -I lib/regex -I lib/thread mbld/main.myr
	ld -o mbld/mbld $pwd/rt/_myrrt.o mbld/deps.o mbld/main.o mbld/util.o mbld/libs.o mbld/syssel.o mbld/config.o mbld/opts.o mbld/subtest.o mbld/types.o mbld/test.o mbld/install.o mbld/parse.o mbld/build.o -Llib/thread -lthread -Llib/bio -lbio -Llib/regex -lregex -Llib/std -lstd -Llib/sys -lsys
true
