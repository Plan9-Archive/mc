#!/bin/sh
# This script is generated by genbootstrap.sh
# to regenerate, run "make bootstrap"
pwd=`pwd`
echo 	cd $pwd/lib/sys;	cd $pwd/lib/sys
echo 	$pwd/6/6.out	systypes.myr ;	$pwd/6/6.out	systypes.myr 
echo 	$pwd/6/6.out	sys+plan9-x64.myr ;	$pwd/6/6.out	sys+plan9-x64.myr 
echo 	$pwd/6/6.out	ifreq+plan9.myr ;	$pwd/6/6.out	ifreq+plan9.myr 
echo 	6a	-o util.6 util+plan9-x64.s ;	6a	-o util.6 util+plan9-x64.s 
echo 	6a	-o syscall.6 syscall+plan9-x64.s ;	6a	-o syscall.6 syscall+plan9-x64.s 
echo 	$pwd/muse/6.out	-o sys sys.use ifreq.use systypes.use ;	$pwd/muse/6.out	-o sys sys.use ifreq.use systypes.use 
echo 	ar	vu libsys.a sys.6 ifreq.6 syscall.6 systypes.6 util.6 ;	ar	vu libsys.a sys.6 ifreq.6 syscall.6 systypes.6 util.6 
echo 	cd $pwd/lib/std;	cd $pwd/lib/std
echo 	$pwd/6/6.out	-I ../sys -I . option.myr ;	$pwd/6/6.out	-I ../sys -I . option.myr 
echo 	$pwd/6/6.out	-I ../sys -I . types.myr ;	$pwd/6/6.out	-I ../sys -I . types.myr 
echo 	$pwd/6/6.out	-I ../sys -I . errno+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . errno+plan9.myr 
echo 	$pwd/6/6.out	-I ../sys -I . result.myr ;	$pwd/6/6.out	-I ../sys -I . result.myr 
echo 	$pwd/6/6.out	-I ../sys -I . cstrconv.myr ;	$pwd/6/6.out	-I ../sys -I . cstrconv.myr 
echo 	$pwd/6/6.out	-I ../sys -I . strfind.myr ;	$pwd/6/6.out	-I ../sys -I . strfind.myr 
echo 	$pwd/6/6.out	-I ../sys -I . syswrap+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . syswrap+plan9.myr 
echo 	$pwd/6/6.out	-I ../sys -I . die.myr ;	$pwd/6/6.out	-I ../sys -I . die.myr 
echo 	$pwd/6/6.out	-I ../sys -I . striter.myr ;	$pwd/6/6.out	-I ../sys -I . striter.myr 
echo 	$pwd/6/6.out	-I ../sys -I . sleq.myr ;	$pwd/6/6.out	-I ../sys -I . sleq.myr 
echo 	$pwd/6/6.out	-I ../sys -I . hassuffix.myr ;	$pwd/6/6.out	-I ../sys -I . hassuffix.myr 
echo 	$pwd/6/6.out	-I ../sys -I . extremum.myr ;	$pwd/6/6.out	-I ../sys -I . extremum.myr 
echo 	$pwd/6/6.out	-I ../sys -I . units.myr ;	$pwd/6/6.out	-I ../sys -I . units.myr 
echo 	$pwd/6/6.out	-I ../sys -I . memops.myr ;	$pwd/6/6.out	-I ../sys -I . memops.myr 
echo 	$pwd/6/6.out	-I ../sys -I . alloc.myr ;	$pwd/6/6.out	-I ../sys -I . alloc.myr 
echo 	$pwd/6/6.out	-I ../sys -I . chartype.myr ;	$pwd/6/6.out	-I ../sys -I . chartype.myr 
echo 	$pwd/6/6.out	-I ../sys -I . utf.myr ;	$pwd/6/6.out	-I ../sys -I . utf.myr 
echo 	$pwd/6/6.out	-I ../sys -I . cmp.myr ;	$pwd/6/6.out	-I ../sys -I . cmp.myr 
echo 	$pwd/6/6.out	-I ../sys -I . hasprefix.myr ;	$pwd/6/6.out	-I ../sys -I . hasprefix.myr 
echo 	$pwd/6/6.out	-I ../sys -I . slcp.myr ;	$pwd/6/6.out	-I ../sys -I . slcp.myr 
echo 	$pwd/6/6.out	-I ../sys -I . sldup.myr ;	$pwd/6/6.out	-I ../sys -I . sldup.myr 
echo 	$pwd/6/6.out	-I ../sys -I . slfill.myr ;	$pwd/6/6.out	-I ../sys -I . slfill.myr 
echo 	$pwd/6/6.out	-I ../sys -I . slpush.myr ;	$pwd/6/6.out	-I ../sys -I . slpush.myr 
echo 	$pwd/6/6.out	-I ../sys -I . bigint.myr ;	$pwd/6/6.out	-I ../sys -I . bigint.myr 
echo 	$pwd/6/6.out	-I ../sys -I . fltbits.myr ;	$pwd/6/6.out	-I ../sys -I . fltbits.myr 
echo 	$pwd/6/6.out	-I ../sys -I . strbuf.myr ;	$pwd/6/6.out	-I ../sys -I . strbuf.myr 
echo 	$pwd/6/6.out	-I ../sys -I . fltfmt.myr ;	$pwd/6/6.out	-I ../sys -I . fltfmt.myr 
echo 	$pwd/6/6.out	-I ../sys -I . hashfuncs.myr ;	$pwd/6/6.out	-I ../sys -I . hashfuncs.myr 
echo 	$pwd/6/6.out	-I ../sys -I . htab.myr ;	$pwd/6/6.out	-I ../sys -I . htab.myr 
echo 	$pwd/6/6.out	-I ../sys -I . introspect.myr ;	$pwd/6/6.out	-I ../sys -I . introspect.myr 
echo 	$pwd/6/6.out	-I ../sys -I . intparse.myr ;	$pwd/6/6.out	-I ../sys -I . intparse.myr 
echo 	$pwd/6/6.out	-I ../sys -I . strsplit.myr ;	$pwd/6/6.out	-I ../sys -I . strsplit.myr 
echo 	$pwd/6/6.out	-I ../sys -I . syswrap-ss+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . syswrap-ss+plan9.myr 
echo 	$pwd/6/6.out	-I ../sys -I . varargs.myr ;	$pwd/6/6.out	-I ../sys -I . varargs.myr 
echo 	$pwd/6/6.out	-I ../sys -I . fmt.myr ;	$pwd/6/6.out	-I ../sys -I . fmt.myr 
echo 	$pwd/6/6.out	-I ../sys -I . assert.myr ;	$pwd/6/6.out	-I ../sys -I . assert.myr 
echo 	$pwd/6/6.out	-I ../sys -I . now.myr ;	$pwd/6/6.out	-I ../sys -I . now.myr 
echo 	$pwd/6/6.out	-I ../sys -I . rand.myr ;	$pwd/6/6.out	-I ../sys -I . rand.myr 
echo 	$pwd/6/6.out	-I ../sys -I . sljoin.myr ;	$pwd/6/6.out	-I ../sys -I . sljoin.myr 
echo 	$pwd/6/6.out	-I ../sys -I . slurp.myr ;	$pwd/6/6.out	-I ../sys -I . slurp.myr 
echo 	$pwd/6/6.out	-I ../sys -I . dirname.myr ;	$pwd/6/6.out	-I ../sys -I . dirname.myr 
echo 	$pwd/6/6.out	-I ../sys -I . optparse.myr ;	$pwd/6/6.out	-I ../sys -I . optparse.myr 
echo 	$pwd/6/6.out	-I ../sys -I . ipparse.myr ;	$pwd/6/6.out	-I ../sys -I . ipparse.myr 
echo 	$pwd/6/6.out	-I ../sys -I . env+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . env+plan9.myr 
echo 	$pwd/6/6.out	-I ../sys -I . execvp.myr ;	$pwd/6/6.out	-I ../sys -I . execvp.myr 
echo 	$pwd/6/6.out	-I ../sys -I . slput.myr ;	$pwd/6/6.out	-I ../sys -I . slput.myr 
echo 	$pwd/6/6.out	-I ../sys -I . spork.myr ;	$pwd/6/6.out	-I ../sys -I . spork.myr 
echo 	$pwd/6/6.out	-I ../sys -I . getint.myr ;	$pwd/6/6.out	-I ../sys -I . getint.myr 
echo 	$pwd/6/6.out	-I ../sys -I . blat.myr ;	$pwd/6/6.out	-I ../sys -I . blat.myr 
echo 	$pwd/6/6.out	-I ../sys -I . mktemp.myr ;	$pwd/6/6.out	-I ../sys -I . mktemp.myr 
echo 	$pwd/6/6.out	-I ../sys -I . writeall.myr ;	$pwd/6/6.out	-I ../sys -I . writeall.myr 
echo 	$pwd/6/6.out	-I ../sys -I . clear.myr ;	$pwd/6/6.out	-I ../sys -I . clear.myr 
echo 	$pwd/6/6.out	-I ../sys -I . wait+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . wait+plan9.myr 
echo 	$pwd/6/6.out	-I ../sys -I . strjoin.myr ;	$pwd/6/6.out	-I ../sys -I . strjoin.myr 
echo 	$pwd/6/6.out	-I ../sys -I . mk.myr ;	$pwd/6/6.out	-I ../sys -I . mk.myr 
echo 	$pwd/6/6.out	-I ../sys -I . fndup.myr ;	$pwd/6/6.out	-I ../sys -I . fndup.myr 
echo 	$pwd/6/6.out	-I ../sys -I . putint.myr ;	$pwd/6/6.out	-I ../sys -I . putint.myr 
echo 	$pwd/6/6.out	-I ../sys -I . mkpath.myr ;	$pwd/6/6.out	-I ../sys -I . mkpath.myr 
echo 	$pwd/6/6.out	-I ../sys -I . resolve+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . resolve+plan9.myr 
echo 	$pwd/6/6.out	-I ../sys -I . pathjoin.myr ;	$pwd/6/6.out	-I ../sys -I . pathjoin.myr 
echo 	$pwd/6/6.out	-I ../sys -I . slpop.myr ;	$pwd/6/6.out	-I ../sys -I . slpop.myr 
echo 	$pwd/6/6.out	-I ../sys -I . bitset.myr ;	$pwd/6/6.out	-I ../sys -I . bitset.myr 
echo 	$pwd/6/6.out	-I ../sys -I . fmtfuncs.myr ;	$pwd/6/6.out	-I ../sys -I . fmtfuncs.myr 
echo 	$pwd/6/6.out	-I ../sys -I . strstrip.myr ;	$pwd/6/6.out	-I ../sys -I . strstrip.myr 
echo 	$pwd/6/6.out	-I ../sys -I . try.myr ;	$pwd/6/6.out	-I ../sys -I . try.myr 
echo 	$pwd/6/6.out	-I ../sys -I . sort.myr ;	$pwd/6/6.out	-I ../sys -I . sort.myr 
echo 	$pwd/6/6.out	-I ../sys -I . search.myr ;	$pwd/6/6.out	-I ../sys -I . search.myr 
echo 	$pwd/6/6.out	-I ../sys -I . endian.myr ;	$pwd/6/6.out	-I ../sys -I . endian.myr 
echo 	$pwd/6/6.out	-I ../sys -I . getcwd.myr ;	$pwd/6/6.out	-I ../sys -I . getcwd.myr 
echo 	$pwd/6/6.out	-I ../sys -I . swap.myr ;	$pwd/6/6.out	-I ../sys -I . swap.myr 
echo 	$pwd/6/6.out	-I ../sys -I . dial+plan9.myr ;	$pwd/6/6.out	-I ../sys -I . dial+plan9.myr 
echo 	6a	-o memops-impl.6 memops-impl+plan9-x64.s ;	6a	-o memops-impl.6 memops-impl+plan9-x64.s 
echo 	$pwd/muse/6.out	-o std fmtfuncs.use fmt.use try.use pathjoin.use strjoin.use syswrap-ss.use sljoin.use slpush.use strstrip.use htab.use now.use getcwd.use rand.use env.use slurp.use varargs.use strbuf.use clear.use slput.use strsplit.use introspect.use resolve.use alloc.use optparse.use memops.use fltbits.use sldup.use fltfmt.use extremum.use option.use slcp.use writeall.use putint.use sort.use blat.use mk.use errno.use hassuffix.use execvp.use swap.use ipparse.use types.use slpop.use strfind.use utf.use cstrconv.use search.use die.use units.use wait.use result.use bitset.use intparse.use hasprefix.use mkpath.use getint.use syswrap.use dirname.use sleq.use endian.use spork.use dial.use assert.use cmp.use chartype.use bigint.use hashfuncs.use slfill.use fndup.use ;	$pwd/muse/6.out	-o std fmtfuncs.use fmt.use try.use pathjoin.use strjoin.use syswrap-ss.use sljoin.use slpush.use strstrip.use htab.use now.use getcwd.use rand.use env.use slurp.use varargs.use strbuf.use clear.use slput.use strsplit.use introspect.use resolve.use alloc.use optparse.use memops.use fltbits.use sldup.use fltfmt.use extremum.use option.use slcp.use writeall.use putint.use sort.use blat.use mk.use errno.use hassuffix.use execvp.use swap.use ipparse.use types.use slpop.use strfind.use utf.use cstrconv.use search.use die.use units.use wait.use result.use bitset.use intparse.use hasprefix.use mkpath.use getint.use syswrap.use dirname.use sleq.use endian.use spork.use dial.use assert.use cmp.use chartype.use bigint.use striter.use hashfuncs.use slfill.use fndup.use mktemp.use
echo 	ar	vu libstd.a fmtfuncs.6 fmt.6 try.6 pathjoin.6 strjoin.6 syswrap-ss.6 sljoin.6 slpush.6 strstrip.6 htab.6 now.6 getcwd.6 rand.6 env.6 slurp.6 varargs.6 strbuf.6 clear.6 slput.6 strsplit.6 introspect.6 resolve.6 alloc.6 optparse.6 memops.6 fltbits.6 sldup.6 fltfmt.6 extremum.6 option.6 slcp.6 writeall.6 putint.6 sort.6 blat.6 mk.6 errno.6 hassuffix.6 execvp.6 swap.6 ipparse.6 types.6 slpop.6 strfind.6 utf.6 cstrconv.6 search.6 die.6 units.6 wait.6 result.6 bitset.6 intparse.6 hasprefix.6 mkpath.6 getint.6 syswrap.6 dirname.6 sleq.6 endian.6 spork.6 dial.6 assert.6 cmp.6 chartype.6 memops-impl.6 bigint.6 hashfuncs.6 slfill.6 fndup.6 ;	ar	vu libstd.a fmtfuncs.6 fmt.6 try.6 pathjoin.6 strjoin.6 syswrap-ss.6 sljoin.6 slpush.6 strstrip.6 htab.6 now.6 getcwd.6 rand.6 env.6 slurp.6 varargs.6 strbuf.6 clear.6 slput.6 strsplit.6 introspect.6 resolve.6 alloc.6 optparse.6 memops.6 fltbits.6 sldup.6 fltfmt.6 extremum.6 option.6 slcp.6 writeall.6 putint.6 sort.6 blat.6 mk.6 errno.6 hassuffix.6 execvp.6 swap.6 ipparse.6 types.6 slpop.6 strfind.6 utf.6 cstrconv.6 search.6 die.6 units.6 wait.6 result.6 bitset.6 intparse.6 hasprefix.6 mkpath.6 getint.6 syswrap.6 dirname.6 sleq.6 endian.6 spork.6 dial.6 assert.6 cmp.6 chartype.6 memops-impl.6 bigint.6 hashfuncs.6 slfill.6 fndup.6 mktemp.6 striter.6
echo 	cd $pwd/lib/regex;	cd $pwd/lib/regex
echo 	$pwd/6/6.out	-I ../std -I ../sys types.myr ;	$pwd/6/6.out	-I ../std -I ../sys types.myr 
echo 	$pwd/6/6.out	-I ../std -I ../sys interp.myr ;	$pwd/6/6.out	-I ../std -I ../sys interp.myr 
echo 	$pwd/6/6.out	-I ../std -I ../sys ranges.myr ;	$pwd/6/6.out	-I ../std -I ../sys ranges.myr 
echo 	$pwd/6/6.out	-I ../std -I ../sys compile.myr ;	$pwd/6/6.out	-I ../std -I ../sys compile.myr 
echo 	$pwd/muse/6.out	-o regex interp.use types.use compile.use ranges.use ;	$pwd/muse/6.out	-o regex interp.use types.use compile.use ranges.use 
echo 	ar	vu libregex.a interp.6 types.6 compile.6 ranges.6 ;	ar	vu libregex.a interp.6 types.6 compile.6 ranges.6 
echo 	cd $pwd/lib/bio;	cd $pwd/lib/bio
echo 	$pwd/6/6.out	-I ../sys -I ../std bio.myr ;	$pwd/6/6.out	-I ../sys -I ../std bio.myr 
echo 	$pwd/6/6.out	-I ../sys -I ../std puti.myr ;	$pwd/6/6.out	-I ../sys -I ../std puti.myr 
echo 	$pwd/6/6.out	-I ../sys -I ../std geti.myr ;	$pwd/6/6.out	-I ../sys -I ../std geti.myr 
echo 	$pwd/muse/6.out	-o bio puti.use bio.use geti.use ;	$pwd/muse/6.out	-o bio puti.use bio.use geti.use 
echo 	ar	vu libbio.a puti.6 bio.6 geti.6 ;	ar	vu libbio.a puti.6 bio.6 geti.6 
echo 	cd $pwd/mbld;	cd $pwd/mbld
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys config+plan9-x64.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys config+plan9-x64.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys opts.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys opts.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys types.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys types.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys util.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys util.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys deps.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys deps.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys syssel.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys syssel.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys parse.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys parse.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys build.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys build.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys install.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys install.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys clean.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys clean.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys test.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys test.myr 
echo 	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys main.myr ;	$pwd/6/6.out	-I ../lib/regex -I ../lib/bio -I ../lib/std -I ../lib/sys main.myr 
echo 	6l	-lo mbld $pwd/rt/_myrrt.6 clean.6 types.6 deps.6 syssel.6 util.6 parse.6 main.6 build.6 opts.6 config.6 install.6 test.6 ../lib/regex/libregex.a ../lib/bio/libbio.a ../lib/std/libstd.a ../lib/sys/libsys.a ;	6l	-lo mbld $pwd/rt/_myrrt.6 clean.6 types.6 deps.6 syssel.6 util.6 parse.6 main.6 build.6 opts.6 config.6 install.6 test.6 ../lib/regex/libregex.a ../lib/bio/libbio.a ../lib/std/libstd.a ../lib/sys/libsys.a 
