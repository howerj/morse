# Morse code Encoder/Decoder

* Author: Richard James Howe
* License: The Unlicense
* Email: howe.r.j.89@gmail.com
* Repo: https://github.com/howerj/morse

This application is a simple [Morse Code](https://en.wikipedia.org/wiki/Morse_code) 
encoder and decoder. It requires a [C compiler](https://gcc.gnu.org/), 
[GNU Make](https://www.gnu.org/software/make/) and [git](https://git-scm.com/) to build
(well git is only needed to clean the project properly after building).

The implementation is a [Header Only Library](https://en.wikipedia.org/wiki/Header-only),
with a few a structure that allows you to get around most of the potential deficiencies
of header only libraries (that is, you can make all of the functions be
"static" and "inline" if you want, or you can turn this header into a normal
library).

To build, type "make".

To run, either:

	./morse encode HELLO

Or:

	./morse decode .... . .-.. .-.. --- 

To bring up a help message just run:

	./morse

A non-zero value is returned on error.

The program is written in portable [C99](https://en.wikipedia.org/wiki/C99)
and should work, compile and build on many platforms but the program has
only been tested with GCC, GNU Make (no effort has been made to ensure
the makefile is portable) and on a 64-bit Linux.

