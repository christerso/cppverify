#ifndef CHEADERS_H__
#define CHEADERS_H__

// First try to loop through the include files we expect to be there
// Grab the _GLIBCXX_<HEADER>s to make sure they exist

// The standard C99 Header files

enum headers_t {
	H_COMPLEX = 0,
	H_INTTYPES,
	H_STDINT,
	H_FENV,
	H_STDBOOL,
	H_TGMATH,
	H_ISO646,
	H_WCHAR,
	H_WCTYPE,
	H_ASSERT,
	H_CTYPE,
	H_ERRNO,
	H_FLOAT,
	H_LIMITS,
	H_LOCATE,
	H_MATH,
	H_SETJUMP,
	H_SIGNAL,
	H_STDARG,
	H_STDDEF,
	H_STDIO,
	H_STDLIB,
	H_STRING,
	H_TIME
};

enum cstyles_t {
	C99,
	C95,
	C89
};

const char* c99_headers[6][2] = {
	{ "complex.h", "complex" },
	{ "inttypes.h", "cinttypes" },
	{ "stdint.h", "cstdint" },
	{ "fenv.h", "cfenv" },
	{ "stdbool.h", "cstdbool" },
	{ "tgmath.h", "ctgmath" }
};

const char* c94_95_headers[3][2] = {
	{ "iso646.h", "ciso646" },
	{ "wchar.h", "cwchar" },
	{ "wctype.h", "cwctype" }
};

const char* c89_90_headers[15][2] = {
	{ "assert.h", "cassert" },
	{ "ctype.h", "cctype" },
	{ "errno.h", "cerrno" },
	{ "float.h", "cfloat" },
	{ "limits.h", "climits" },
	{ "locale.h", "clocale" },
	{ "math.h", "cmath" },
	{ "setjmp.h", "csetjmp" },
	{ "signal.h", "csignal" },
	{ "stdarg.h", "cstdarg" },
	{ "stddef.h", "cstddef" },
	{ "stdio.h", "cstdio" },
	{ "stdlib.h", "cstdlib" },
	{ "string.h", "cstring" },
	{ "time.h", "ctime" }
};

#endif // CHEADERS_H__
