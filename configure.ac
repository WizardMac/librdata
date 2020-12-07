AC_INIT([rdata], [0.1.0-prerelease])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_SUBST([LIBRDATA_VERSION], [0.1.0-prerelease])

LT_INIT([disable-static])

AC_PROG_CC
AC_PROG_CC_STDC

AC_ARG_ENABLE([code-coverage], AS_HELP_STRING([--enable-code-coverage], [Enable code coverage profiling]), [code_coverage=yes], [code_coverage=no])
AC_ARG_ENABLE([sanitizers], AS_HELP_STRING([--enable-sanitizers], [Enable address sanitizing]),
    [SANITIZERS="-fsanitize=address,bool,float-cast-overflow,integer-divide-by-zero,return,returns-nonnull-attribute,shift-exponent,signed-integer-overflow,unreachable,vla-bound -fsanitize-coverage=trace-pc-guard,indirect-calls,trace-cmp"], [SANITIZERS=""])
AC_SUBST([SANITIZERS])

AM_ICONV

AC_CANONICAL_HOST
AS_CASE([$host],
	[*mingw*], [EXTRA_WARNINGS="-Wno-pedantic-ms-format -Wno-stringop-truncation"],
    [EXTRA_WARNINGS=""])
AC_SUBST([EXTRA_WARNINGS])

AS_CASE([$host],
    [*mingw*|*cygwin*], [EXTRA_LDFLAGS="$LTLIBICONV -no-undefined"],
    [EXTRA_LDFLAGS="$LTLIBICONV"])
AC_SUBST([EXTRA_LDFLAGS])

AC_CHECK_LIB([bz2], [BZ2_bzCompressInit], [true], [false])
AM_CONDITIONAL([HAVE_BZIP2], test "$ac_cv_lib_bz2_BZ2_bzCompressInit" = yes)

AC_CHECK_LIB([compression], [compression_stream_init], [true], [false])
AM_CONDITIONAL([HAVE_APPLE_COMPRESSION], test "$ac_cv_lib_compression_compression_stream_init" = yes)

AC_CHECK_LIB([lzma], [lzma_stream_decoder], [true], [false])
AM_CONDITIONAL([HAVE_LZMA], test "$ac_cv_lib_lzma_lzma_stream_decoder" = yes)

AC_CHECK_LIB([z], [deflate], [true], [false])
AM_CONDITIONAL([HAVE_ZLIB], test "$ac_cv_lib_z_deflate" = yes)

AM_CONDITIONAL([CODE_COVERAGE_ENABLED], test "x$code_coverage" = "xyes")
AC_CHECK_LIB([Fuzzer], [__libfuzzer_is_present], [true], [false], [-lstdc++])
AM_CONDITIONAL([HAVE_FUZZER], test "$ac_cv_lib_Fuzzer___libfuzzer_is_present" = yes)

AC_OUTPUT([Makefile])

AC_MSG_RESULT([
Configuration:

C compiler: $CC
CFLAGS: $CFLAGS

Host: $host
Extra warnings: $EXTRA_WARNINGS
Extra libs: $EXTRA_LIBS
Extra ld flags: $EXTRA_LDFLAGS])
