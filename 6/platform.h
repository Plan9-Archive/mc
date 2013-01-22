#if defined(__APPLE__) && defined(__MACH__)
/* for OSX */
#   define Asmcmd "as -g -o %s %s"
#   define Symprefix "_"
#else
/* Default to linux */
#   define Asmcmd "as -g -o %s %s"
#   define Symprefix ""
#endif
