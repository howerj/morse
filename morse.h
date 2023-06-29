#ifndef MORSE_H
#define MORSE_H
#ifdef __cplusplus
extern "C" {
#endif
#define MORSE_AUTHOR  "Richard James Howe"
#define MORSE_LICENSE "The Unlicense"
#define MORSE_EMAIL   "howe.r.j.89@gmail.com"
#define MORSE_REPO    "https://github.com/howerj/morse"
#define MORSE_PROJECT "A Morse code encoder/decoder"
#define MORSE_VERSION 0x010000ul
/* See <https://stackoverflow.com/questions/28045172/morse-code-converter-in-c>
 * for the table encoding.
 *
 * For a different decoder: <https://nullprogram.com/blog/2020/12/31/>
 *
 * This could easily be turned into a header only library. The code
 * is quite small so could run on an AVR/Arduino or similar constrained
 * system. 
 *
 * This CODEC only handles the easy bit of decoding, which is the character
 * transformation, the difficult bit is determining whether a on/off pulse
 * is a dot or a dash, decoding spaces, cleaning the signal up, and handling
 * different operator speeds. 
 *
 * As an aside: 
 *
 * I have been interested in making a computer that uses a Morse code as
 * its primary interface, this would require a switch input and either an
 * Light Emitting Diode or a Buzzer as an output. It could perhaps interface
 * with a specially built Forth programming language interpreter. It could
 * then be used whilst walking around to take notes, perform calculations,
 * controlling audio playback and recording and more. This would require
 * dedicated hardware, or an Android phone application and a wireless button
 * to interface with it. It might require too much concentration to use. 
 *
 * There are just two functions that matter in this library, and this library
 * showcases how header-only libraries can contain the best of both worlds.
 * The only disadvantage is slower compile times (not much of a problem for
 * two reasons; this header and implementation is shorter than many headers
 * for traditional libraries, and I tend to use C, which has better compile 
 * times than C++).
 *
 * Header only libraries are sometimes accused of leading to bloat, this
 * may be true but it depends on how they are designed, the functions
 * they provide and how the compiler optimizations. Using "static inline"
 * functions *MAY* (or *MAY NOT*) lead to faster and *smaller* code as
 * opposed to traditional libraries, especially if the header only libraries
 * consists of very short functions. This should be measured and not
 * conjectured about. By offering the function definitions to the compiler
 * many more optimizations are possible, but that is not always a win.
 *
 * Due to the way this header library is structured it is possible to
 * instead integrate this header-only library into the build system in
 * such a way that the functions within it are defined in an object
 * file or library that can be accessed via the header. It is just a question
 * about setting the right macros in the right files. It is also possible
 * to turn on or off unit tests, and even a small command line application
 * to test the library out. The only persistent disadvantage is the
 * extra code within the header.
 *
 * The philosophy of header-only files, a phenomena of C and C++, one
 * which should not be replicated or needed in newer languages but
 * arises due to the peculiarities and history of C/C++, is embodied
 * with the following points:
 *
 * 1. The library should be small.
 * 2. The functions defined within should not depend on the C standard
 *    library (especially functions such as malloc, or anything that
 *    takes a FILE handle). If allocation or file like semantics are
 *    needed they should be passed in via callbacks.
 * 3. It should be easy to integrate the library into the build system.
 * 4. The library should expose a minimal interface and only do one
 *    thing.
 *
 * More points could be added. The second point is good practice for
 * any C library. This library abides by these points (it does include
 * <stdio.h> and use the functions within them, but only for an
 * optional demonstration program).
 *
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#ifndef MORSE_EXTERN
#define MORSE_EXTERN extern
#endif

#ifndef MORSE_API
#define MORSE_API
#endif

/* All these functions return negative on failure, and zero or positive on
 * success. `morse_decode` returns the decoded character or negative on
 * failure. `morse_tests` returns negative on failure and zero on success
 * or if the tests are compiled out. */
MORSE_EXTERN int morse_tests(void);
MORSE_EXTERN int morse_encode(unsigned char in, unsigned char out[/*static*/ 6 /* 6 = 5 chars + NUL*/]);
MORSE_EXTERN int morse_decode(const unsigned char *in); /* NUL terminate Morse code character as input */

#ifdef MORSE_IMPLEMENTATION

#ifndef MORSE_DOT
#define MORSE_DOT ('.')
#endif

#ifndef MORSE_DASH
#define MORSE_DASH ('-')
#endif

#ifndef MORSE_CODEC
/* This is a really neat way of encoding the Morse code table that can
 * be used for encoding and decoding. The table can be extended to
 * deal with numbers and other special characters. */
#define MORSE_CODEC "**ETIANMSURWDKGOHVF?L?PJBXCYZQ??"
#endif

#define MORSE_CODEC_LENGTH (sizeof(MORSE_CODEC) - 1)

/* A possible improvement to this would be to turn this into a state-machine
 * that accepts a single characters at a time until a character pops out
 * or an error condition is reached. 
 *
 * Other directions include:
 * - Operating on a string with a length.
 * - Parsing strings with embedded whitespace, returning the character
 *   parsed and a pointer into the string parsed so far.
 */
MORSE_API int morse_decode(const unsigned char *in) {
	assert(in);
	unsigned char out = '?';
	for (size_t n = 1, j = 0; n < MORSE_CODEC_LENGTH; j++) {
		unsigned char ch = in[j];
		if (ch == MORSE_DOT) {
			n = (n << 1);
		} else if (ch == MORSE_DASH) {
			n = (n << 1) + 1;
		} else if (ch == '\0') {
			out = n > MORSE_CODEC_LENGTH ? '?' : MORSE_CODEC[n];
			break;
		} else {
			return -1;
		}
	}
	return out;
}

static inline void morse_reverse_char_array(unsigned char * const r, const size_t length) {
	assert(r);
	const size_t last = length - 1;
	for (size_t i = 0; i < length / 2ul; i++) {
		const unsigned char t = r[i];
		r[i] = r[last - i];
		r[last - i] = t;
	}
}

MORSE_API int morse_encode(unsigned char in, unsigned char out[/*static*/ 6 /* 5 chars + NUL*/]) {
	assert(out);
	size_t pos = SIZE_MAX, i = 0, next = 0;
	for (i = 0; i < 6; i++) 
		out[i] = 0;
	for (i = 0; i < MORSE_CODEC_LENGTH; i++) {
		if (MORSE_CODEC[i] == in) {
			pos = i;
			break;
		}
	}
	if (pos == SIZE_MAX || MORSE_CODEC[pos] == '*' || MORSE_CODEC[pos] == '?') {
		return -1;
	}
	for (i = 0; (next = pos >> 1); i++) {
		out[i] = (pos & 1) ? MORSE_DASH : MORSE_DOT;
		pos = next;
	}
	morse_reverse_char_array(out, i);
	return 0;
}

#ifdef MORSE_TESTS
MORSE_API int morse_tests(void) {
	for (int ch = 'A'; ch <= 'Z'; ch++) {
		unsigned char buf[6] = { 0, };
		if (morse_encode(ch, buf) < 0)
			return -1;
		const int r = morse_decode(buf);
		if (r < 0)
			return -2;
		if (r != ch)
			return -ch;
	}
	return 0;
}
#else
MORSE_API int morse_tests(void) {
	return 0;
}
#endif /* MORSE_TESTS */

#ifdef MORSE_MAIN

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MORSE_STRINGIFY(X) #X
#define MORSE_XSTRINGIFY(X) MORSE_STRINGIFY(X)
#define MORSE_VERSION_STRING MORSE_XSTRINGIFY(MORSE_VERSION)

static int morse_usage(FILE *out, const char *arg0) {
	assert(out);
	assert(arg0);
	const char *fmt = "\
Usage:   %s encode|decode strings...\n\n\
Project: " MORSE_PROJECT "\n\
Author:  " MORSE_AUTHOR "\n\
License: " MORSE_LICENSE "\n\
Repo:    " MORSE_REPO "\n\
Email:   " MORSE_EMAIL "\n\
Version: " MORSE_VERSION_STRING "\n\n\
This utility returns zero on success an non-zero on failure.\n\
Errors a printed to stderr, and output to stdout. This codebook\n\
only includes the upper case alphabet.\n\nCharacters:\n\n";
	if (fprintf(out, fmt, arg0) < 0)
		return -1;
	for (int ch = 'A'; ch <= 'M'; ch++) {
		unsigned char buf1[6] = { 0, }, buf2[6] = { 0, };
		if (morse_encode(ch, buf1) < 0)
			return -1;
		if (morse_encode(ch + 13, buf2) < 0)
			return -1;
		if (fprintf(out, "\t\t%c %5s %c %5s\n", ch, buf1, ch + 13, buf2) < 0)
			return -1;
	}

	const char *tree = "\
        DIT or '.' <-- * --> DAH or '-'\n\
                /             \\\n\
               E               T\n\
             /   \\           /   \\\n\
           I       A       N       M\n\
          / \\     / \\     / \\     / \\\n\
         S   U   R   W   D   K   G   O\n\
        / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\\n\
        H V F ? L ? P J B X C Y Z Q ? ?\n";

	if (fprintf(out, "\n\nTree:\n\n%s\n\n", tree) < 0)
		return -1;
	return 0;
}

#undef MORSE_STRINGIFY
#undef MORSE_XSTRINGIFY
#undef MORSE_VERSION_STRING 

int main(int argc, char **argv) {
	int decode = -1;
	if (morse_tests() < 0)
		return 1;

	if (argc < 2) { 
		(void)morse_usage(stderr, argv[0]);
		return 2;
	}

	if (!strcmp(argv[1], "encode"))
		decode = 0;
	if (!strcmp(argv[1], "decode"))
		decode = 1;

	if (decode < 0) {
		(void)morse_usage(stderr, argv[0]);
		return 3;
	}

	if (decode) {
		for (int i = 2; i < argc; i++) {
			if (fprintf(stdout, "%c", morse_decode((unsigned char*)argv[i])) < 0)
				return 4;
		}
		if (fprintf(stdout, "\n") < 0)
			return 5;
		return 0;
	}

	for (int i = 2; i < argc; i++) {
		for (size_t j = 0; argv[i][j]; j++) {
			unsigned char out[6] = { 0, };
			if (morse_encode(toupper(argv[i][j]), out) < 0) {
				return 6;
			}
			if (fprintf(stdout, "%s ", out) < 0)
				return 7;
		}
		if (fprintf(stdout, "\n") < 0)
			return 8;
	}
	return 0;
}
#endif /* MORSE_MAIN */
#endif /* MORSE_IMPLEMENTATION */
#ifdef __cplusplus
}
#endif
#endif
