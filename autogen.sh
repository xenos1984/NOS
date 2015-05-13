#! /bin/sh

	touch Makefile.am configure.ac
{
	echo "running aclocal" >&2
	aclocal -I m4
} && {
	echo "running autoheader" >&2
	autoheader
} && {
	echo "running automake" >&2
	automake -a -f --foreign
} && {
	echo "running autoconf" >&2
	autoconf
} &&
	echo "autogen complete" >&2 ||
	echo "ERROR: autogen failed" >&2
